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
	testFread();
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

/*function to test read action in all readable modes*/
int testFread(){
	File *f; /*my implementation of file functions*/
	FILE *fs; /* standard implementation of file functions*/
	int error = 0, ar1[512], ar2[512];
	char str1[2000], str2[2000];
	unsigned long ret1 = 10, ret2 = 10;
	int i = -7, j, k = 0;
	
	printf("TESTING Fread\n");
	printf("just read mode ie \"r\"\ncase:text file smaller than buffer\n");
	f = Fopen("test1", "r");
	fs = fopen("test1", "r");
	if(f == NULL || fs == NULL){
		perror("cant open file\n");
		return 0;
	}
	ret1 = Fread(str1, 1, 1000, f);
	ret2 = fread(str2, 1, 1000, fs);
	printf("ret1 = %lu ret2 = %lu\n", ret1, ret2);
	str1[ret1] = '\0';
	str2[ret2] = '\0';
	Fclose(f);
	fclose(fs);
	if(strcmp(str1, str2) == 0)
		printf("Success\n");
	else{
		printf("Failure: some error occured\n");
		error = 1;
		return 0;
	}
	
	printf("case:text file larger than buffer size\n");
	f = Fopen("test2", "r");
	fs = fopen("test2", "r");
	if(f == NULL || fs == NULL){
		perror("cant open file\n");
		return 0;
	}
	ret1 = Fread(str1, 1, 1508, f);
	ret2 = fread(str2, 1, 1508, fs);
	str1[ret1] = '\0';
	str2[ret2] = '\0';
	Fclose(f);
	fclose(fs);
	if(strcmp(str1, str2) == 0)
		printf("Success\n");
	else{
		printf("Failure: some error occured\n");
		error = 1;
		return 0;
	}
	
	printf("case:binary file smaller than buffer size\n");
	f = Fopen("file3", "rb");/*rb mode handled same as r mode in linux*/
	fs = fopen("file3", "r");
	if(f == NULL || fs == NULL){
		perror("cant open file\n");
		return 0;
	}
	while(i < 15){
		Fread(&i, sizeof(int), 1, f);
		fread(&j, sizeof(int), 1, fs);
		if(i != j){
			error = 1;
			break;
		}
	}
	if(error == 1){
		printf("Failure\n");
		error = 0;
	}
	else
		printf("Success\n");
		
	Fclose(f);
	fclose(fs);
	
	printf("case:binary file larger than buffer size\n");
	j = 0;
	f = Fopen("file4", "r");
	fs = fopen("file4", "r");
	if(f == NULL || fs == NULL){
		perror("cant open file\n");
		return 0;
	}
	while(k < 260){
		Fread(&i, sizeof(int), 1, f);
		fread(&j, sizeof(int), 1, fs);
		if(i != j){
			error = 1;
			break;
		}
		k++;
	}
	
	if(error == 1){
		printf("Failure\n");
		error = 0;
	}
	else
		printf("Success\n");
		
	Fclose(f);
	fclose(fs);
	
	
	return 1;
}
