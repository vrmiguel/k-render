#ifndef __KRENDER_IO_H
#define __KRENDER_IO_H

#include <iostream>
#include "includes/ktypes.h"

bool     save_result(TGAImage img, const char *filename, bool rle=true);
config_t parse_cli_input(int argc, char ** argv);

#endif // __KRENDER_IO_H
