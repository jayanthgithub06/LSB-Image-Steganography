#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include<string.h>

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    uint img_size;

    DecodeInfo decInfo;

    

    if(argc>=4 || argc>=3)
    {
        int ret=check_operation_type(argv);
        if(ret==e_encode)
        {
            if(read_and_validate_encode_args(argv,&encInfo)==e_success)
            {
                do_encoding(&encInfo);
            }
            else
            {
                printf("read_and_validate_encode_args failed");
                e_failure;
            }
        }
        else if (ret==e_decode)
        {
            if(read_and_validate_decode_args(argv,&decInfo)==e_success)
            {
                do_decoding(&decInfo);

            }
            else
            {
                printf("read_and_validate_decode_args\n");

            }
            
        }
        else
        {
            printf("invalid argument\n");
            return e_unsupported;
            
        }
        
    }
    
}
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
    {

        printf("-e found encoding operation started\n");
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {

        printf("-d found decoding operation started\n");
        return e_decode;
    }
    else
    {
        printf("invalid argument");
        return e_unsupported;
    }

}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    
    if(argv[2]==NULL)
    {
        printf("no argument provided");
        return e_failure;

    }
    char *arg2=strstr(argv[2],".bmp");

    if(argv[2]!=NULL)
    {
        encInfo->src_image_fname = argv[2];
        printf(".bmp file found\n");
        // printf("%s\n",encInfo->src_image_fname);
        
    }
    else
    {
        printf(".bmp file is not found ");
        return e_failure;
    }

    if(argv[3]==NULL)
    {
        printf("no argument provided");
        return e_failure;

    }
    char *arg3=strstr(argv[3],".txt");
    encInfo->extn_size=strlen(arg3);
    strcpy(encInfo->extn_secret_file,arg3);
    printf("encInfo->extn_secret_file=%s\n",encInfo->extn_secret_file);
    

    if(argv[3]!=NULL)
    {
        encInfo->secret_fname = argv[3];
        printf(".txt file found\n");
        // printf("%s",encInfo->secret_fname);
        
    }
    else
    {
        printf(".txt file is not found ");
        return e_failure;
    }

    if(argv[4]==NULL)
    {
        encInfo->stego_image_fname="output.bmp";
        printf("No output file provided. using default: output.bmp\n");
        return e_success;

    }
    char *arg4=strstr(argv[4],".bmp");

    if(argv[4]!=NULL)
    {
        encInfo->stego_image_fname = argv[4];
        printf(".bmp file found\n");
        // printf("%s",encInfo->stego_image_fname);
        return e_success;
    }
    else
    {
        printf(".bmp file is not found ");
        return e_failure;
    }
    

}

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decode)
{
    if(argv[2]==NULL)
    {
        printf("no argument provided");
        return e_failure;

    }

    char *arg2=strstr(argv[2],".bmp");
    if(arg2!=NULL)
    {

        decode->input_fname=argv[2];
        printf(".bmp file found\n");
        printf("file saved as .bmp %s\n",decode->input_fname);
    }
    else
    {
        printf(".bmp file not found\n");
        return e_failure;
    }
    
    if(argv[3]==NULL)
    {
        decode->output_fname="decode.txt";
        printf("file saved as decode.txt %s\n",decode->output_fname);
        return e_success;
    }

    char *arg3=strstr(argv[3],".txt");
    if(arg3!=NULL)
    {

        decode->output_fname=argv[3];
        printf(".txt file found\n");
        printf("file saved as decode.txt %s\n",decode->output_fname);
        return e_success;
    }
    
}


