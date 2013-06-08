#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pll.h"

int main(int argc, char *argv[]) {
	if (argc != 3) {
	    fprintf(stderr, "Usage: hardwareinfo <flags> <iface name>\n");
	    return EXIT_FAILURE;
	}
    if (!strncmp(argv[1], "-mac", 5)) {
        unsigned char mac[6];
        int err = pll_get_macaddr((char*) mac, argv[2]);
        if (err) {
            if (err == -2) fprintf(stderr, "pll_get_macaddr: interface not found\n");
            else perror("pll_get_macaddr");
            return EXIT_FAILURE;
        }
        printf("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else if (!strncmp(argv[1], "-hd", 4)) {
        char serial[256];
        int err = pll_get_hdserial(serial, 256, argv[2]);
        if (err) {
            perror("pll_get_hdserial");
            return EXIT_FAILURE;
        }
        printf("%s\n", serial);
    } else {
        printf("Error: unknown flag %s\n", argv[1]);
        return EXIT_FAILURE;
    }
	return EXIT_SUCCESS;
}
