/*
 *  Copyright (c) 1999-2000 Vojtech Pavlik
 *  Copyright (c) 2009-2011 Red Hat, Inc
 */

/**
 * List Wacom devices loaded by kernel driver.
 *
 * Modified from evtest.c by Aaron Armstrong Skomra
 * 
 * Compile:
 *	gcc -o devices devices.c 
 * Run:
 *	sudo ./devices
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@ucw.cz>, or by paper mail:
 * Vojtech Pavlik, Simunkova 1594, Prague 8, 182 00 Czech Republic
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <string.h>
#include <dirent.h>
#include <linux/input.h>

static int is_event_device(const struct dirent *dir) {
        return strncmp("event", dir->d_name, 5) == 0;
}

int main()
{
        struct dirent **namelist;
        int i, ndev, devnum;
        char *filename;

        ndev = scandir("/dev/input", &namelist, is_event_device, 0);
        if (ndev <= 0)
                return -1;

        for (i = 0; i < ndev; i++)
        {
                char fname[300];
                int fd = -1;
                char name[256] = "???";

                snprintf(fname, sizeof(fname),
                         "%s/%s", "/dev/input", namelist[i]->d_name);
                fd = open(fname, O_RDONLY);
                if (fd < 0)
                        continue;
                ioctl(fd, EVIOCGNAME(sizeof(name)), name);

		if (strncmp("Wacom", name, 5) == 0)
                	fprintf(stderr, "%s:    %s\n", fname, name);
                close(fd);
                free(namelist[i]);
        }

        return 0;
}

