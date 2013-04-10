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

#define BUF_SIZE 256
#define MAC_SIZE 6

int getMacAddr(char* name, char* mac) {
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

    return EXIT_SUCCESS;
}
