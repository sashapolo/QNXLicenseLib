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

int getHdSerial(char* name, char* serial, int serialSize) {
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

void md5sum(unsigned char digest[16], const unsigned char* string, size_t length) {
    MD5Context context;
    md5_init(&context);
    md5_update(&context, string, length);
    md5_finalize(&context, digest);
}

int generateOpenKey(unsigned char result[16]) {
    unsigned char mac[MAC_SIZE];
    char* ifaces[] = {"en0"};
    unsigned int i;
    int err = 2;
    for (i = 0; i < sizeof(ifaces); ++i) {
        err = getMacAddr(ifaces[i], (char*) mac);
        if (err == 0) break;
    }
    if (err) {
        return err;
    }

    //debug
    printf("%02X%02X%02X%02X%02X%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    char serial[BUF_SIZE];
    err = getHdSerial("/dev/hd0", serial, BUF_SIZE);
    if (err) {
        return err;
    }

    //debug
    printf("SERIAL_NUMBER is %s\n", serial);

    char str[BUF_SIZE + MAC_SIZE];
    strcpy(str, (char*) mac);
    strcat (str, serial);

    md5sum(result, (unsigned char*) str, sizeof(str));
    return 0;
}

int main(int argc, char** argv) {
    unsigned char digest[16];
    int err = generateOpenKey(digest);
    if (err) {
        perror("generateOpenKey");
        return EXIT_FAILURE;
    }

    printf("Open key = ");
    md5_print_hex(digest);
    printf("\n");

    return EXIT_SUCCESS;
}
