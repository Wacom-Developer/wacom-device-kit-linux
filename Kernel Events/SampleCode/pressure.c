/* print stylus raw coordinates and pressure from kernel events for both tip and eraser
 *
 * to compile:
 *  gcc -o pressure pressure.c  
 *
 * to run:
 *  find your device in /dev/input/...
 *  sudo ./pressure /dev/input/eventX
 *
 * hint: compile and run devices.c first to find the
 * /dev/input/eventX that your pen is associated with
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

int main (int argc, const char * argv[]) {

	int fd = -1;
	size_t sz;
	struct input_event events[128];
	int i;

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		perror("fd open error!");
		exit(1);
	}

	while (1) {
		sz = read(fd, events, sizeof(struct input_event) * 128);

		if (sz < (int) sizeof(struct input_event)) {
			perror("size error!");
			exit (1);
		}

		for (i = 0; i < (int) (sz / sizeof(struct input_event)); i++) {

			if (events[i].type != 3)
				continue;

			if (events[i].code == ABS_X)
				printf("x value %d\n", events[i].value);
			if (events[i].code == ABS_Y)
				printf("\t\t\ty value %d\n", events[i].value);
			if (events[i].code == ABS_PRESSURE)
				printf("\t\t\t\t\t\tpressure value %d\n", events[i].value);

		}
	}

	return 0;
}
