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

int pll_get_macaddr(char mac[6], const char* iface_name);
int pll_get_hdserial(char* serial, size_t serial_len, const char* hd_name);
int pll_get_open_key(char key[PLL_KEY_SIZE]);
int pll_get_check_key(char key[PLL_KEY_SIZE], const char open_key[PLL_KEY_SIZE], const char license[PLL_LICENSE_LEN]);

#endif /* PLL_H_ */
