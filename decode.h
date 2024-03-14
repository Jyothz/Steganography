#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define DECODE_MAX_IMAGE_BUF_SIZE 8
#define DECODE_MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /*Decoded image info*/
    char *src_image_fname;
    FILE *fptr_src_image;
    char image_data[DECODE_MAX_IMAGE_BUF_SIZE];



    /*Secret image info*/
    char *secret_file_fname;
    FILE *fptr_secret_file;
    char extn_secret_file[DECODE_MAX_FILE_SUFFIX];
    long size_secret_file;
    long size_secret_file_extn;
}DecodeInfo;

/* Decoding function prototype */

/*Read and validate Decode args from argv*/
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

/*Perform Decoding */
Status do_decoding(DecodeInfo *decInfo);

/*open files for decoding*/
Status open_decode_files(DecodeInfo *decInfo);

/*Decode Magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/*Decode secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/*Decode secret file extension*/
Status decode_secret_file_extn(DecodeInfo *decInfo);

/*Decode secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo);


/*Decode secret file data*/
Status decode_secret_data(DecodeInfo *decInfo);

/*Decode LSB to Byte function*/
Status decode_lsb_to_byte(char *data, char *image_buffer);

/*Decode LSB to int function*/
Status decode_lsb_to_int(long *data, char *image_buffer);


/*Decode LSB to int Function*/

#endif
