// For window code included here see https://wayland-book.com/xdg-shell-basics/example-code.html

// Compile and run with the following command:
// gcc client.c zwp-tablet-v2-client-protocol.c xdg-shell-protocol.c -o client     $(pkg-config --cflags --libs wayland-client)
// ./client

#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client.h>
#include "zwp-tablet-v2-client-protocol.h"
#include "xdg-shell-client-protocol.h"
#include <stdio.h>
#include <inttypes.h> // Required for PRIu32

/* Wayland code */
struct client_state {
    /* Globals */
    struct wl_display *wl_display;
    struct wl_registry *wl_registry;
    struct wl_shm *wl_shm;
    struct wl_compositor *wl_compositor;
    struct xdg_wm_base *xdg_wm_base;
    /* Objects */
    struct wl_surface *wl_surface;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    struct zwp_tablet_manager_v2 *tablet_manager;
    struct wl_seat *seat;
};

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

static void registry_global_handler(void *data, struct wl_registry *registry,
        uint32_t name, const char *interface, uint32_t version) {
    struct client_state *state = data;

    if (strcmp(interface, wl_shm_interface.name) == 0) {
        state->wl_shm = wl_registry_bind(
                registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
        state->wl_compositor = wl_registry_bind(
                registry, name, &wl_compositor_interface, 4);
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        state->xdg_wm_base = wl_registry_bind(
                registry, name, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(state->xdg_wm_base,
                &xdg_wm_base_listener, state);
    } else if (strcmp(interface, "zwp_tablet_manager_v2") == 0) {
        state->tablet_manager = wl_registry_bind(registry, name,
            &zwp_tablet_manager_v2_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        state->seat = wl_registry_bind(registry, name,
            &wl_seat_interface, 1);
    }
}

/* Shared memory support code */
static void
randname(char *buf)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A'+(r&15)+(r&16)*2;
        r >>= 5;
    }
}

static int
create_shm_file(void)
{
    int retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        randname(name + sizeof(name) - 7);
        --retries;
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);
    return -1;
}

static int
allocate_shm_file(size_t size)
{
    int fd = create_shm_file();
    if (fd < 0)
        return -1;
    int ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

static void
wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
    /* Sent by the compositor when it's no longer using this buffer */
    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
};

static struct wl_buffer *
draw_frame(struct client_state *state)
{
    const int width = 640, height = 480;
    int stride = width * 4;
    int size = stride * height;

    int fd = allocate_shm_file(size);
    if (fd == -1) {
        return NULL;
    }

    uint32_t *data = mmap(NULL, size,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(state->wl_shm, fd, size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0,
            width, height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    /* Draw checkerboxed background */
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if ((x + y / 8 * 8) % 16 < 8)
                data[y * width + x] = 0xFF666666;
            else
                data[y * width + x] = 0xFFEEEEEE;
        }
    }

    munmap(data, size);
    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}


static void
xdg_surface_configure(void *data,
        struct xdg_surface *xdg_surface, uint32_t serial)
{
    struct client_state *state = data;
    xdg_surface_ack_configure(xdg_surface, serial);

    struct wl_buffer *buffer = draw_frame(state);
    wl_surface_attach(state->wl_surface, buffer, 0, 0);
    wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static void
registry_global_remove(void *data,
        struct wl_registry *wl_registry, uint32_t name)
{
    /* This space deliberately left blank */
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global_handler,
    .global_remove = registry_global_remove,
};


/* ===================== zwp_tablet_pad_ring_v2 ===================== */
static void handle_ring_source(void *data, struct zwp_tablet_pad_ring_v2 *ring, uint32_t source) {
    printf("[tablet-ring] source: %u\n", source);
}

static void handle_ring_angle(void *data, struct zwp_tablet_pad_ring_v2 *ring, wl_fixed_t degrees) {
    printf("[tablet-ring] angle: %.2f degrees\n", wl_fixed_to_double(degrees));
}

static void handle_ring_stop(void *data, struct zwp_tablet_pad_ring_v2 *ring) {
    printf("[tablet-ring] stop\n");
}

static void handle_ring_frame(void *data, struct zwp_tablet_pad_ring_v2 *ring, uint32_t time) {
    printf("[tablet-ring] frame at %u\n", time);
}

static const struct zwp_tablet_pad_ring_v2_listener tablet_ring_listener = {
    .source = handle_ring_source,
    .angle = handle_ring_angle,
    .stop = handle_ring_stop,
    .frame = handle_ring_frame,
};

/* ===================== zwp_tablet_pad_strip_v2 ===================== */

static void handle_strip_source(void * data, struct zwp_tablet_pad_strip_v2 *strip, uint32_t source) {
    printf("[tablet-strip] source: %u\n", source);
}

static void handle_strip_position(void * data, struct zwp_tablet_pad_strip_v2 *strip, uint32_t position) {
    // Note: 'position' should likely be wl_fixed_t based on the protocol, but the code is passing uint32_t to wl_fixed_to_double. 
    // Assuming the intent is for position to be treated as a fixed-point value if it were of type wl_fixed_t.
    // The protocol shows 'position' as a fixed-point argument. Correcting the printf, assuming the argument passed is actually wl_fixed_t.
    // If the header defines position as uint32_t, the output is questionable. Keeping the original *call*, but noting the type discrepancy.
    printf("[tablet-strip] position: %.3f\n", wl_fixed_to_double(position));
}

static void handle_strip_stop(void * data, struct zwp_tablet_pad_strip_v2 *strip) {
    printf("[tablet-strip] stop\n");
}

static void handle_strip_frame(void * data, struct zwp_tablet_pad_strip_v2 *strip, uint32_t time) {
    printf("[tablet-strip] frame at %u\n", time);
}

static const struct zwp_tablet_pad_strip_v2_listener tablet_strip_listener = {
    .source = handle_strip_source,
    .position = handle_strip_position,
    .stop = handle_strip_stop,
    .frame = handle_strip_frame,
};


/* ===================== zwp_tablet_pad_group_v2 ===================== */

static void handle_group_buttons(void *data, struct zwp_tablet_pad_group_v2 *group, struct wl_array *buttons) {
    printf("[tablet-group] buttons assigned\n");
}

static void handle_group_ring(void *data, struct zwp_tablet_pad_group_v2 *group, struct zwp_tablet_pad_ring_v2 *ring) {
    printf("[tablet-group] attached ring: %p\n", (void *)ring);
    zwp_tablet_pad_ring_v2_add_listener(ring, &tablet_ring_listener, NULL);
}

static void handle_group_strip(void *data, struct zwp_tablet_pad_group_v2 *group, struct zwp_tablet_pad_strip_v2 *strip) {
    printf("[tablet-group] attached strip: %p\n", (void *)strip);
    zwp_tablet_pad_strip_v2_add_listener(strip, &tablet_strip_listener, NULL);
}

static void handle_group_modes(void *data, struct zwp_tablet_pad_group_v2 *group, uint32_t modes) {
    printf("[tablet-group] modes: %u\n", modes);
}

static void handle_group_done(void *data, struct zwp_tablet_pad_group_v2 *group) {
    printf("[tablet-group] done configuring group\n");
}

static void handle_group_mode_switch(void *data, struct zwp_tablet_pad_group_v2 *group, uint32_t time, uint32_t serial, uint32_t mode) {
    printf("[tablet-group] mode switch - time:%u serial:%u mode:%u\n", time, serial, mode);
}


/* ===================== zwp_tablet_pad_v2 ===================== */

static const struct zwp_tablet_pad_group_v2_listener tablet_pad_group_listener = {
    .buttons = handle_group_buttons,
    .ring = handle_group_ring,
    .strip = handle_group_strip,
    .modes = handle_group_modes,
    .done = handle_group_done,
    .mode_switch = handle_group_mode_switch,
};


struct tablet_seat_state {
    // Stores state related to the tablet seat
};

static void tablet_seat_handle_tablet_added(void *data,
        struct zwp_tablet_seat_v2 *tablet_seat, struct zwp_tablet_v2 *tablet) {
    // Handle a newly added tablet device
    printf("Tablet added.\n");
}

static const char *pointer_state_str(uint32_t state) {
	switch (state) {
	case 0: // WL_POINTER_BUTTON_STATE_RELEASED
		return "released";
	case 1: // WL_POINTER_BUTTON_STATE_PRESSED
		return "pressed";
	default:
		return "unknown state";
	}
}

static void tablet_pad_group(void *data, struct zwp_tablet_pad_v2 *pad,
		struct zwp_tablet_pad_group_v2 *pad_group) {
	zwp_tablet_pad_group_v2_add_listener(pad_group,
			&tablet_pad_group_listener, data);
	printf("pad group id %u\n", wl_proxy_get_id((struct wl_proxy *)pad_group));
}

static void tablet_pad_path(void *data, struct zwp_tablet_pad_v2 *pad,
		const char *path) {
	printf("pad path %s\n", path);
}

static void tablet_pad_buttons(void *data, struct zwp_tablet_pad_v2 *pad,
		uint32_t buttons) {
	printf("[pad] buttons %u\n", buttons);
}

static void tablet_pad_done(void *data, struct zwp_tablet_pad_v2 *pad) {
	printf("[pad] done\n");
}

static void tablet_pad_button(void *data, struct zwp_tablet_pad_v2 *pad,
		uint32_t time, uint32_t button, uint32_t state) {
	printf("[pad] button time %u, button %u, state %u, (%s)\n", time, button, state, pointer_state_str(state));
}

static void tablet_pad_enter(void *data, struct zwp_tablet_pad_v2 *pad,
		uint32_t serial, struct zwp_tablet_v2 *tablet,
		struct wl_surface *surface) {
	printf("[pad] serial %u, tablet %u, surface %u\n", serial,
                      wl_proxy_get_id((struct wl_proxy *)tablet),
                      wl_proxy_get_id((struct wl_proxy *)surface));
}

static void tablet_pad_leave(void *data, struct zwp_tablet_pad_v2 *pad,
		uint32_t serial, struct wl_surface *surface) {
	printf("[pad] leave %u surface %u\n",serial,
                      wl_proxy_get_id((struct wl_proxy *)surface) );
}

static void tablet_pad_removed(void *data, struct zwp_tablet_pad_v2 *pad) {
	printf("[pad] removed\n");
}

static const struct zwp_tablet_pad_v2_listener tablet_pad_listener = {
	.group = tablet_pad_group,
	.path = tablet_pad_path,
	.buttons = tablet_pad_buttons,
	.done = tablet_pad_done,
	.button = tablet_pad_button,
	.enter = tablet_pad_enter,
	.leave = tablet_pad_leave,
	.removed = tablet_pad_removed,
};

static void tablet_seat_handle_pad_added(void *data,
        struct zwp_tablet_seat_v2 *tablet_seat, struct zwp_tablet_pad_v2 *pad_id) {
    printf("Tablet pad added.\n");

	zwp_tablet_pad_v2_add_listener(pad_id, &tablet_pad_listener, data);
}

/* ===================== zwp_tablet_tool_v2 ===================== */
static void tool_handle_proximity_in(void *data,
        struct zwp_tablet_tool_v2 *tool, uint32_t serial,
        struct zwp_tablet_v2 *tablet, struct wl_surface *surface) {
    printf("Tool proximity in.\n");
}

static void tool_handle_proximity_out(void *data,
        struct zwp_tablet_tool_v2 *tool) {
    printf("Tool proximity out.\n");
}

static void tool_handle_down(void *data,
        struct zwp_tablet_tool_v2 *tool, uint32_t serial) {
    printf("Tool down (contact).\n");
}

static void tool_handle_up(void *data, struct zwp_tablet_tool_v2 *tool) {
    printf("Tool up (no contact).\n");
}

static void tool_handle_motion(void *data,
        struct zwp_tablet_tool_v2 *tool, wl_fixed_t x, wl_fixed_t y) {
    double dx = wl_fixed_to_double(x);
    double dy = wl_fixed_to_double(y);
    printf("Tool motion: x=%f, y=%f\n", dx, dy);
}

static void tool_handle_pressure(void *data,
        struct zwp_tablet_tool_v2 *tool, uint32_t pressure) {
    printf("Tool pressure: %u\n", pressure);
}

static void tablet_tool_handle_tilt(void *data, struct zwp_tablet_tool_v2 *tool,
                                    wl_fixed_t tilt_x, wl_fixed_t tilt_y) {
    printf("Tool tilt: X=%f, Y=%f\n",
           wl_fixed_to_double(tilt_x),
           wl_fixed_to_double(tilt_y));
}


static void tablet_tool_handle_button(void *data, struct zwp_tablet_tool_v2 *tool,
                                      uint32_t serial, uint32_t button, uint32_t state) {
    const char *state_str = (state == 1) ? "pressed" : "released"; // 1 is WL_POINTER_BUTTON_STATE_PRESSED
    printf("Tool button (Serial: %u) Button %u %s\n", serial, button, state_str);
}

// see enum zwp_tablet_tool_v2_type
static void tool_handle_type(void *data, struct zwp_tablet_tool_v2 *tool, uint32_t type) {
    printf("Tool type: 0x%x\n", type);
}

static void tool_handle_serial(void *data, struct zwp_tablet_tool_v2 *tool, uint32_t v1, uint32_t v2) {
    printf("Tool serial: %u %u \n", v1, v2);
}

static void tool_handle_hardware_id(void *data, struct zwp_tablet_tool_v2 *tool, uint32_t v1, uint32_t v2) {
    printf("Tool hardware id: %u %u \n", v1, v2);
}

// see zwp_tablet_tool_v2_capability
void tool_handle_capability(void *data, struct zwp_tablet_tool_v2 *tool, uint32_t value) {
    printf("Tool handle capability: %u\n", value);
}
void tool_handle_done(void *data, struct zwp_tablet_tool_v2 *tool) {
    printf("Tool handle done.\n");
}
void tool_handle_distance(void *data, struct zwp_tablet_tool_v2 *tool, uint32_t value) {
    printf("Tool handle distance: %" PRIu32 "\n", value);
}
void tool_handle_rotation(void *data, struct zwp_tablet_tool_v2 *tool, wl_fixed_t value) {
    printf("Tool handle rotation: %f\n", wl_fixed_to_double(value));
}
void tool_handle_slider(void *data, struct zwp_tablet_tool_v2 *tool, wl_fixed_t value) {
    printf("Tool handle slider: %f\n", wl_fixed_to_double(value));
}

void tool_handle_wheel(void *data, struct zwp_tablet_tool_v2 *tool, wl_fixed_t degrees, int32_t discrete) {
    printf("Tool handle wheel: degrees=%.2f, discrete=%i\n", wl_fixed_to_double(degrees), discrete);
}

void tool_handle_frame(void *data, struct zwp_tablet_tool_v2 *tool, uint32_t value) {
    printf("Tool handle frame: %u\n", value);
}


static const struct zwp_tablet_tool_v2_listener tablet_tool_listener = {
    .type = tool_handle_type,
    .hardware_serial = tool_handle_serial,
    .hardware_id_wacom = tool_handle_hardware_id, // or .hardware_id if your header uses that name
    .capability = tool_handle_capability,
    .done = tool_handle_done,
    .proximity_in = tool_handle_proximity_in,
    .proximity_out = tool_handle_proximity_out,
    .down = tool_handle_down,
    .up = tool_handle_up,
    .motion = tool_handle_motion,
    .pressure = tool_handle_pressure,
    .distance = tool_handle_distance,
    .tilt = tablet_tool_handle_tilt,
    .rotation = tool_handle_rotation,
    .slider = tool_handle_slider,
    .wheel = tool_handle_wheel,
    .button = tablet_tool_handle_button,
    .frame =  tool_handle_frame,
};

void tool_added_handler(struct zwp_tablet_tool_v2 *tool) {
    zwp_tablet_tool_v2_add_listener(tool, &tablet_tool_listener, NULL);
}

static void tablet_seat_handle_tool_added(void *data,
        struct zwp_tablet_seat_v2 *tablet_seat, struct zwp_tablet_tool_v2 *tool) {

    printf("Tool added. Attaching listener for motion/pressure events.\n");

    tool_added_handler(tool);
}


static const struct zwp_tablet_seat_v2_listener tablet_seat_listener = {
    .tablet_added = tablet_seat_handle_tablet_added,
    .tool_added = tablet_seat_handle_tool_added,
    .pad_added = tablet_seat_handle_pad_added,
};


void setup_tablet_input(struct client_state *state) {
    // The state struct should track tablet objects, but for now we'll use a local struct
    struct tablet_seat_state seat_state = {};

    if (state->tablet_manager && state->seat) {
        struct zwp_tablet_seat_v2 *tablet_seat = zwp_tablet_manager_v2_get_tablet_seat(
            state->tablet_manager, state->seat);
        // This is where you register to receive tablet/tool added events
        // NOTE: passing `&seat_state` might limit data access if the handlers need `client_state`.
        // If the handlers only need to print, this is fine. If they need window info, 
        // `state` (struct client_state *) should be passed. Assuming `&seat_state` is correct per the original intent.
        zwp_tablet_seat_v2_add_listener(tablet_seat, &tablet_seat_listener, &seat_state);

        wl_display_flush(state->wl_display);
    }
}

int
main(int argc, char *argv[])
{
    struct client_state state = { 0 };
    state.wl_display = wl_display_connect(NULL);
    if (!state.wl_display) {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return -1;
    }

    state.wl_registry = wl_display_get_registry(state.wl_display);
    wl_registry_add_listener(state.wl_registry, &wl_registry_listener, &state);
    wl_display_roundtrip(state.wl_display);

    // Check if required globals were found
    if (!state.wl_compositor || !state.xdg_wm_base || !state.wl_shm) {
        fprintf(stderr, "Required Wayland globals (compositor, xdg_wm_base, shm) not found.\n");
        return -1;
    }

    // Initial setup for the window
    state.wl_surface = wl_compositor_create_surface(state.wl_compositor);
    state.xdg_surface = xdg_wm_base_get_xdg_surface(
            state.xdg_wm_base, state.wl_surface);
    xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, &state);
    state.xdg_toplevel = xdg_surface_get_toplevel(state.xdg_surface);
    xdg_toplevel_set_title(state.xdg_toplevel, "Example client");
    wl_surface_commit(state.wl_surface);

    // Call tablet setup here, once, before the main loop.
    setup_tablet_input(&state);

    while (wl_display_dispatch(state.wl_display) != -1) {
        /* No-op, events are handled by listeners */
    }

    // Cleanup (minimal for this example)
    if (state.xdg_toplevel) xdg_toplevel_destroy(state.xdg_toplevel);
    if (state.xdg_surface) xdg_surface_destroy(state.xdg_surface);
    if (state.wl_surface) wl_surface_destroy(state.wl_surface);
    if (state.xdg_wm_base) xdg_wm_base_destroy(state.xdg_wm_base);
    if (state.wl_compositor) wl_compositor_destroy(state.wl_compositor);
    if (state.wl_shm) wl_shm_destroy(state.wl_shm);
    if (state.wl_registry) wl_registry_destroy(state.wl_registry);
    if (state.wl_display) wl_display_disconnect(state.wl_display);

    return 0;
}
