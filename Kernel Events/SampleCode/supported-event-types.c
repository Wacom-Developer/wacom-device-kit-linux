/**
 *  Copyright (c) 1999-2000 Vojtech Pavlik
 *  Copyright (c) 2009-2011 Red Hat, Inc
 */

/**
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/**
 * List supported kernel event types for a device.
 *
 * to compile:
 *  gcc -o supported-event-types supported-event-types.c
 *
 * to run:
 *  sudo ./supported-event-types /dev/input/eventX
 *
 * Excerpted and modified from evtest.c by Aaron Armstrong Skomra @Wacom
 *
 * To get a graphic view of the multi-touch kernel events, please refer to:
 * https://github.com/whot/mtview
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <string.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

static const char * const absval[6] = { "Value", "Min  ", "Max  ", "Fuzz ", "Flat ", "Resolution "};

static void print_absdata(int fd, int axis)
{
	int abs[6] = {0};
	int k;

	ioctl(fd, EVIOCGABS(axis), abs);
	for (k = 0; k < 6; k++)
		if ((k < 3) || abs[k])
			printf("			%s %6d\n", absval[k], abs[k]);
}

int main (int argc, const char * argv[]) {

	int fd = -1;
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	unsigned int type, code;
	char evtype_b[512]  = "";

	memset(bit, 0, sizeof(bit));

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		perror("evdev open");
		exit(1);
	}

	memset(evtype_b, 0, sizeof(evtype_b));
	if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evtype_b) < 0) {
		perror("ioctl error");
	}

	ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);

	for (type = 0; type < EV_MAX; type++) {
		if (test_bit(type, bit[0]) && type != EV_REP) {

			printf("  Events of type %d\n", type);
			if (type == EV_SYN)
				continue;
			ioctl(fd, EVIOCGBIT(type, KEY_MAX), bit[type]);
			for (code = 0; code < KEY_MAX; code++)
				if (test_bit(code, bit[type])) {
					printf("		Event code %d\n", code);
					if (type == EV_ABS)
						print_absdata(fd, code);
				}
		}
	}

	close(fd);
	return 0;
}
