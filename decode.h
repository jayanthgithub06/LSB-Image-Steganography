#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAG_SIZE 100
#define EXTEN_LEN 4
#define DATA_LEN 500

typedef struct DecodeInfo
{
    char *input_fname;
    FILE *fp_input;

    //decoding image

    int magic_string_len;
    char magic_string[MAG_SIZE];

    //secret decoding
    int extn_len;
    char extn[EXTEN_LEN];
    int data_len;
    char data[DATA_LEN];
    char *output_fname;
    FILE *fp_output;
} DecodeInfo;


Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

Status open_files_for_decoding(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status skip_header(FILE *fp_input);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_extension_size(DecodeInfo *decInfo);

Status decode_extension(DecodeInfo *decInfo);

Status decode_data(DecodeInfo *decInfo);

Status decode_data_size(DecodeInfo *decInfo);

int decode_size_from_lsb(FILE *fp);

Status decode_data_from_image(int len,char *data,FILE *fp_input,FILE *fp_output);

char decode_byte_from_lsb(char *data,int i);



#endif