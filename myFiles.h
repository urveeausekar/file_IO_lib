#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>

#define MAXOPEN 32
#define BUFSIZE 1024


#define ENDF -1

enum mode {EnOF = -1, R = 1, W = 2, A = 3, RP = 4, WP = 5, AP = 6};

typedef struct myFile{
	int fd;         /*File descriptor*/
	char *buf;      /*Base address of buffer*/
	enum mode flag; /*mode of file access*/
	char *next;     /*address of next char in buffer*/
	int left;       /*charachters left to be read or written in buffer*/
	char last;      /*this was last operation. To be used in all plus modes*/
	char bufornot;  /*to buffer or not to buffer*/
}File;

extern File ptrarr[MAXOPEN];

#define Stdin (&ptrarr[0])
#define Stdout (&ptrarr[1])
#define Stderr (&ptrarr[2])

typedef struct Fpos_t{
	long p;
}Fpos_t;

File *Fopen(const char *path, const char *mode);
unsigned long Fread(void *ptr, unsigned long size, unsigned long nmem, File *fp);
unsigned long Fwrite(void *ptr, unsigned long size, unsigned long nmem, File *fp);
int bufflush(File *fp);
int Fclose(File *fp);
long Ftell(File *fp);
int Fseek(File *fp, long pos, int whence);
int Feof(File *fp);
int Fsetpos(File *fp, Fpos_t *position);
int Fgetpos(File *fp, Fpos_t *position);
