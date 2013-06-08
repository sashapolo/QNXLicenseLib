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

static const int BUF_SIZE = 255;

static void md5sum(char* digest, const char* string, size_t len);

int pll_get_macaddr(char* mac, const char* iface_name) {
    struct ifaddrs* ifaphead;
    if (getifaddrs(&ifaphead) != 0) return -1;

    struct ifaddrs* ifap;
    for (ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
        if (ifap->ifa_addr->sa_family == AF_LINK) {
            if (iface_name == NULL || !strcmp(ifap->ifa_name, iface_name)) {
                struct sockaddr_dl* sdl = (struct sockaddr_dl*) ifap->ifa_addr;
                if (sdl) {
                    memcpy(mac, LLADDR(sdl), PLL_MAC_SIZE);
                    freeifaddrs(ifaphead);
                    return 0;
                }
                break;
            }
        }
    }

    if (ifaphead) freeifaddrs(ifaphead);
    return -2;
}

int pll_get_hdserial(char* serial, size_t serial_len, const char* hd_name) {
    int file = open(hd_name, O_RDONLY);
    if (!file) return -1;

    int err = devctl(file, DCMD_CAM_DEV_SERIAL_NUMBER, serial, serial_len, NULL);
    if (err != EOK) return -1;

    return 0;
}

int pll_get_open_key(char* key) {
    unsigned char mac[PLL_MAC_SIZE];
    int err = pll_get_macaddr((char*) mac, NULL);
    if (err) return -1;

    char serial[BUF_SIZE];
    err = pll_get_hdserial(serial, BUF_SIZE, "/dev/hd0");
    if (err) return -1;

    char str[BUF_SIZE + PLL_MAC_SIZE];
    strcpy(str, (char*) mac);
    strcat(str, serial);

    md5sum(key, str, sizeof(str));
    return 0;
}

int pll_get_check_key(char* key, const char* open_key, const char* license, size_t len) {
    char parsed_license[len];
    strncpy(parsed_license, license, len);
    if (pll_parse_license(parsed_license, len)) return -1;
    const size_t buf_size = PLL_KEY_SIZE + PLL_LICENSE_LEN;
    char buf[buf_size];
    strcpy(buf, open_key);
    strcat(buf, parsed_license);
    md5sum(key, buf, buf_size);
    return 0;
}

int pll_parse_license(const char* license, size_t len) {
    return (len != PLL_LICENSE_LEN) ? -1 : 0;
}

void md5sum(char* digest, const char* string, size_t len) {
    md5_context_t context;
    md5_init(&context);
    md5_update(&context, (const unsigned char*) string, len);
    md5_finalize(&context, (unsigned char*) digest);
}
