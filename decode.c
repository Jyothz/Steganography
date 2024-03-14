#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/*Read and validate Decode args from argv*/
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    //check for .bmp file
    if(strcmp(strstr(argv[2],"."),".bmp") != 0)
    {
	printf("ERROR : .bmp file not found\n");
	return e_failure;
    }
    decInfo->src_image_fname = argv[2];

    /*check for iutput file if not present creat default*/
    if(argv[3] == NULL)
    {
	decInfo->secret_file_fname = "secret_out.txt";
    }
    else
    {
	if(strcmp(strstr(argv[3],"."),".txt") != 0 )
	{
	    printf("ERROR : .txt file required as output file\n");
	    return e_failure;
	}
	decInfo->secret_file_fname = argv[3];
    }
    return e_success;

}

/*Decoding function*/
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == e_success)
    {
	printf("File opened successfully\n");
    }
    else
    {
	printf("Error : File opening failed\n");
	return e_failure;
    }
     if(decode_magic_string(decInfo) == e_success)
    {
	printf("Magic string decoding successful\n");
    }
    else
    {
	printf("NO MAGIC STRING FILE NOT ENCODED\n");
	return e_failure;
    }
    
     if(decode_secret_file_extn_size(decInfo) == e_success)
    {
	printf("Secret file extension size decoding successful\n");
    }
    else
    {
	printf("ERROR : decoding secret file extension size\n");
	return e_failure;
    }

     if(decode_secret_file_extn(decInfo) == e_success)
    {
	printf("Secret file extension decoding successful\n");
    }
    else
    {
	printf("ERROR : decoding secret file extension\n");
	return e_failure;
    }

    if(decode_secret_file_size(decInfo) == e_success)
    {
	printf("Secret file size decoded successfully\n");
    }
    else
    {
	printf("ERROR : decodeing secret file size\n");
	return e_failure;
    }

    if(decode_secret_data(decInfo) == e_success)
    {
	printf("Secret file data decoded successfully\n");
    }
    else
    {
	printf("ERROR : decoding secret file size\n");
	return e_failure;
    }


    return e_success;
}

/*decoding secret file data*/
Status decode_secret_data(DecodeInfo *decInfo)
{
    char ch;
    rewind(decInfo->fptr_secret_file);
    for(int i=0; i<decInfo->size_secret_file; i++)
    {
	fread(decInfo->image_data,1,8,decInfo->fptr_src_image);
	decode_lsb_to_byte(&ch,decInfo->image_data);
	fwrite(&ch,1,1,decInfo->fptr_secret_file);
    }
    return e_success;
}

/*decoding secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_data[32];
    fread(image_data,1,32,decInfo->fptr_src_image);
    decode_lsb_to_int(&decInfo->size_secret_file,image_data);
    return e_success;
}

/*decoding secret file extension*/
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    
    for (int i=0; i<decInfo->size_secret_file_extn; i++)
    {
	fread(decInfo->image_data,1,8,decInfo->fptr_src_image);
	decode_lsb_to_byte(decInfo->extn_secret_file+i,decInfo->image_data);
    }

    if(strcmp(strstr(decInfo->secret_file_fname,"."),decInfo->extn_secret_file) != 0)
    {
	printf("Cannot decode to %s , need %s type output file\n",decInfo->secret_file_fname,decInfo->extn_secret_file);
	return e_failure;
    }
    return e_success;
}

/*decoding secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char image_data[32];

	fread(image_data,1,32,decInfo->fptr_src_image);
	decode_lsb_to_int(&decInfo->size_secret_file_extn, image_data);
	return e_success;

}

/*opening files for operation*/
Status open_decode_files(DecodeInfo *decInfo)
{
    //open encoded file with r mode
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname,"r");
    if(decInfo->fptr_src_image == NULL)
    {
	perror("fopen");
	fprintf(stderr,"ERROR : Unable to open file %s \n",decInfo->src_image_fname);
	return e_failure;
    }

    /*open output file in write mode*/
    decInfo->fptr_secret_file = fopen(decInfo->secret_file_fname,"w");
    if(decInfo->fptr_secret_file == NULL)
    {
	perror("fopen");
	fprintf(stderr,"ERROR : Unable to open file %s \n",decInfo->secret_file_fname);
	return e_failure;
    }
 
    return e_success;
}

/*decoding magic string*/
Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[2];

    fseek(decInfo->fptr_src_image,54,SEEK_SET);
    for(int i=0; i<2; i++)
    {
	fread(decInfo->image_data,1,8,decInfo->fptr_src_image);
    if(decode_lsb_to_byte(magic_string+i,decInfo->image_data) == e_failure)
    {
	printf("Error : File opening failed\n");
	return e_failure;
    }
    }
    if(strcmp(magic_string,"#*") == 0)
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

/*decoding integer*/
Status decode_lsb_to_int(long *data,char *image_buffer)
{
    int temp;
    *data=*data&0;
    for(int i=0; i<32; i++)
    {
	temp=image_buffer[i]&1;
	*data=*data | (temp<<i);
    }
    return e_success;

}

/*decoding char bytes*/
Status decode_lsb_to_byte(char *data,char *image_buffer)
{
    char temp;
    *data=*data&0;
    for(int i=0; i<8; i++)
    {
	temp=image_buffer[i]&1;
	*data=*data | (temp<<i);
    }
    return e_success;

}


