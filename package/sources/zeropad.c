#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//This way we will make our uImage block size (of the nand flash) multiple
//In addition we will be able to extend the file so we can calculate the md5 check sum

int main(int argc, char *argv[]) {
    FILE *fin, *fout;
    char *buf;
    int  n;
    int  blocksize;
   
    if (argc != 4) {
	printf("usage: %s InputFile PaddedFile BlockSize\n", argv[0]);
	exit(1);
    }
	
    fin = fopen(argv[1],"rb");
    if (fin == NULL) {
	printf("Error opening %s\n", argv[1]);
	exit(1);
    }
    
    fout = fopen(argv[2],"wb");
    if (fout == NULL) {
	printf("Error opening %s\n", argv[2]);
	exit(1);
    }

    blocksize = strtol(argv[3], NULL, 0);
    if (blocksize < 0) {
	printf("Error in blocksize\n");
	exit(1);
    }
    printf("blocksize = %d (0x%0x)\n", blocksize, blocksize);

    buf = (char *)malloc(blocksize);
    if (buf == NULL) {
	printf("Error in blocksize\n");
	exit(1);
    }
  


    //Copy the full blocks
    while((n = fread(buf, sizeof(char), blocksize, fin))==blocksize) {
	fwrite(buf, sizeof(char), blocksize, fout);
    }
	
    //Copy the zero padded last block  	
    memset(buf+n, 0xff, blocksize-n); //nand flash is 0xff if cleared
    fwrite(buf, sizeof(char), blocksize, fout);	 	
    
    free(buf);
    fclose(fin);
    fclose(fout);
    
    return 0;
}
