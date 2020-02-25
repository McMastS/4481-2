#include "libpnm.h"
#include <stdlib.h>

#ifndef GEN_PIX_FREQ_H
#define GEN_PIX_FREQ_H

long int *generate_pixel_frequency(struct PGM_Image *input_pgm_image, int *number_of_non_zero_values_in_the_frequency_array);

#endif