#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char **argv)
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    int op_type = check_operation_type(argv);
    if(op_type == e_encode)
    {
	printf("Encoding Started\n");
	if(read_and_validate_encode_args(argv,&encInfo) == e_success)
	{
	    printf("Validation Success\n");
	    if(do_encoding(&encInfo) == e_success)
	    {
		printf("Encoding Complete\n");
	    }
	    else
	    {
		printf("ERROR : Encoding Unsuccessful\n");
		return 1;
	    }
	}
	else 
	{
	    printf("Error : File name not provided\n");
	printf("encode execute -> ./a.out -e <.bmp> <.txt> [optional]stego.bmp\n");
	    return 1;
	}
    }
    else if(op_type == e_decode)
    {
	printf("Decoding started\n");
	if(read_and_validate_decode_args(argv,&decInfo) == e_success)
	{
	    printf("Validation Success\n");
	    if(do_decoding(&decInfo) == e_success)
	    {
		printf("Decoding Complete\n");
	    }
	    else
	    {
		printf("ERROR : Decoding Unsuccessful\n");
		return 1;
	    }
	}
	else 
	{
	    printf("ERROR : File name not provided\n");
	printf("decode execute -> ./a.out -d <.bmp> [optional]decode.txt\n");
	    return 1;
	}

    }
    else
    {
	printf("\nif encode execute -> ./a.out -e <.bmp> <.txt> [optional]stego.bmp\n");
	printf("if decode execute -> ./a.out -d <.bmp> [optional]decode.txt\n");
    }

    return 0;
}

