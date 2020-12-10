/*
 * Read device state with ioctls
 *
 * Compile:
 *	gcc -o ioctl ioctl.c
 * Run (example):
 *	sudo ./ioctl /dev/input/event5
 *
 * Based on http://www.linuxjournal.com/files/linuxjournal.com/linuxjournal/articles/064/6429/6429l17.html
 */

#include <stdio.h>
#include <linux/input.h>
#include <stdint.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <stdlib.h>

int main (int argc, const char * argv[]) {

	uint8_t abs_b[ABS_MAX/8 + 1];
	struct input_absinfo abs_feat;
	int fd, i;

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
    		perror("open error");
    		exit(1);
	}

	ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abs_b)), abs_b);

	printf("Current axes values:\n");

	for (i = 0; i < ABS_MAX; i++) {
		switch (i) {
			case ABS_X :
				printf("X Axis - ");
				break;
			case ABS_Y :
				printf("Y Axis - ");
				break;
			case ABS_PRESSURE :
				printf("Pressure - ");
				break;
			/* You can check for other ABS_* values like tilt here */

			default:
				printf("Axis 0x%02x - skipped\n", i);
				continue;
		}
		if(ioctl(fd, EVIOCGABS(i), &abs_feat))
			perror("EVIOCGABS error");

		printf("%d (min:%d max:%d flat:%d fuzz:%d)\n",
			abs_feat.value,
			abs_feat.minimum,
			abs_feat.maximum,
			abs_feat.flat,
			abs_feat.fuzz);
	}
}
