#ifndef DE_CODE_H
#define DE_CODE_H

#include "types.h"

#define MAX_BUFFER 100

typedef struct De_code{
	///encoded file info
	FILE *fptr_enco;
	char  *fname_enco;

	//magic string info
	int d_ms_len;
	char d_ms_string[MAX_BUFFER];
	
	//decoded file info
	char *de_fname;
	FILE *fptr_deco;
	//file externtion info
	int  d_sec_extn_len;
	char sec_extn[MAX_BUFFER];

	//secret msg info
	int sec_msg_len;

} DecodeInfo;

//for read and store the things for decode 
Status read_and_validate_decode_args(char **,DecodeInfo *);
// decoding processs done by below function
Status Do_decode(DecodeInfo *);

Status DE_openfiles(FILE **,char *);

Status Decode_len(int*,DecodeInfo *);

Status Decode_str(int *,char*,DecodeInfo*);

Status Decode_Ushhhh_msg(DecodeInfo *);

#endif 









