#include "generate_pixel_frequency.h"


long int *generate_pixel_frequency(struct PGM_Image *input_pgm_image, int *number_of_non_zero_values_in_the_frequency_array)
{
    // Allocate memory for return array, size is max_gray_value + 1, as the array must include 0 and the max_gray_value
    long int *pixel_frequency;
    // Uses calloc to initialize the array as all 0s
    pixel_frequency = (long int*) calloc(input_pgm_image->maxGrayValue+1, sizeof(long int));

    if (pixel_frequency == NULL) 
    {
        printf("Pixel frequency memory not allocated.\n");
        exit(0);
    }

    
    for (int row = 0; row < input_pgm_image->height; row++)
    {
        for (int col = 0; col < input_pgm_image->width; col++)
        {
            int gray_val = input_pgm_image->image[row][col];
            pixel_frequency[gray_val]++;
            
            // If pixel_frequency[gray_val] is 1, then this is our first instance of the pixel value
            // so we increment the value of the non zero counter
            if (pixel_frequency[gray_val] == 1) {
                (*number_of_non_zero_values_in_the_frequency_array)++;
            }
        }
    }
    
    return pixel_frequency;
}

// int main(int argc, const char* argv[]) 
// {
//     struct PGM_Image pic;
//     if (load_PGM_Image(&pic, "test_square.raw.pgm") == -1) {
//         printf("Something went wrong loading the image.\n");
//         return -1;
//     }

//     int non_zero_freq = 0;

//     long int *pixel_freq = generate_pixel_frequency(&pic, &non_zero_freq);
//     printf("Non zero freq: %d \n", non_zero_freq);

//     for (int i = 0; i < pic.maxGrayValue + 1; i++) 
//     {
//         if (pixel_freq[i] != 0) printf("Pixel val: %d  Freq: %ld \n", i, pixel_freq[i]);
//     }

//     free_PGM_Image(&pic);
// }
