#include<string.h>
#include"myFiles.h"
#include<stdio.h>
#include<errno.h>

int testFopen();
/* for testing the "r" mode */
int testFread();
/* for testing the "w" and "a" mode */
int testFwrite();
/*for testing file positioning systems*/
int testFseek();
int testFtell();
int testFgetpos();
int testFsetpos();
/*for testing the + modes ie "r+", "w+" and "a+"*/
int testPlusmodes();
int testFeof();
/*above functions will return 1 on success and 0 on failure*/

int main(int argc, char *argv[]){
	int r = testFopen();
	testFread();
	testFwrite();
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
	printf("\n\n");
	
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
		//write which error in some file?
		error = 1;
	}
	else{
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
	}
		
	printf("case:text file larger than buffer size\n");
	f = Fopen("test2", "r");
	fs = fopen("test2", "r");
	if(f == NULL || fs == NULL){
		perror("cant open file\n");
	}
	else{
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
	}
	
	printf("case:binary file smaller than buffer size\n");
	f = Fopen("file3", "rb");/*rb mode handled same as r mode in linux*/
	fs = fopen("file3", "r");
	if(f == NULL || fs == NULL){
		perror("cant open file\n");
		return 0;
	}
	else{
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
	}
	
	printf("case:binary file larger than buffer size\n");
	j = 0;
	f = Fopen("file4", "r");
	fs = fopen("file4", "r");
	if(f == NULL || fs == NULL){
		perror("cant open file\n");
		return 0;
	}
	else{
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
		printf("\n\n");
	}
	
	return 1;
}

int testFwrite(){
	File *fr, *fw, *f;
	char str1[64] = "Richard Phillips Feynman", str2[64], str3[64] = "Richard Phillips Feynman was a great physicist!";
	int error = 9, i = 0, j = 0;
	
	printf("TESTING Fwrite\ntesting \"w\" mode\n");
	fw = Fopen("writetest1", "w");
	if(fw == NULL){
		perror("cant open file\n");
		return 0;
	}
	Fwrite(str1, 1, strlen(str1), fw);
	Fclose(fw);
	/*testing Fwrite for writing charachters*/
	printf("testing with text file\n");
	fr = Fopen("writetest1", "r");
	if(fw == NULL){
		perror("cant open file\n");
		return 0;
	}
	printf("%ld\n", strlen(str1));
	Fread(str2, 1, 24, fr);
	str2[24] = '\0';
	//printf("%s\n", str2);
	if(strcmp(str1, str2) == 0){
		printf("Success\n");
	}
	else{
		printf("Failure\n");
		error = 1;
	}
	
	/*testing Fwrite for writing non charachters: here integers*/
	Fclose(fr);
	printf("testing with binary file of size greater than buffer\n");
	fw = Fopen("writetest2", "w");
	if(fw == NULL){
		perror("cant open file\n");
		return 0;
	}
	while(i < 300){
		Fwrite(&i, sizeof(int), 1, fw);
		i++;
	}
	
	Fclose(fw);
	fr = Fopen("writetest2", "r");
	if(fr == NULL){
		perror("cant open file\n");
		return 0;
	}
	i = 0;
	error = 0;
	while(i < 300){
		Fread(&j, sizeof(int), 1, fr);
		if(i != j){
			error = 1;
			break;
		}
		i++;
	}
	if(error == 1){
		printf("Failure\n");
		error = 0;
	}
	else
		printf("Success\n");
	Fclose(fr);
	
	/*Now testing append mode*/
	printf("Now testing append mode \"a\"\n");
	strcpy(str1, " was a great physicist!");
	f = Fopen("writetest1", "a");
	if(f == NULL){
		perror("cant open file\n");
		return 0;
	}
	Fwrite(str1, 1, 23, f);
	Fclose(f);
	f = Fopen("writetest1", "r");
	if(f == NULL){
		perror("cant open file\n");
		return 0;
	}
	Fread(str2, 1, 47, f);
	str2[47] = '\0';
	if(strcmp(str2, str3) == 0){
		printf("Success\n");
	}
	else
		printf("Failure!\n");
	return error;
}
