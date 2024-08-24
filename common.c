#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "De_code.h"
#include "types.h"


int main(int argc, char *argv[]){
    ///Checkinng operation type - encoding or decoding
    int perform = check_operation_type(argv);
    switch (perform){
	    case e_decode :
		    DecodeInfo decInfo;
		    if(read_and_validate_decode_args(argv,&decInfo) == e_success){
		            printf("## Decoding procedure started ##\n");
			    if(Do_decode(&decInfo) == e_success){
				    printf("## Decoding Successfully completed check %s file for secret message ##",decInfo.de_fname);
				    return 0;
			    }
			    else{
				    return -1;
			    }
		    }
		    else{
			    return -1;
		    }
    
		    // for  decode things
		    break;
	    case e_encode :
                   EncodeInfo encInfo;
		   if(read_and_validate_encode_args(argv,&encInfo) == e_success ){
				///validation things are done we procede with the encode
			   if (do_encoding(&encInfo) == e_success){
				   printf("INFO : ## Encoding Done Successfully ##\n");
				   free(encInfo.secret_data);
			   }
			   else{
				   return -1;
			   } 
		   }
		   else{
			printf("Error : There is no encode File in command line argument\n");
		        return -1;
		   }
		    break;
	    default :
		    printf("Error : Invalid operation try again\n");
		    return -1;
    }
    return 0;
}

/* Check operation type */

OperationType check_operation_type(char *argv[]){
	if(strcmp(argv[1],"-e") == 0){
		return e_encode;
	}
	else if(strcmp(argv[1],"-d") == 0){
		return e_decode;
	}
	else{
		return e_unsupported;
	}
}
