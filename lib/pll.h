/*
 * pll.h
 *
 *  Created on: 30.05.2013
 *      Author: Alexander
 */

#ifndef PLL_H_
#define PLL_H_

#include <stdlib.h>

#define PLL_KEY_SIZE 16
#define PLL_LICENSE_LEN 16

/*
 * returns MAC address of given interface
 * input: network interface name (NULL-terminated string)
 * output: MAC address
 * returns: zero on success, non-zero on failure and ERRNO is set appropriately
 */
int pll_get_macaddr(char* mac, const char* iface_name);

/*
 * returns hard drive serial number
 * input: hard drive interface name (NULL-terminated string)
 * output: HD serial
 * returns: zero on success, non-zero on failure and ERRNO is set appropriately
 */
int pll_get_hdserial(char* serial, size_t serial_len, const char* hd_name);

/*
 * returns open key, based on hardware info. Assumes that key length is PLL_KEY_SIZE.
 * input:
 * output: open key
 * returns: zero on success, non-zero on failure and ERRNO is set appropriately
 */
int pll_get_open_key(char* key);

/*
 * returns check key, based on open key and license string.
 * Assumes that key length is PLL_KEY_SIZE and license length is PLL_LICENSE_LEN.
 * input: open key, license key
 * output: check key
 * returns: zero on success, -1 if license format is incorrect
 */
int pll_get_check_key(char* key, const char* open_key, const char* license);

/*
 * checks whether given license key matches the assumed pattern.
 * Right now it's just a dummy function that always returns 0
 */
int pll_parse_license(const char* license);

#endif /* PLL_H_ */
