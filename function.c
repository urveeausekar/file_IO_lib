#include"myFiles.h"
#include<stdio.h>
#include<limits.h>
#include<string.h>

File ptrarr[MAXOPEN] = {{0, NULL, R, NULL, 0, '\0', 'b'}, {1, NULL, W, NULL, 0, '\0', 'b'}, {2, NULL, A, NULL, 0, '\0', 'u'}};

File *Fopen(const char *path, const char *mode){
	File *fp /*= malloc(sizeof(File))*/;
	int fd;
	for(fp = ptrarr; fp < ptrarr + MAXOPEN; fp++){
		if(fp -> flag == EOF || (fp -> buf == NULL && fp -> bufornot != 'u'))
			break;
	}
	if(fp >= ptrarr + MAXOPEN) 
		return NULL;
	switch(*mode){
		case 'r':
			if(*(mode + 1) == '\0'){
				fd = open(path, O_RDONLY);
				fp -> flag = R;
			}
			else if(*(mode + 1) == '+'){
				fd = open(path, O_RDWR);
				fp -> flag = RP;
			}
			else
				fp -> flag = EOF;
			break;
		case 'w':
			if(*(mode + 1) == '\0'){
				fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
				fp -> flag = W;
			}
			else if(*(mode + 1) == '+'){
				fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
				fp -> flag = WP;
			}
			else
				fp -> flag = EOF;
			break;
		case 'a':
			if(*(mode + 1) == '\0'){
				fd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
				fp -> flag = A;
			}
			else if(*(mode + 1) == '+'){
				fd = open(path, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
				fp -> flag = AP;
			}
			else
				fp -> flag = EOF;
			break;
		default:
			fp -> flag = EOF;
			break;
	}
	if(fd == -1){
		perror("Can't open file!");
		//free(fp);
		return NULL;
	}
	if(fp -> flag == EOF){
		fprintf(stderr,"Invalid Mode");//check if this works.
		return NULL;
	}
	fp -> fd = fd;
	fp -> buf = NULL;
	fp -> next = NULL;
	fp -> left = 0;
	return fp;
}

int buffill(File *fp){
	int r = INT_MIN;
	if(fp -> bufornot == 'u')//if stream is unbuffered, return 
		return 1;
	if(fp -> flag == R && fp -> buf == NULL){
		fp -> buf = malloc(BUFSIZE);
		if(fp -> buf == NULL){  /*can't get buffer, so try unbuffered*/
			fp -> bufornot = 'u';
			return INT_MIN;
		}
		fp -> bufornot = 'b';
		r = read(fp -> fd, fp -> buf, BUFSIZE);
		fp -> left = r;
		fp -> next = fp -> buf;
		//if(r == 0)
			//fp -> flag = EOF;
	}
	else if(fp -> flag == R && fp -> left == 0){
		r = read(fp -> fd, fp -> buf, BUFSIZE);
		fp -> left = r;
		fp -> next = fp -> buf;
	}
	if(r == 0)
		fp -> flag = EOF;
	return r;		
}

unsigned long Fread(void *ptr, unsigned long size, unsigned long nmem, File *fp){
	long toberead = size * nmem;
	int r = 0;
	void *tr;
	if(toberead > BUFSIZE){
		//dosomething maybe unbuffer
	}
	if(fp -> flag == RP || fp -> flag == WP || fp -> flag == AP){
		fp -> last = 'r';
	}
	if(fp == &ptrarr[0]){
		fp -> bufornot = 'u';
	}
	if(fp -> left == 0)
		 r = buffill(fp);
	if(/*r == INT_MIN || */fp -> flag == EOF)
		return 0;
	if(fp -> bufornot == 'u'){
		/*then read and write unbuffered;*/
		int rd = read(fp -> fd, ptr, toberead);
		if(rd < toberead){
			fp -> flag = EOF;
		}
		return rd / size;
	}
	if(fp -> left < toberead){
		lseek(fp -> fd, -toberead, SEEK_CUR);
		fp -> left = 0;
		r = buffill(fp);
		if(fp -> flag == EOF)
			return 0;
		if(fp -> left < toberead)
			toberead = fp -> left;
	}
	tr = memmove(ptr, fp -> next, toberead);
	fp -> next = fp -> next + toberead;
	fp -> left = fp -> left - toberead;
	return toberead / size;	
}

