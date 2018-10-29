#include<string.h>
#include"myFiles.h"
#include<stdio.h>
#include<errno.h>

int testFopen();
int testFread();
int testFwrite();
int testFseek();
int testFtell();
int testFgetpos();
int testFsetpos();
int testFeof();

int main(int argc, char *argv[]){
	int r = testFopen();
	return 0;
}

int testFopen(){
	File *f;
	int error = 0;
	printf("TESTING Fopen\n");
	
	/*case both inputs wrong */
	printf("Test case: both arguments NULL in Fopen:\n");
	f = Fopen(NULL, NULL);
	if(f == NULL && errno == EINVAL){
		printf("Success\n");
	}
	else{
		printf("Failure: return value not NULL or errno not set correctly\n");
		error = 1;
	}
	
	/*wrong pathname case*/
	printf("Test case: NULL as pathname in Fopen:\n");
	f = Fopen(NULL, "r");
	if(f == NULL && errno == EINVAL){
		printf("Success\n");
	}
	else{
		printf("Failure: return value not NULL or errno not set correctly\n");
		error = 1;
	}
	/*two cases of bad mode as input :
	*case1 NULL
	*case2 not one of the standard modes
	*/
	printf("Test case: mode string as NULL in Fopen:\n");
	f = Fopen("test1", NULL);
	if(f == NULL && errno == EINVAL){
		printf("Success: badmode case1 handled\n");
	}
	else{
		printf("Failure: return value not NULL or errno not set correctly\n");
		error = 1;
	}
	
	printf("Test case: bad mode in Fopen:\n");
	f = Fopen("test1", "dr++");
	if(f == NULL && errno == EINVAL){
		printf("Success: badmode case2 handled\n");
	}
	else{
		printf("Failure: return value not NULL or errno not set correctly\n");
		error = 1;
	}
	/*case trying to open a nonexistant file in read mode*/
	
	printf("Test case: trying to open nonexistant file in read mode:\n");
	f = Fopen("test3456", "r");
	if(f == NULL && errno == ENOENT){
		printf("Success\n");
	}
	else
	{
		printf("Failure: return value not NULL or errno not set correctly\n");
		error = 1;
	}
	
	
	if(error != 1)
		return 1;
	else
		return 0;
	
}
