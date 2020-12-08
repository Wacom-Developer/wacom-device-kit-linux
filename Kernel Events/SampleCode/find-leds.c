/* find the mode on a device that has an led
 *
 * To compile:
 *  gcc -o find-leds find-leds.c
 *
 * To run:
 *  ./find-leds
 */

#include <dirent.h> 
#include <stdio.h> 
#include <string.h>

/* Concatenate on the end of a path */
int add_to_path(char * buf, char *root, char* adding){
	strcpy(buf, root);
	strcat(buf, "/");
	strcat(buf, adding);
}

/* find the mode on a device that has an led */
int find_mode(char *path){
	FILE *f;
	struct dirent *dir;
        char str[1024];
	char c;

	/* Note: a few devices also conain a second led switch 
	 * at "status_led1_select" that we don't look for
	 * here for the sake of simplicity.
	 */
	add_to_path(str, path, "status_led0_select");

	f = fopen(str,"r");
	if (f) {
		fscanf(f,"%c", &c);
		printf("%s - value of mode switch=%c\n", str, c);

	} else {
		printf("no led file found - %s\n", str);
	}
}

/* See if we have a device with leds */
void find_led_dev(char *path) {

	DIR *d;
	struct dirent *dir;
	char str[1024];

	d = opendir(path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if(strcmp(dir->d_name, "wacom_led") == 0) {
				add_to_path(str, path, "wacom_led");
				find_mode(str);
				return;
			}
		}
		printf("%s - no devices with leds\n", path);

		closedir(d);
  	}
}

/* Call finddev if the wacom sysfs diretory contains a device */
int main(void) {

	DIR *d;
	struct dirent *dir;
	char str[1024];
	char *start = "/sys/bus/hid/drivers/wacom";
	d = opendir(start);

	if (d) {
		while ((dir = readdir(d)) != NULL) {
		/* if the wacom driver directory contains entries that
		 * starts with "0" we know that there is a device to
		 * examine further
		 */
			if (dir->d_name[0] == '0') {
				add_to_path(str, start, dir->d_name);
				find_led_dev(str);
			}
		}

		closedir(d);
	}

	return(0);
}
