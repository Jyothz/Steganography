#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)
	return e_encode;
    else if (strcmp(argv[1],"-d") == 0)
	return e_decode;
    else
	return e_unsupported;

}

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //check .bmp file
    if(strcmp(strstr(argv[2],"."),".bmp") != 0)
    {
	printf("error : .bmp file not found\n");
	return e_failure;
    }
    encInfo->src_image_fname= argv[2];


    //check secret .txt file
    if(strcmp(strstr(argv[3],"."),".txt") != 0)
    {
	printf("error : .txt file not found\n");
	return e_failure;
    }
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file,".txt");

    //check optional file if not creat one
    if( argv[4] == NULL )
    {
	encInfo->stego_image_fname= "stego.bmp";
	encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"w");
	if(encInfo->fptr_stego_image == NULL)
	{
	    printf("Error : unable to create file\n");
	    return e_failure;
	}
	printf("Output file 'stego.bmp' created Sucessfully\n");
    }
    else
    {
	if(strcmp(strstr(argv[4],"."),".bmp") != 0) 
	{
	    printf("ERROR : .bmp file required as output file\n");
	    return e_failure;
	}
	encInfo->stego_image_fname = argv[4];
	encInfo->fptr_stego_image =fopen(encInfo->stego_image_fname,"w");

	printf("Output file %s file created sucessfully\n",argv[4]);
    }

    return e_success;
}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo)
{
    /* Get File pointers for i/p and o/p files */
    if(open_files(encInfo) == e_success)
    {
	printf("File Opened Successfully\n");
    }
    else
    {
	printf("ERROR : Unable to open file\n");
	return e_failure;
    }
    if(check_capacity(encInfo) == e_success)
    {

	/* Copy bmp image header */
	if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_failure )
	{
	    printf("Error : in copying BMP Header\n");
	    return e_failure;
	}

	/* Store Magic String */
	if(encode_magic_string(MAGIC_STRING,encInfo) == e_success )
	{
	    printf("MAGIC_STRING encoded successfully \n");
	}
	else
	{
	    printf("ERROR : Unable to encode magic string \n");
	    return e_failure;
	}

	/* Encode secret file size */
	if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
	{
	    printf("Secret file extension size encoded successfully\n");

	}
	else
	{
	    printf("ERROR : encoding secret file extention size\n");
	    return e_failure;
	}

	/* Encode secret file extenstion */
	if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
	{
	    printf("Secret file extention encode success\n");
	}
	else
	{
	    printf("ERROR : encoding secret file extension\n");
	    return e_failure;
	}

	/* Encode secret file size */
	if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
	{
	    printf("Secret file size encode success\n");
	}
	else
	{
	    printf("ERROR : encoding secret file size\n");
	    return e_failure;
	}

	/* Encode secret file data*/
	if(encode_secret_file_data(encInfo) == e_success)
	{
	    printf("Secret file data encoded successfully\n");
	}
	else
	{
	    printf("ERROR : Unable to encode secret file data\n");
	    return e_failure;
	}

	/* Copy remaining image bytes from src to stego image af    ter encoding */
	if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	{
	    printf("Successfully Copyied rest of the data\n");
	}
	else
	{
	    printf("ERROR : Copying remaining data \n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR : The File cannot be encoded\n");
	return e_failure;
    }

    return e_success;
}

/* Copy remaining image bytes from src to stego image af    ter encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,sizeof(ch),1,fptr_src) > 0)
    {
	fwrite(&ch,sizeof(ch),1,fptr_dest);
    }
    return e_success;
}
/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    rewind(encInfo->fptr_secret);
    /* Encode function, which does the real encoding */
    for(int i=0; i<encInfo->size_secret_file; i++)
    {
	fread(&ch,1,1,encInfo->fptr_secret);
	int status = encode_data_to_image(&ch,MAX_SECRET_BUF_SIZE, encInfo->fptr_src_image, encInfo->fptr_stego_image);
	if(status == e_failure)
	{
	    printf("Error Encoding Secret file data\n");
	    return e_failure;
	}
    }

    return e_success;
}


/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_data[32];
    /* Encode function, which does the real encoding */
    fread(image_data,1,32,encInfo->fptr_src_image);
    if(encode_size_to_lsb(file_size,image_data) == e_success)
    {
	fwrite(image_data,1,32,encInfo->fptr_stego_image);
    }
    else
    {
	printf("Error  : Encoding file size\n");
	return e_failure;
    }
    return e_success;


}
/* Encode secret file extesion size */
Status encode_secret_file_extn_size(long file_size, EncodeInfo *encInfo)
{
    char image_data[32];
    /* Encode function, which does the real encoding */
    fread(image_data,1,32,encInfo->fptr_src_image);
    if(encode_size_to_lsb(file_size,image_data) == e_success)
    {
	fwrite(image_data,1,32,encInfo->fptr_stego_image);
    }
    else
    {
	printf("Error  : Encoding file size\n");
	return e_failure;
    }
    return e_success;


}
/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    /* Encode function, which does the real encoding */
    int status = encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if(status == e_failure)
    {
	printf("Error Encoding extension\n");
	return e_failure;
    }
    return e_success;

}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    /* Encode function, which does the real encoding */
    int status = encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if(status == e_failure)
    {
	printf("Error Encoding Magic String\n");
	return e_failure;
    }

    return e_success;
}



/* Encode function, which does the real encoding */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];

    for(int i=0; i<size; i++)
    {
	fread(image_buffer,1,8,fptr_src_image);
	/* Encode a byte into LSB of image data array */
	if(encode_byte_to_lsb(*(data+i),image_buffer) == e_success)
	{
	    fwrite(image_buffer,1,8,fptr_stego_image);
	}
	else
	{
	    printf("Error : data to image Encoding failed\n");
	    return e_failure;
	}

    }
    return e_success;
}

/* Encode a int byte into LSB of image data array */
Status encode_size_to_lsb(int data, char *image_buffer)
{
    int temp;
    for(int i=0; i<32; i++)
    {
	temp=data;
	temp=(temp>>i)&1;
	image_buffer[i]=(image_buffer[i] & 0xFE) | temp;
    }
    return e_success;
}

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    char temp;
    for(int i=0; i<8; i++)
    {
	temp=data;
	temp=(temp>>i)&1;
	image_buffer[i]=(image_buffer[i] & 0xFE) | temp;
    }
    return e_success;
}


/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char head[54];
    rewind(fptr_src_image);
    rewind(fptr_stego_image);
    fread(head,1,54,fptr_src_image);
    fwrite(head,1,54,fptr_stego_image);
    return e_success;
}
/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    /* Get file size */
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);


    if(encInfo->image_capacity >= (strlen(MAGIC_STRING)+32+strlen(encInfo->extn_secret_file)+32+ (encInfo->size_secret_file*8)))
    {
	return e_success;
    }
    else
    {
	printf("Error : The secret file cannot be encoded : greater size\n");
	return e_failure;
    }
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);

}

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

