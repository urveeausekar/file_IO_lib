#include"myFiles.h"
#include<stdio.h>
#include<limits.h>
#include<string.h>

File ptrarr[MAXOPEN] = {{0, NULL, R, NULL, 0, '\0', 'b'}, {1, NULL, W, NULL, 0, '\0', 'b'}, {2, NULL, A, NULL, 0, '\0', 'u'}};

File *Fopen(const char *path, const char *mode){
	File *fp /*= malloc(sizeof(File))*/;
	if(path == NULL || mode == NULL){ /*Invalid input*/
		errno = EINVAL;
		return NULL;
	}
	int fd, i = 0;
	/*for(i = 0; i < MAXOPEN; i++){
		printf("%d th structure:\n", i);
		printf("fd = %d,", ptrarr[i].fd);
		if(ptrarr[i].buf == NULL)
			printf("NULL,");
		printf("%d", ptrarr[i].flag);
		if(ptrarr[i].next == NULL)
			printf("NULL,");
		printf("left = %d, last = %c, bufornot = %c\n", ptrarr[i].left, ptrarr[i].last, ptrarr[i].bufornot );
	}*/
	for(fp = ptrarr + 3 ; fp < ptrarr + MAXOPEN; fp++, i++){
		printf("%d th iteration\n", i);
		if(/*fp -> flag == EnOF || */(fp -> buf == NULL && fp -> next == NULL && fp -> flag == 0 && fp -> left == 0 && fp -> bufornot != 'u' && fp -> last == '\0'))
			break;
	}
	printf("ith structure %ld\n", fp - ptrarr);
	if(fp >= ptrarr + MAXOPEN) 
		return NULL;
	switch(*mode){
		case 'r':
			if(*(mode + 1) == '\0' || (*(mode + 1) == 'b' && *(mode + 2) == '\0')){
				fd = open(path, O_RDONLY);
				fp -> flag = R;
			}
			else if((*(mode + 1) == '+' && *(mode + 2) == '\0') || (*(mode + 1) == 'b' && *(mode + 2) == '+' && *(mode + 3) == '\0') || (*(mode + 1) == '+' && *(mode + 2) == 'b' && *(mode + 3) == '\0')){
				fd = open(path, O_RDWR);
				fp -> flag = RP;
			}
			else
				fp -> flag = EnOF;
			break;
		case 'w':
			if(*(mode + 1) == '\0' || (*(mode + 1) == 'b' && *(mode + 2) == '\0')){
				fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				fp -> flag = W;
			}
			else if((*(mode + 1) == '+' && *(mode + 2) == '\0') || (*(mode + 1) == 'b' && *(mode + 2) == '+' && *(mode + 3) == '\0') || (*(mode + 1) == '+' && *(mode + 2) == 'b' && *(mode + 3) == '\0')){
				fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				fp -> flag = WP;
			}
			else
				fp -> flag = EnOF;
			break;
		case 'a':
			if(*(mode + 1) == '\0' || (*(mode + 1) == 'b' && *(mode + 2) == '\0')){
				fd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				fp -> flag = A;
			}
			else if((*(mode + 1) == '+' && *(mode + 2) == '\0') || (*(mode + 1) == 'b' && *(mode + 2) == '+' && *(mode + 3) == '\0') || (*(mode + 1) == '+' && *(mode + 2) == 'b' && *(mode + 3) == '\0')){
				fd = open(path, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				fp -> flag = AP;
			}
			else
				fp -> flag = EnOF;
			break;
		default:
			fp -> flag = EnOF;
			break;
	}
	if(fd == -1){
		perror("Can't open file!");
		//free(fp);
		return NULL;
	}
	if(fp -> flag == EnOF){
		fprintf(stderr,"Invalid Mode");//check if this works.
		errno = EINVAL;
		return NULL;
	}
	fp -> bufornot = '\0';
	fp -> fd = fd;
	fp -> buf = NULL;
	fp -> next = NULL;
	fp -> left = 0;
	return fp;
}

/*reads BUFSIZE amount of data from file into buffer*/
int buffill(File *fp){
	int r = INT_MIN;
	/*if stream is going to be unbuffered, return */
	if(fp -> bufornot == 'u')
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
			//fp -> flag = EnOF;
	}
	else if((fp -> flag == R || fp -> flag == RP || fp -> flag == AP || fp -> flag == WP)  && fp -> left == 0){
		r = read(fp -> fd, fp -> buf, BUFSIZE);
		fp -> left = r;
		fp -> next = fp -> buf;
		/*when r < BUFSIZE, end of file approaches*/
	}
	if(r == 0)
		fp -> flag = EnOF;
	return r;		
}

/*writes everything in buffer to file*/
int bufflush(File *fp){
	int w = 0;
	if(fp -> bufornot == 'u')//If stream is unbuffered, return;
		return 1;
	if((fp -> flag == W || fp -> flag == A || fp -> flag == RP || fp -> flag == AP || fp -> flag == WP) && fp -> buf == NULL){
		fp -> buf = malloc(BUFSIZE);
		if(fp -> buf == NULL){     /*can't get buffer, so just do it without buffer*/
			fp -> bufornot = 'u';
			return INT_MIN;
		}
		fp -> left = BUFSIZE;
		fp -> next = fp -> buf;
	}
	else if(fp -> flag == W || fp -> flag == A || fp -> flag == RP || fp -> flag == AP || fp -> flag == WP){
		w = write(fp -> fd, fp -> buf, BUFSIZE - fp -> left);
		if(w < BUFSIZE - fp -> left)
			fp -> flag = EnOF;
		fp -> left = BUFSIZE;
		fp -> next = fp -> buf;
	}
	return 1;
}

unsigned long Fread(void *ptr, unsigned long size, unsigned long nmem, File *fp){
	long toberead = size * nmem;
	int r = 0;
	void *tr;
	if(fp -> flag == EnOF)
		return 0;
	/*If size of buffer is less than amount to be read , it is better to call read
	directly rather than call buffill() n number of times, because each buffill() will call read once*/
	
	if(toberead > BUFSIZE){
		/*this ensures that unbuffered read begins at correct position*/
		lseek(fp -> fd, -(fp -> left), SEEK_CUR); 
		fp -> bufornot = 'u';
		//need true lseek, no playing around with buffer 
	}
	fp -> last = 'r';
	if(fp -> left == 0 || fp -> buf == NULL)//what
		 r = buffill(fp);
	if(/*r == INT_MIN || */fp -> flag == EnOF){
		return 0;
	}
		
	if(fp -> bufornot == 'u'){        /*then read and write unbuffered; What if you already have something in buffer?*/
		//lseek(fp -> fd, -(fp -> left), SEEK_CUR); i think this should be here 
		int rd = read(fp -> fd, ptr, toberead);
		if(rd < toberead){
			fp -> flag = EnOF;
		}
		fp -> bufornot = 'b';
		fp -> left = 0; /*this will ensure that next call to Fread is normally buffered*/
		return rd / size;
	}
	if(fp -> left < toberead){
		lseek(fp -> fd, -(fp -> left), SEEK_CUR);//should this be -(fp -> left)? ys, i think
		fp -> left = 0;
		r = buffill(fp);
		if(fp -> flag == EnOF)
			return 0;
		if(fp -> left < toberead)
			toberead = fp -> left;
	}
	/*if(fp -> left - toberead < BUFSIZE - r){
		toberead = fp -> left - (BUFSIZE - r); no need
	}*/
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
		//flush buffer ,call fseek, pos 0, seekcur . flushed buffer, but why fseek? no fseekv
		bufflush(fp);
		fp -> bufornot = 'u';
		// now you have to set bufornot to \0 again after you finish write;
		//done after write check if works
	}
	if(fp == &(ptrarr[2])){
		fp -> bufornot = 'u';
	}
	if(fp -> buf == NULL || fp -> left == 0){
		i = bufflush(fp);
		if(/*i == INT_MIN || */fp -> flag == EnOF)
			return 0;//what is this?
	}
	if(fp -> bufornot == 'u'){
		int wr = write(fp -> fd, ptr, towrite);
		if(wr < towrite){
			/*this is write error*/
                	fp -> flag = EnOF;
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
	int retval = 10;
	//*(fp -> next) = '\0';
	//fp -> next = fp -> next + 1;
	//fp -> left = fp -> left - 1;
	bufflush(fp);
	retval = close(fp -> fd);
	free(fp -> buf);
	//free(fp);
	fp -> last = '\0';
	fp -> buf = NULL;
	fp -> next = NULL;
	fp -> left = 0;
	fp -> bufornot = '\0';
	/*if(fp -> flag == EnOF)   why did i do this?
		return -1;*/
	fp -> flag = 0;
	return retval;
}

long Ftell(File *fp){
	long currpos;
	if(fp -> last == '\0')
		return 0; 
	currpos = lseek(fp -> fd, 0, SEEK_CUR);
	if(currpos == -1)
		return -1;
	if(fp -> flag == R || ((fp -> flag == RP || fp -> flag == WP || fp -> flag == AP) && fp -> last == 'r')){
		return (currpos - fp -> left);
	}
	else if(fp -> flag == W || fp -> flag == A || ((fp -> flag == RP || fp -> flag == WP || fp -> flag == AP) && fp -> last == 'w')){
		printf("in ftell this is fp -> left %d", fp -> left);
		printf("in ftell retval of ftell is %ld\n", currpos + BUFSIZE - fp -> left); 
		return (currpos + BUFSIZE - fp -> left);
	}
	else
		return -1;
	
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
		if(fp -> last == 'w' && pos > 0 && pos < fp -> left){
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
	if(fp -> last == 'r')
		buffill(fp);//this is new see if works sometime
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
