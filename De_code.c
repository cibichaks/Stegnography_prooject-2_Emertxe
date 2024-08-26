#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "De_code.h"


Status read_and_validate_decode_args(char *argv[],DecodeInfo  *decInfo){
	int count=0;
	while (argv[count] != NULL){
		count++;
	}
	
	if (strstr(argv[2],".bmp") != NULL && (count >= 2) ){
		decInfo->fname_enco=argv[2];
	}
	else{
		printf("ERROR : .bmp files only can be Decoded go somewhere to decode %s file\n",argv[2]);
		return e_failure;
	}
	if( argv[3] != NULL ){
		decInfo->de_fname=argv[3];
	}else{
		static char created_fname[]="TopSecret";
		decInfo->de_fname=created_fname;
		printf("ALERT : user didn't provide output file name so defaultly output file name is TopSecret.secret_file_type \n");
	}
	return e_success;
}

Status Do_decode(DecodeInfo *decInfo){
	printf("INFO : Opening Required Files\n");
	if(DE_openfiles(&decInfo->fptr_enco,decInfo->fname_enco)  == e_success){
			if(fseek(decInfo->fptr_enco,54,SEEK_SET) == -1){
				perror("fopen");
			}
			if(Decode_len(&decInfo->d_ms_len,decInfo) == e_success){
				if(decInfo->d_ms_len > 20){
				printf("ERROR : While encoding Got length as %d its not possible \n",decInfo->d_ms_len);
				printf("ERROR : Provide the Propper encoded File there is no Encode Data \n");
				return e_failure;
			}

			if(Decode_str(&decInfo->d_ms_len,decInfo->d_ms_string,decInfo) == e_success){
				printf("ATTENTION : Enter the secret magic string :");
				char magic_string[20];
				scanf("%[^\n]",decInfo->d_ms_string);
				getchar();
				if((strcmp(decInfo->d_ms_string,magic_string)) == 0 ){
					printf("ALERT : Magic strings are not matching \n");
					return e_failure;
					}
				else{
					printf("INFO : ## Magic strings are matching validation complete decoding secret file ##\n");
					}
				}
					if(Decode_len(&decInfo->d_sec_extn_len,decInfo)  == e_success){
						if(decInfo->d_sec_extn_len > 5){
							printf("ERROR : While encoding Got length as %d its not possible \n",decInfo->d_sec_extn_len);
							printf("ERROR : Provide the Propper encoded File there is no Encode Data \n");
							return e_failure;
							}
						if(Decode_str(&decInfo->d_sec_extn_len,decInfo->sec_extn,decInfo)== e_success){
							strcat(decInfo->de_fname,decInfo->sec_extn);
							printf("ATTENTION : After decode your encoded data present file %s that located in current directry\n",decInfo->de_fname);
							if((decInfo->fptr_deco = fopen(decInfo->de_fname,"w")) != NULL){
								printf("INFO : Done Opened all required files\n");
								printf("INFO : Decoding secret message size\n");
								if(Decode_len(&decInfo->sec_msg_len,decInfo) == e_success){
									printf("INFO : Done\n");
									printf("INFO : Decoding Secret message \n");
									if(Decode_Ushhhh_msg( decInfo) == e_success){
									
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
								perror("fopen");
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
Status DE_openfiles (FILE **fptr,char *fname){
	if((*fptr=fopen(fname,"r")) == NULL){

		printf("ERROR : while opening %s \n",fname);
		return e_failure;
	}
	else{
		
		printf("INFO : Opened %s for decoing\n",fname);
		return e_success;
	}
}

Status Decode_len(int *store,DecodeInfo *decInfo){
	int fetch_mem=sizeof(int)*8;
	unsigned char RGB_buffer[fetch_mem];
	//printf(" file ptr %d\n",(int)ftell(decInfo->fptr_enco));
	if(fread(RGB_buffer,sizeof(char),fetch_mem,decInfo->fptr_enco) != fetch_mem){
		perror("fread");
		return e_failure;
	}

	*store=0;
	for(int i=0;i<32;i++){
		char mask = RGB_buffer[i] & 1;
		*store= *store | (mask<<i);
	}
	

	//printf("decoded file length %d\n",*store);
	return e_success;
}


Status Decode_str(int *len,char *store,DecodeInfo* decInfo){
	int fetch_mem=*len * 8;
	unsigned char RGB_buffer[fetch_mem];
	if(fread(RGB_buffer,sizeof(char),fetch_mem,decInfo->fptr_enco) != fetch_mem){
		printf("ERROR : while reading Rgb Data from src image");
		return e_failure;
	}
	char temp_ms_buffer[200]={0};
	int RGB_count=0;
	for(int i=0;i<*len;i++){
		for(int j=0;j<8;j++){
			char mask = RGB_buffer[RGB_count] & 1;
			*(temp_ms_buffer+i) = *(temp_ms_buffer+i) & (~(1<<j)) | (mask<<j);
			RGB_count++;
		}
	}
	strcpy(store , temp_ms_buffer);
	//printf("Magic string is : %s",store);
	return e_success;
}

Status Decode_Ushhhh_msg(DecodeInfo * decInfo){
	int fetch_mem=decInfo->sec_msg_len * 8;
	char *RGB_buffer = calloc(fetch_mem * sizeof(char),sizeof(char));// dynamic memory to fetch a byte for secret message * 8  times
	if(fread(RGB_buffer,sizeof(char),fetch_mem,decInfo->fptr_enco) != fetch_mem){
		printf("ERROR : while reading secret RGB byte from encoded file\n");
		return e_failure;
	}
	char *temp_secret_string=calloc((decInfo->sec_msg_len+1) * sizeof(char),sizeof(char)); //To hanlle large number of secret message 
	int RGB_count=0;
	for(int i=0;i<decInfo->sec_msg_len;i++){
		for(int j=0;j<8;j++){
			char mask = RGB_buffer[RGB_count] & 1;
			*(temp_secret_string+i) = *(temp_secret_string+i) | (mask<<j);
			RGB_count++;
		}
	}
	
	temp_secret_string[decInfo->sec_msg_len] = '\0';
	if(fwrite(temp_secret_string,sizeof(char),fetch_mem,decInfo->fptr_deco) != fetch_mem){
		printf("Error : While writing data to secret txt file happened\n");
	}
	free(temp_secret_string);
	free(RGB_buffer);
	return e_success;
}




