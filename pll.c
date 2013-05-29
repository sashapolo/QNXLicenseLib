/*
 * pll.c
 *
 *  Created on: 30.05.2013
 *      Author: Alexander
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <devctl.h>
#include <ifaddrs.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/dcmd_cam.h>
#include <net/if_dl.h>

#include "pll.h"
#include "md5.h"

const int BUF_SIZE = 255;

void md5sum(char digest[16], const char* string, size_t len);

int pll_get_macaddr(char mac[6], const char* iface_name) {
    struct ifaddrs* ifaphead;
    if (getifaddrs(&ifaphead) != 0) return 1;

    struct ifaddrs* ifap;
    for (ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
        if (ifap->ifa_addr->sa_family == AF_LINK && !strcmp(ifap->ifa_name, iface_name)) {
            struct sockaddr_dl* sdl = (struct sockaddr_dl*) ifap->ifa_addr;
            if (sdl) {
                memcpy(mac, LLADDR(sdl), 6);
                freeifaddrs(ifaphead);
                return 0;
            }
        }
    }

    if (ifaphead) freeifaddrs(ifaphead);
    return 2;
}

int pll_get_hdserial(char* serial, size_t serial_len, const char* hd_name) {
    int file = open(hd_name, O_RDONLY);
    if (!file) return 1;

    int err = devctl(file, DCMD_CAM_DEV_SERIAL_NUMBER, serial, serial_len, NULL);
    if (err != EOK) return 2;

    return 0;
}

int pll_get_open_key(char key[PLL_KEY_SIZE]) {
    unsigned char mac[6];
    const char* ifaces[] = {"en0"};
    unsigned int i;
    int err = 2;
    for (i = 0; i < sizeof(ifaces); ++i) {
        err = pll_get_macaddr((char*) mac, ifaces[i]);
        if (err == 0) break;
    }
    if (err) return err;

    char serial[BUF_SIZE];
    err = pll_get_hdserial(serial, BUF_SIZE, "/dev/hd0");
    if (err) return err;

    char str[BUF_SIZE + 6];
    strcpy(str, (char*) mac);
    strcat(str, serial);

    md5sum(key, str, sizeof(str));
    return 0;
}

int pll_get_check_key(char key[PLL_KEY_SIZE],
                      const char open_key[PLL_KEY_SIZE],
                      const char license[PLL_LICENSE_LEN]) {
    const size_t buf_size = PLL_KEY_SIZE + PLL_LICENSE_LEN;
    char buf[buf_size];
    strcpy(buf, open_key);
    strcat(buf, license);
    md5sum(key, buf, buf_size);
    free(buf);
    return 0;
}

void md5sum(char digest[16], const char* string, size_t len) {
    md5_context_t context;
    md5_init(&context);
    md5_update(&context, (const unsigned char*) string, len);
    md5_finalize(&context, (unsigned char*) digest);
}
