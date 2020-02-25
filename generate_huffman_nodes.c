#include "generate_huffman_nodes.h"
#include <limits.h> // for INT_MAX

int find_min_pos(long int *freq_array, int size)
{
    int min = INT_MAX;
    int location;
 
    for (int i = 0; i < size ; i++ ) 
    {
        if (freq_array[i] != 0)
        {
            if ( freq_array[i] < min ) 
            {
                min = freq_array[i];
                location = i;
            }
        }    
    }

    return location;
}

struct node *generate_huffman_nodes(long int *pixel_frequency, int max_gray_value, int number_of_non_zero_values_in_frequency_array)
{
    struct node *huffman_nodes;
    huffman_nodes = (struct node*) malloc((number_of_non_zero_values_in_frequency_array - 1) * sizeof(struct node));

    if (huffman_nodes == NULL) 
    {
        printf("Memory for Huffman node array was not allocated.\n");
        exit(0);
    }

    for (int i = 0; i < number_of_non_zero_values_in_frequency_array - 1; i++)
    {
        // Array size will be max_gray_value + 1, as the array has indices for each possible pixel value
        int first_min_pos = find_min_pos(pixel_frequency, max_gray_value + 1);
        int first_val = pixel_frequency[first_min_pos];

        // Remove the first minimum from consideration while we find the next min
        pixel_frequency[first_min_pos] = INT_MAX;
        
        int second_min_pos = find_min_pos(pixel_frequency, max_gray_value + 1);
        int second_val = pixel_frequency[second_min_pos];

        // Store the min values in order determined by position in array
        // First min to appear will be first_value, followed by the second min to appear
        struct node node;
        node.first_value = first_min_pos < second_min_pos ? first_min_pos : second_min_pos;
        node.second_value = first_min_pos < second_min_pos ? second_min_pos : first_min_pos;
        
        pixel_frequency[node.first_value] = first_val + second_val;
        pixel_frequency[node.second_value] = 0;

        huffman_nodes[i] = node;

        // printf("[");
        // for (int j = 0; j < max_gray_value + 1; j++) {
        //     if (pixel_frequency[j] != 0) printf("%ld ", pixel_frequency[j]);
        // }
        // printf("] \n");
        // printf("-------------\n");
    }

    return huffman_nodes;
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
//     printf("[");
//     for (int j = 0; j < pic.maxGrayValue + 1; j++) {
//         if (pixel_freq[j] != 0) printf("%ld ", pixel_freq[j]);
//     }
//     printf("] \n");
//     struct node *huff_nodes = generate_huffman_nodes(pixel_freq, pic.maxGrayValue, non_zero_freq);

//     for (int i = 0; i <5; i++) {
//         printf("%d first: %d second: %d \n", i, huff_nodes[i].first_value, huff_nodes[i].second_value);
//     }

//     free(huff_nodes);
//     free(pixel_freq);
//     free_PGM_Image(&pic);
// }

// 0: 8192 - 4, 255: 8192 - 6, 95: 24576 - 2, 191: 24576 - 3, 63: 32768 - 5, 127: 163840 - 1