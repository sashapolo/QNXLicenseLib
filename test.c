#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <devctl.h>
#include <ifaddrs.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/dcmd_cam.h>
#include <net/if_dl.h>

#include "md5.h"

static const int BUF_SIZE = 256;
static const int MAC_SIZE = 6;

int getMacAddr(char* name, char mac[MAC_SIZE]) {
    struct ifaddrs* ifaphead;
    if (getifaddrs(&ifaphead) != 0) {
        return 1;
    }

    struct ifaddrs* ifap;
    for (ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
        if (ifap->ifa_addr->sa_family == AF_LINK && !strcmp(ifap->ifa_name, name)) {
            struct sockaddr_dl* sdl = (struct sockaddr_dl*) ifap->ifa_addr;
            if (sdl) {
                memcpy(mac, LLADDR(sdl), MAC_SIZE);
                freeifaddrs(ifaphead);
                return 0;
            }
        }
    }

    if (ifaphead) {
        freeifaddrs(ifaphead);
    }

    return 2;
}

int getHdSerialNumber(char* name, char* serial, int serialSize) {
    int file = open(name, O_RDONLY);
    if (!file) {
        return 1;
    }

    int err = devctl(file, DCMD_CAM_DEV_SERIAL_NUMBER, serial, serialSize, NULL);
    if (err != EOK) {
        return 2;
    }

    return 0;
}

int main(int argc, char** argv) {
    unsigned char mac[MAC_SIZE];
    int err = getMacAddr("en0", (char*) mac);
    if (err == 2) {
        fprintf(stderr, "getMacAddr: Interface not found\n");
        return EXIT_FAILURE;
    } else if (err) {
        perror("getMacAddr");
        return EXIT_FAILURE;
    }
    printf("%02X%02X%02X%02X%02X%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    char serial[BUF_SIZE];
    err = getHdSerialNumber("/dev/hd0", serial, BUF_SIZE);
    if (err) {
        perror("getHdSerialNumber");
        return EXIT_FAILURE;
    }
    printf("SERIAL_NUMBER is %s\n", serial);

    char str[BUF_SIZE + MAC_SIZE];
    strcpy(str, (char*) mac);
    strcat (str, serial);
    MD5Context context;
    unsigned char digest[16];

    md5_init(&context);
    md5_update(&context, (unsigned char*)str, strlen(str));
    md5_finalize(&context, digest);

    printf("Open key = ");
    md5_print_hex(digest);
    printf("\n");

    return EXIT_SUCCESS;
}
