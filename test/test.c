#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#include <stdlib.h>

void printMAC(unsigned char* mac) {
	printf("%02X%02X%02X%02X%02X%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

int main(int argc, char** argv) {
	struct ifaddrs* ifaphead;
	if (getifaddrs(&ifaphead) != 0) {
		perror("get_if_name: getifaddrs() failed");
		exit(1);
	}

	struct ifaddrs* ifap;
	for (ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
		if (ifap->ifa_addr->sa_family == AF_LINK) {
			struct sockaddr_dl* sdl = (struct sockaddr_dl*) ifap->ifa_addr;
			if (sdl) {
				char* if_mac = LLADDR(sdl);
				printMAC((unsigned char*) if_mac);
			}
		}
	}

	if (ifaphead) freeifaddrs(ifaphead);
	exit(0);
}
