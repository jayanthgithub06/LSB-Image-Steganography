#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>
int temp;
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

    
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("opened all the files successfully!\n");
        if(check_capacity(encInfo)==e_success)
        {
            printf("passed the check capacity\n");
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
            {
                printf("copied header file to bmp\n");
                if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
                {
                    printf("encode magic string successful\n");
                    if (encode_secret_file_extn_size(encInfo->extn_size, encInfo) == e_success)
                    {
                        printf("encoded secret file extn size successfully\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)== e_success)
                        {
                            printf("encode_secret_file_extn successfully\n");
                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
                            {
                                printf("encode_secret_file_size successfully\n");
                                if(encode_secret_file_data(encInfo)==e_success)
                                {
                                    printf("encode_secret_file_data successfully\n");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image, encInfo)==e_success)
                                    {
                                        printf("copy_remaining_img_data passed\n");
                                        printf("encoding success!");
                                    }
                                    else
                                    {
                                        printf("copy_remaining_img_data failed");
                                        return e_failure;

                                    }
                                }
                                else
                                {
                                    printf("encode_secret_file_data failed");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("encode_secret_file_size failed\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("encode_secret_file_extn failed");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("encoded secret file extn failed");
                        return e_failure;
                    }
                }
                else
                {
                    printf("encode magic string failed");
                    return e_failure;
                }

            }
            else
            {
                printf("copy failed to bmp header");
                return e_failure;
            }
        }
        else
        {
            printf("failed all check capacity");
            return e_failure;
        }

    }
    else
    {
        printf("can't open files ");
        return e_failure;

    }

}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0L,SEEK_END);
    printf("size of secret file=%ld\n",ftell(fptr));
    int size=ftell(fptr);
    return size;
}
Status check_capacity(EncodeInfo *encInfo)
{
  
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("image capacity = %d\n",encInfo->image_capacity);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

    int size_of_magic_string=strlen(MAGIC_STRING);
    temp = (54+(size_of_magic_string + sizeof(int) + sizeof(int) + sizeof(int) + encInfo->size_secret_file )*8);

    if(encInfo->image_capacity > temp)
    {
        printf("size of temp=%d\n",temp);
        return e_success;

    }
    else
    {
        printf("temp is greater");
        return e_failure;
    }

}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char head[54];
    fseek(fptr_src_image,0L,SEEK_SET);
    fread(head,54,1,fptr_src_image);
    fwrite(head,54,1,fptr_dest_image);
    return e_success;
}
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    
    printf("Encode magic string started!\n");
    int len = strlen(magic_string);
    
    OperationType ret = encode_size_to_lsb(len,encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    printf("after encode_size_to_lsb=%ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    

    encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("src=%ld\n",ftell(encInfo->fptr_src_image));
        printf("dest=%ld\n",ftell(encInfo->fptr_stego_image));
        return e_success;
    }
    else
        return e_failure;
    
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(buffer,8,1,fptr_src_image);
        encode_byte_to_lsb(data[i],buffer);
        fwrite(buffer,8,1,fptr_stego_image);
    }
    return e_success;

}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        if(data & (1<<(7-i)))
        {
            image_buffer[i]=image_buffer[i] | 0x01;
        }
        else
        {
            image_buffer[i]=image_buffer[i] & 0xFE;
        }
    }
    return e_success;

}
Status encode_size_to_lsb(int data,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    printf("Encoding size to lsb started!\n");
    char image_buffer[32];
    fread(image_buffer,32,1,fptr_src_image);
    for(int i = 31; i >= 0; i--)
    {
        if(data & (1<<i))
            image_buffer[31 - i] |= 0x01;
        else
            image_buffer[31 - i] &= 0xFE;
    }
    fwrite(image_buffer,32,1,fptr_stego_image);
    printf("Encoding size to lsb completed!\n");
    return e_success;
}
Status encode_secret_file_extn_size(int file_extn_size,EncodeInfo *encInfo)
{
     encode_size_to_lsb(file_extn_size, encInfo->fptr_src_image, encInfo->fptr_stego_image);
     printf("after encode_size_to_lsb = %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
     return e_success;
}
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{

    encode_data_to_image(file_extn,4,encInfo->fptr_src_image,encInfo->fptr_stego_image);

    printf("\nencode_data_to_image = %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    return e_success;


}
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    int ret = encode_size_to_lsb(file_size,encInfo->fptr_src_image,encInfo->fptr_stego_image);
    // printf("function=%d",encInfo->size_secret_file);

    if(ret==e_success){
        printf("after encode_secret_file_size %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
        return e_success;
    }
    else
    {
        printf("failure encode_secret_file_size");
        return e_failure;

    }

}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    
    fseek(encInfo->fptr_secret,0L,SEEK_SET);
    fread(encInfo -> secret_data,encInfo -> size_secret_file,1,encInfo -> fptr_secret);
    printf("data=%s",encInfo -> secret_data);
    encode_data_to_image(encInfo -> secret_data,encInfo -> size_secret_file,encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    printf("\nafter encode_data_to_image %ld %ld\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
    return e_success;

}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest, EncodeInfo *encInfo)
{

    char ch;
    while (fread(&ch,1,1,fptr_src))
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}

