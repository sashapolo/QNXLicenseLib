#include "pll.h"
#include "md5.h"
#include <stdio.h>

int main(int argc, char** argv) {
    unsigned char digest[16];
    int err = pll_get_open_key(digest);
    if (err) {
        perror("pll_get_open_key");
        return EXIT_FAILURE;
    }

    printf("Open key = ");
    md5_print_hex(digest);
    printf("\n");

    unsigned char digest2[16];
    err = pll_get_check_key(digest2, digest, "AAAAAAAAAAAAAAAA");
    if (err) {
        perror("pll_get_check_key");
        return EXIT_FAILURE;
    }

    printf("Check key = ");
    md5_print_hex(digest2);
    printf("\n");
    return EXIT_SUCCESS;
}
