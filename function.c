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
			if(*(mode + 1) == '\0' || (*(mode + 1) == 'b' && *(mode + 2) == '\0')){
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
			if(*(mode + 1) == '\0' || (*(mode + 1) == 'b' && *(mode + 2) == '\0')){
				fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				fp -> flag = W;
			}
			else if(*(mode + 1) == '+'){
				fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				fp -> flag = WP;
			}
			else
				fp -> flag = EOF;
			break;
		case 'a':
			if(*(mode + 1) == '\0' || (*(mode + 1) == 'b' && *(mode + 2) == '\0')){
				fd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				fp -> flag = A;
			}
			else if(*(mode + 1) == '+'){
				fd = open(path, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
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
	fp -> bufornot = '\0';
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
	if((fp -> flag == R || fp -> flag == RP || fp -> flag == AP || fp -> flag == WP) && fp -> buf == NULL){
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
	else if((fp -> flag == R || fp -> flag == RP || fp -> flag == AP || fp -> flag == WP)  && fp -> left == 0){
		r = read(fp -> fd, fp -> buf, BUFSIZE);
		fp -> left = r;
		fp -> next = fp -> buf;
	}
	if(r == 0)
		fp -> flag = EOF;
	return r;		
}

/*writes everything in buffer to file*///what will you do if there is a write error?
int bufflush(File *fp){
	int w = 0;
	if(fp -> bufornot == 'u')//If stream is unbuffered, return;
		return 1;
	if((fp -> flag == W || fp -> flag == A || fp -> flag == RP || fp -> flag == AP || fp -> flag == WP) && fp -> buf == NULL){
		fp -> buf = malloc(BUFSIZE);
		if(fp -> buf == NULL){  /*can't get buffer, so just do it without buffer*/
			fp -> bufornot = 'u';
			return INT_MIN;
		}
		fp -> left = BUFSIZE;
		fp -> next = fp -> buf;
	}
	else if(fp -> flag == W || fp -> flag == A || fp -> flag == RP || fp -> flag == AP || fp -> flag == WP){
		w = write(fp -> fd, fp -> buf, BUFSIZE - fp -> left);
		if(w < BUFSIZE - fp -> left)
			fp -> flag = EOF;
		fp -> left = BUFSIZE;
		fp -> next = fp -> buf;
	}
	return 1;
}

unsigned long Fread(void *ptr, unsigned long size, unsigned long nmem, File *fp){
	long toberead = size * nmem;
	int r = 0;
	void *tr;
	/*If size of buffer is less than amount to be read , it is better to call read
	directly rather than call buffill() n number of times, because each buffill() will call read once*/
	if(toberead > BUFSIZE){
		
		fp -> bufornot = 'u';
		//youre forgetting something else
	}
	fp -> last = 'r';
	/*if(fp == &ptrarr[0]){
		fp -> bufornot = 'u';       check if this works
	}*/
	if(fp -> left == 0)
		 r = buffill(fp);
	if(/*r == INT_MIN || */fp -> flag == EOF)
		return 0;
	if(fp -> bufornot == 'u'){        /*then read and write unbuffered; What if you already have something in buffer?*/
		
		int rd = read(fp -> fd, ptr, toberead);
		if(rd < toberead){
			fp -> flag = EOF;
		}
		fp -> bufornot = 'b';
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


unsigned long Fwrite(void *ptr, unsigned long size, unsigned long nmem, File *fp){
	long towrite = size * nmem;
	int i = 0;
	fp -> last = 'w';
	/*If size of buffer is less than amount to be written , it is better to call write 
	directly rather than call bufflush() n number of times, because each bufflush() will call write once*/
	if(towrite > BUFSIZE){  
		//flush buffer ,call fseek, pos 0, seekcur 
		fp -> bufornot = 'u';
		// now you have to set bufornot to \0 again after you finish write;
		//done after write check if works
	}
	if(fp == &(ptrarr[2])){
		fp -> bufornot = 'u';
	}
	if(fp -> buf == NULL || fp -> left == 0){
		i = bufflush(fp);
		if(/*i == INT_MIN || */fp -> flag == EOF)
			return 0;
	}
	if(fp -> bufornot == 'u'){
		int wr = write(fp -> fd, ptr, towrite);
		if(wr < towrite){
			/*this is write error*/
                	fp -> flag = EOF;
		}
		fp -> bufornot = 'b';//check this
		return wr / size;
	}
	if(towrite > fp -> left)
		i = bufflush(fp);
	void *r = memmove(fp -> next, ptr, towrite);
	fp -> next = fp -> next + towrite;
	fp -> left = fp -> left - towrite;
	return towrite / size; 
}

int Fclose(File *fp){
	//*(fp -> next) = '\0';
	//fp -> next = fp -> next + 1;
	//fp -> left = fp -> left - 1;
	bufflush(fp);
	close(fp -> fd);
	free(fp -> buf);
	//free(fp);
	fp -> buf = NULL;
	fp -> next = NULL;
	fp -> left = 0;
	fp -> bufornot = '\0';
	if(fp -> flag == EOF)
		return -1;
	return 1;
}

long Ftell(File *fp){//havent dealt with all modes yet
	long currpos;
	if(fp -> last == '\0')
		return 0; 
	currpos = lseek(fp -> fd, 0, SEEK_CUR);
	if(currpos == -1)
		return -1;
	if(fp -> flag == R || fp -> last == 'r'){
		return (currpos - fp -> left);
	}
	else if(fp -> flag == W || fp -> flag == A || fp -> last == 'w'){
		printf("in ftell this is fp -> left %d", fp -> left);
		printf("in ftell retval of ftell is %ld\n", currpos + BUFSIZE - fp -> left); 
		return (currpos + BUFSIZE - fp -> left);
	}
	
}

int Fseek(File *fp, long pos, int whence){
	if(whence != SEEK_CUR && whence != SEEK_SET && whence != SEEK_END){
		errno = EINVAL;
		return 1;
	}
	if(whence == SEEK_SET && pos < 0){
		errno = EINVAL;
		return 1;
	}
	if(whence == SEEK_CUR){
		if(fp -> last == 'r' && pos > 0 && pos < fp -> left){
			fp -> next = fp -> next + pos;
			fp -> left = fp -> left - pos;
			return 0;
			
		}
		if(fp -> last == 'r' && pos < 0 &&  -(pos) < BUFSIZE - fp -> left){
			fp -> next = fp -> next + pos;
			fp -> left = fp -> left - pos;
			return 0;
		}
		if(fp -> last == 'w' && pos < 0 && -(pos) < BUFSIZE - fp -> left){
			printf("NOT using memset here\n");
			fp -> next = fp -> next + pos;
			fp -> left = fp -> left - pos ;
			return 0;
		}
		if(fp -> last == 'w' && pos > 0 && pos < fp -> left){//problems here? use mycat and see entire file is not read, but gedit shows entire file. but when i was not attempting to move ptr in buffer, things work fine
			printf("fp -> left = %d\n", fp -> left);
			printf("About to use memset\n");
			memset(fp -> next, '\0', pos);// * sizeof(pos));
			fp -> next = fp -> next + pos;
			fp -> left = fp -> left - pos;
			printf("fp -> left after = %d\n", fp -> left);
			return 0;
		}
	}
	//what if user tries to seek beyond buffer?
	//you should fill buffer
	switch(fp -> flag){
		case R:
			lseek(fp -> fd, -(fp -> left), SEEK_CUR);
			break;
		case W:
		case A:
			bufflush(fp);
			break;
		case AP:
		case WP:
		case RP:
			if(fp -> last == 'w')
				bufflush(fp);
			else if(fp -> last == 'r')
				lseek(fp -> fd, -(fp -> left), SEEK_CUR);
			break;
		default:
			return 1;
			break;
	}
	lseek(fp -> fd, pos, whence);
	return 0;
}

int Feof(File *fp){
	return (fp -> flag == EnOF);
}

int Fsetpos(File *fp, Fpos_t *position){
	if(position == NULL)
		return -1;
	int r = 20;
	r = Fseek(fp, position -> p, SEEK_SET);
	if(r == 0)
		return 0;
	else
		return -1;
}
int Fgetpos(File *fp, Fpos_t *position){
	if(position == NULL)
		return -1;
	position -> p = Ftell(fp);
	if(position -> p == -1)
		return -1;
	else 
		return 0;
}

int Fflush(File *fp){
	int r = bufflush(fp);
	if(r == 1)
		return 0;
	else
		return ENDF;   /*returns -1 on error*/
}
