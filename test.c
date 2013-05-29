#include "pll.h"
#include "md5.h"

int main(int argc, char** argv) {
    unsigned char digest[16];
    int err = pll_get_open_key(digest);
    if (err) {
        perror("generateOpenKey");
        return EXIT_FAILURE;
    }

    printf("Open key = ");
    md5_print_hex(digest);
    printf("\n");

    return EXIT_SUCCESS;
}
