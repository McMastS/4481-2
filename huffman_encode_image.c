#include "huffman_encode_image.h"
#include "generate_pixel_frequency.h"

#define MAX_CODE_LENGTH 16

void print_binary(int number)
{
    if (number == 0) printf("0");
    if (number) {
        print_binary(number >> 1);
        putc((number & 1) ? '1' : '0', stdout);
    }
}

void free_huffman_codes(char **codes, int length)
{
    for(int i = 0; i < length; i++) 
        free(codes[i]);
    free(codes);
}

// https://stackoverflow.com/questions/14176123/correct-usage-of-strtol
bool parse_long(const char *str, long *val)
{
    char *temp;
    bool rc = true;
    *val = strtol(str, &temp, 2); // radix must be 2, as string is in binary representation

    if (temp == str || *temp != '\0') rc = false;

    return rc;
}

unsigned char *huffman_encode_image(struct PGM_Image *input_pgm_image, struct node *huffman_node, int number_of_nodes, long int *length_of_encoded_image_array)
{
    char **huffman_codes = malloc((input_pgm_image->maxGrayValue + 1) * sizeof(char *));
    if (huffman_codes == NULL)
        {
            printf("Allocating huffman codes went wrong.\n");
            exit(0);
        }
    for (int i = 0; i < input_pgm_image->maxGrayValue + 1; i++)
    {
        huffman_codes[i] = (char *)malloc(MAX_CODE_LENGTH + 1);
        if (huffman_codes[i] == NULL)
        {
            printf("Allocating strings in huffman codes went wrong.\n");
            exit(0);
        }
        strcpy(huffman_codes[i], "");
    }

    for (int i = number_of_nodes - 1; i >= 0; i--)
    {
        struct node curr_node = huffman_node[i];
        if (i == number_of_nodes - 1)
        {
            strcpy(huffman_codes[curr_node.first_value], "0");
            strcpy(huffman_codes[curr_node.second_value], "1");
        }
        else 
        {
            const char *first_val = huffman_codes[curr_node.first_value];
            const char *second_val = huffman_codes[curr_node.second_value];
            if (first_val[0] == '\0')
            {
                strcpy(huffman_codes[curr_node.first_value], second_val);
                strcat(huffman_codes[curr_node.first_value], "0");
                strcat(huffman_codes[curr_node.second_value], "1");
            }
            else if (second_val[0] == '\0')
            {
                strcpy(huffman_codes[curr_node.second_value], first_val);
                strcat(huffman_codes[curr_node.first_value], "0");
                strcat(huffman_codes[curr_node.second_value], "1");
            }
        }
    }

    for (int i = 0; i < 255+1; i++)
    {
        if (*huffman_codes[i] != '\0') printf("Code for %d = %s \n", i, huffman_codes[i]);
    }
    
    // for the size, you have the frequency array and the bit size for each, so you can find the byte length that way

    // Start with a size of 100
    long int allocated_len = 100;
    unsigned char *encoded_image = malloc(allocated_len * sizeof(unsigned char));
    if (encoded_image == NULL) 
    {
        printf("Memory for encoded image was not allocated.\n");
        exit(0);
    }

    int curr_bits_allocated = 0;
    char *curr_byte = malloc(9); // 8 bits + \0 char
    for (int row = 0; row < input_pgm_image->height; row++)
    {
        for (int col = 0; col < input_pgm_image->width; col++)
        {
            int symbol = input_pgm_image->image[row][col];
            const char *code = huffman_codes[symbol];
            int code_length = strlen(code);

            if (curr_bits_allocated + code_length > 8) // byte would overflow
            {
                // printf("overflow\n");
                int code_remainder = (curr_bits_allocated + code_length) - 8; // bits left in code
                int byte_remainder = code_length - code_remainder; // bits left in byte
               
                // figure out logic
                for (int i = 8 - byte_remainder, j = 0; i < 8; i++, j++) // probably wrong
                {
                    curr_byte[i] = code[j];    
                // strcat(curr_byte, code[j]); // might have to do strcpy?
                }

                // throw in function if time allows
                unsigned char temp_char;
                char *temp;
                long val = strtol(curr_byte, &temp, 2);
                temp_char = (unsigned char) val;
                
                if (*length_of_encoded_image_array + 10 > allocated_len)
                {
                    allocated_len *= 2;
                    unsigned char *realloacted = realloc(encoded_image, allocated_len);
                    if (realloacted == NULL)
                    {
                        printf("Couldn't reallocate encoded image array.");
                        exit(0);
                    }
                    encoded_image = realloacted;
                }
                encoded_image[*length_of_encoded_image_array] = temp_char;
                (*length_of_encoded_image_array)++;
                curr_bits_allocated = 0;
                strcpy(curr_byte, "");
                for (int i = 0, j =  code_length - code_remainder; j < code_remainder; i++, j++)
                {
                    curr_byte[i] = code[j];
                    // strcat(curr_byte, to_add);
                }
            }
            else if (curr_bits_allocated + code_length == 8) // byte is full, but not overflowing
            {
                strcat(curr_byte, code);
                
                unsigned char temp_char;
                char *temp;
                long val = strtol(curr_byte, &temp, 2);
                temp_char = (unsigned char) val;
                
                if (*length_of_encoded_image_array + 10 > allocated_len)
                {
                    allocated_len *= 2;
                    unsigned char *realloacted = realloc(encoded_image, allocated_len);
                    if (realloacted == NULL)
                    {
                        printf("Couldn't reallocate encoded image array.");
                        exit(0);
                    }
                    encoded_image = realloacted;
                }
                encoded_image[*length_of_encoded_image_array] = temp_char;
                (*length_of_encoded_image_array)++;
                curr_bits_allocated = 0;
                strcpy(curr_byte, "");
            }
            else // regular case
            {
                strcat(curr_byte, code);
                curr_bits_allocated += code_length;
            }
        }
    }

    free_huffman_codes(huffman_codes, input_pgm_image->maxGrayValue+1);
    return encoded_image;
}

int main(int argc, const char* argv[])
{
    struct PGM_Image pic;
    if (load_PGM_Image(&pic, "test_square.raw.pgm") == -1) 
    {
        printf("Something went wrong loading the image.\n");
        return -1;
    }

    int non_zero_freq = 0;
    long int *pixel_freq = generate_pixel_frequency(&pic, &non_zero_freq);
    struct node *huff_nodes = generate_huffman_nodes(pixel_freq, pic.maxGrayValue, non_zero_freq);
    long int image_size = 0;
    unsigned char *encoded_image = huffman_encode_image(&pic, huff_nodes, non_zero_freq - 1, &image_size);

    printf("size: %ld\n", image_size);
    printf("Last 10 for encoded: \n");
    for (int i = image_size - 1; i > image_size - 10; i--){
        printf("%u", encoded_image[i]);
        printf(" \n");
    }
    
    free(huff_nodes);
    free(pixel_freq);
    free(encoded_image);
    free_PGM_Image(&pic);
}