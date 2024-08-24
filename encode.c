#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include "encode.h"
#include "types.h"

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
	printf("INFO : Opening required filess\n");
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    else{
	    printf("INFO : Opened %s \n",encInfo->src_image_fname);
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
    else{
	    printf("INFO : Opened %s\n",encInfo->secret_fname);
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
    else{
	    printf("INFO : Opened %s\n",encInfo->stego_image_fname);
    }
    // No failure return e_success
    return e_success;
}


Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo){

      	// step 1 ---- Input File validation
	if (strstr(argv[2],".bmp") == NULL){
		printf(".bmp nt detected \n");
		return e_failure;
	}
	else{
		encInfo->src_image_fname=argv[2];
	}

	// step 2 ---- Secret file validation
	char *check=strchr(argv[3],'.');
	if( check == NULL){
		printf("Error : there is no .extention file");
		return e_failure;
	}
	else{
		encInfo->secret_fname=argv[3];
		int i=0;
		strcpy(encInfo->extn_secret_file,check);

		encInfo->extern_size=strlen( encInfo->extn_secret_file);
	}
	//step 3 ------ output file is present or not
	if(argv[4] == NULL){
		static char default_name[]="ushhhhh_image.bmp";
		encInfo->stego_image_fname=default_name;
		printf("INFO : Output file  mentioned . Creating %s as a  Default\n",encInfo->stego_image_fname);
	}
	else{
		if(strstr(argv[4],".bmp") != NULL){
			encInfo->stego_image_fname=argv[4];
		}
		else{
			printf("Error : there is no correct output file detected give .bmp format\n");

			return e_failure;
		}
	}
	return e_success;
}
Status do_encoding(EncodeInfo *encInfo){
		 if(open_files(encInfo) == e_success){
			 printf("INFO : Done\n");
			 printf("ALERT : Enter the magic string it should be less than 20 characters : ");
			 static char magic_string[20];
			 scanf("%[^\n]",magic_string);
			 getchar();
			 encInfo->magic_str=magic_string;
			 encInfo->magic_len=strlen(magic_string);
			 if(check_capacity(encInfo) == e_success){//Checking capacity for encode things 
				 printf("INFO : Done. Found ok\n");
                                 if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)== e_success){//copying RGB headder
					 //starting the the ecncode process
			 		printf("INFO : ## Validations are Done Encode Procedure Started ##\n");
					printf("INFO : Started to Encode Magic string Length \n");
					if(Encode_generic_integer((int)encInfo->magic_len,encInfo) == e_success){//encode magic_string interger
						printf("INFO : Process  done\n");
						printf("INFO : Started to Encode Magic string\n");
						if(Encode_generic_string((int)encInfo->magic_len,encInfo->magic_str,encInfo) == e_success){// for encode magic string 
							printf("INFO : Process done\n");
							
							if(Encode_generic_integer((int)encInfo->extern_size,encInfo) == e_success){//for encode extern integer
								printf("INFO : Started to Encode secret file Extention\n"); 
								if(Encode_generic_string((int)encInfo->extern_size,encInfo->extn_secret_file,encInfo) == e_success){//for encode extern string
									printf("INFO : Process done\n");
									if(Encode_generic_integer((int)encInfo->size_secret_file,encInfo) == e_success){// for store extesecret file integer
										printf("INFO : Started to Encode secret data \n");

										if(Encode_generic_string((int)encInfo->size_secret_file,encInfo->secret_data,encInfo) == e_success){
											printf("INFO : Process done\n");
											printf("INFO : Required Things Encoded\n");
											if(copy_remaining_img_data(encInfo) == e_success){
												printf("INFO : Process done\n");
												
												return e_success;
											}
											else{
												return e_failure;
											}
										}
										else{
											return e_failure;
										}
									}
									else{
										return e_failure;
									}
								}
								else{
									return e_failure;
								}
							}
							else{
								return e_failure;
							}
						}
						else{
							return  e_failure;
						}
					}
					else{
						return e_failure;
					}
				}
				 else{
					 return e_failure;
			 	}
			 }
			 else{
				 return e_failure;
			 }
		 }
		 else{
		 	return e_failure;
		}

}

Status Encode_generic_string(int size ,char  *data,EncodeInfo* encInfo){  // to encode String types
	int fetch_mem= size * 8;
	unsigned char temp_buffer[fetch_mem];
	if(fread(temp_buffer , sizeof(char) , fetch_mem , encInfo->fptr_src_image) != fetch_mem){
		printf("ERROR : while fetching RGB data from %s file\n",encInfo->src_image_fname);
		return e_failure;
	}
	int byte_itr=0;
	for(int i=0; i< size ;i++){
		unsigned char temp = *(data+i);
		for(int j=0;j<8;j++){
			unsigned char mask = temp & (unsigned char) (1 << j);
			mask = mask >> j;
			temp_buffer[byte_itr] = (temp_buffer[byte_itr] & (~1)) | mask;
			byte_itr ++ ;
		}
	}
	if ( fwrite(temp_buffer,sizeof(char),fetch_mem,encInfo->fptr_stego_image) != fetch_mem){
		printf("ERROR : Error occured while writing string into %s file\n",encInfo->stego_image_fname);
		return e_failure;
	}

        return e_success;
}


Status Encode_generic_integer(int size,EncodeInfo* encInfo){ // to encode integer type of datas
	int fetch_mem = sizeof(int)*8;
	unsigned char temp_buffer[fetch_mem];
	if(fread(temp_buffer,sizeof(char),fetch_mem,encInfo->fptr_src_image) != fetch_mem){
		printf("ERROR : While Fetching RGB Datas from  %s to encode String length \n",encInfo->src_image_fname);
		return e_failure;
	}
	for(int i=0;i<fetch_mem;i++){
		int mask = size & (1<<i);
		mask = mask >> i; 
		temp_buffer[i] = (temp_buffer[i] & (~1)) | mask; 
	}
	if(fwrite(temp_buffer,sizeof(char),fetch_mem,encInfo->fptr_stego_image) != fetch_mem){
		printf("ERROR : While writing the size to %s file \n",encInfo->stego_image_fname);
		return e_failure;
	}
    return e_success;
}
					


Status check_capacity(EncodeInfo *encInfo){
	printf("INFO : Checking for %s size\n",encInfo->secret_fname);
	if(read_and_Validate_secret_file(encInfo) == e_success){
		printf("INFO : Done. Not empty\n");
		printf("INFO : Checking for %s capacity to handle %s \n",encInfo->src_image_fname,encInfo->secret_fname);
		encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
		uint Encode_things_size=(uint)54+(uint)(sizeof(encInfo->magic_len) + strlen(encInfo->magic_str) + sizeof(encInfo->extern_size) + strlen(encInfo->extn_secret_file) + sizeof(encInfo->size_secret_file) + encInfo->size_secret_file) * (uint)8;
		if(encInfo->image_capacity>Encode_things_size){
			return e_success;
		}
		else{

			printf("ERROR : Your input %s size is smaller than all the Encoding informations \n",encInfo->src_image_fname);
			printf("ALLERT : image capacity is %d encode capacity is %d secret file capacity %ld still %d require  \n",(int)encInfo->image_capacity,(int)Encode_things_size,encInfo->size_secret_file,(int)Encode_things_size-encInfo->image_capacity);
			return e_failure;
		}
	}
}

Status read_and_Validate_secret_file(EncodeInfo *encInfo){// store secret datas for encode 
	
	fseek(encInfo->fptr_secret,0,SEEK_END);
	encInfo->size_secret_file=(int)ftell(encInfo->fptr_secret);
	encInfo->secret_data=calloc(encInfo->size_secret_file*sizeof(char),1);// Dynamic Memorry allocation for secret Buffer to handle large size data
	rewind(encInfo->fptr_secret);
	if(fgets(encInfo->secret_data,MAX_SECRET_BUF_SIZE,encInfo->fptr_secret) == NULL ){
		return e_failure;
	}
	return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_src_image){// get Img size for BMP file
	fseek(fptr_src_image,0,SEEK_END);
	return (uint)ftell(fptr_src_image);
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
	rewind( fptr_src_image );
	int end_head=54;
	unsigned char head_byte[end_head];
	int suc_read = (int) fread(head_byte,sizeof(char),end_head,fptr_src_image) ;
	if(suc_read == 54){
		printf("INFO : copying image Header\n");
	}
	else{
		printf("ERROR : Error occured while copying source Image Header \n");
		return e_failure;
	}
	int suc_write = (int) fwrite(head_byte,sizeof(char),end_head,fptr_dest_image);
	if(suc_write == 54){
		printf("INFO : Done \n");
	}
	else{
		printf("ERROR : Error occured while writing Image Header to stego file \n");
		return e_failure;
	}
	return e_success;
}

Status copy_remaining_img_data(EncodeInfo *encInfo){
	long int fetch_mem=encInfo->image_capacity - (long int )ftell(encInfo->fptr_src_image);
	printf("INFO : Copying Left over Data\n");
	char buffer[fetch_mem];
	if (fread(buffer,sizeof(char),fetch_mem,encInfo->fptr_src_image) != fetch_mem){
		printf("ERROR : while reading remaining data into %s \n",encInfo->src_image_fname);
		return e_failure;
	}
	if(fwrite(buffer,sizeof(char),fetch_mem,encInfo->fptr_stego_image) != fetch_mem){
		printf("ERROR : While writing remaining data into %s \n",encInfo->stego_image_fname);
		return e_failure;
	}
	
	
return e_success;
}

