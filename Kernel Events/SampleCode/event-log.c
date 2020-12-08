/* Print raw pen or expresskey kernel events
 *
 * to compile:
 *  gcc -o event-log event-log.c  
 *
 * to run:
 *  find your device in /dev/input/...
 *  sudo ./event-log /dev/input/eventX
 *
 * hint: compile and run devices.c first to find the /dev/input/eventX
 * that your pen and/or expresskey is associated with
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

                        printf("%ld.%06ld type %d code %d value %d\n",
				events[i].time.tv_sec,
				events[i].time.tv_usec,
				events[i].type,
				events[i].code,
				events[i].value);
		}
	}

	return 0;
}
