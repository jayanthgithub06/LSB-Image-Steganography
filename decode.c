#include <stdio.h>
#include "decode.h"
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>


Status open_files_for_decoding(DecodeInfo *decInfo)
{
    decInfo->fp_input=fopen(decInfo->input_fname,"r");

    if(decInfo->fp_input == NULL)
    {
        perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->input_fname);

    	return e_failure;
    }

    decInfo->fp_output=fopen(decInfo->output_fname,"w");

    if(decInfo->fp_output == NULL)
    {
        perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);

    	return e_failure;   
    }
    return e_success;


}


Status do_decoding(DecodeInfo *decInfo)
{
    
    if(open_files_for_decoding(decInfo)==e_success)
    {
        printf("opened file success\n");
        if(skip_header(decInfo->fp_input)==e_success)
        {
            printf("\nheader files skipped successfully\n");
            if(decode_magic_string(decInfo)==e_success)
            {
                printf("magic string decoded completed\n");
                if(decode_extension_size(decInfo)==e_success)
                {
                    printf("decode extension size completed\n");
                    if(decode_extension(decInfo)==e_success)
                    {
                        printf("decode extension successfully\n");
                        if(decode_data_size(decInfo)==e_success)
                        {
                            printf("decoded data size successfully\n");
                            if(decode_data(decInfo)==e_success)
                            {
                                printf("decoding successfully done\n");

                            }
                            else
                            {
                                printf("decoding failed\n");
                                return e_failure;
                            }

                        }
                        else
                        {
                            printf("decode data to size failed\n");
                        }
                    }
                    else
                    {
                        printf("decode extension failed\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("decode extension size failed\n");
                    return e_failure;

                }

            }
            else
            {
                printf("magic string decoded failed");
                return e_failure;
            }
            

        }
        else
        {
            printf("header files skipped failed\n");
            return e_failure;
        }

    }
    else
    {
        printf("open header fails\n");
        return e_failure;

    }

}

Status skip_header(FILE *fp_input)
{
    fseek(fp_input,54,SEEK_SET);

    printf("after skipping the header %ld",ftell(fp_input));

    if(ftell(fp_input)==54)
        return e_success;

    else
        return e_failure;

}
Status decode_magic_string(DecodeInfo *decode)
{
    decode->magic_string_len = decode_size_from_lsb(decode->fp_input);
    printf("magic string = %d\n",decode->magic_string_len);
    printf("after decoding magic string length input=%ld\n",ftell(decode->fp_input));

    
    
    OperationType ret=decode_data_from_image(decode->magic_string_len,decode->magic_string,decode->fp_input,decode->fp_output);

    printf("decode_data_from_image %ld\n",ftell(decode->fp_input));

    printf("decode->magic_string=%s\n",decode->magic_string);
    return e_success;
}

int decode_size_from_lsb(FILE *fp_input)
{

    char buffer[32];
    int magic_string_len=0;
    fread(buffer,32,1,fp_input);

    for(int i = 31; i >= 0; i--)
    {
        if(buffer[31-i] & 0x01)
            magic_string_len|=(1<<i);
        else
           magic_string_len&=(~(1<<i));
        
    }
    
    return magic_string_len;
}
Status decode_data_from_image(int len,char *data,FILE *fp_input,FILE *fp_output)
{
    char buffer[8];
    int i;

    for(i=0; i<len; i++)
    {
        fread(buffer,8,1,fp_input);
        data[i]=decode_byte_from_lsb(buffer,i);

    }
    data[i]='\0';
   
    return e_success;

}
char decode_byte_from_lsb(char *data,int i)
{
    char ch;
    for(int j=0;j<8;j++)
    {
        if(data[j] & 0x01)
            ch|=(1<<(7-j));
        else
            ch&=~(1<<(7-j));
    }
    return ch;

}

Status decode_extension_size(DecodeInfo *decInfo)
{
    decInfo->extn_len=decode_size_from_lsb(decInfo->fp_input);
    printf("decInfo->extn_len=%d\n",decInfo->extn_len);
    printf("after decode_extension_size %ld\n",ftell(decInfo->fp_input));
    return e_success;
}
Status decode_extension(DecodeInfo *decInfo)
{
    OperationType res=decode_data_from_image(decInfo->extn_len,decInfo->extn,decInfo->fp_input,decInfo->fp_output);
    printf("extension decoding %s\n",decInfo->extn);
    return e_success;
}
Status decode_data_size(DecodeInfo *decInfo)
{
    decInfo->data_len=decode_size_from_lsb(decInfo->fp_input);
    printf("decoded data length %d\n",decInfo->data_len);
    return e_success;
}
Status decode_data(DecodeInfo *decInfo)
{
    decode_data_from_image(decInfo->data_len,decInfo->data,decInfo->fp_input,decInfo->fp_output);
    printf("extension decoding = %s\n",decInfo->data);

    fwrite(decInfo->data,decInfo->data_len,1,decInfo->fp_output);
    return e_success;

}