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
	testFtell();
	testFseek();
	testFeof();
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
	char str1[2000] = "Richard Phillips Feynman", str2[2000], str3[2000] = "Richard Phillips Feynman was a great physicist!";
	int error = 9, i = 0, j = 0, flag = 0;
	long ret1;
	char str4[8] = "AAAAAA", str5[8];
	
	printf("TESTING Fwrite\ntesting \"w\" mode\n");
	fw = Fopen("writetest1", "w");
	if(fw == NULL){
		perror("cant open file\n");
		return 0;
	}
	else{
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
		Fclose(fr);
		if(strcmp(str1, str2) == 0){
			printf("Success\n");
		}
		else{
			printf("Failure\n");
			error = 1;
		}
	}
	
	/*testing Fwrite for writing non charachters: here integers*/
	
	printf("testing with binary file of size greater than buffer\n");
	fw = Fopen("writetest2", "w");
	if(fw == NULL){
		perror("cant open file\n");
		return 0;
	}
	else{
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
	}
	
	/*Now testing append mode*/
	printf("Now testing append mode \"a\", size lesser than buffer\n");
	strcpy(str1, " was a great physicist!");
	f = Fopen("writetest1", "a");
	if(f == NULL)
		perror("cant open file\n");
	else{
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
		
	}
	
	printf("case Text file greater than buffer\n");
	f = Fopen("test2", "r");
	fw = Fopen("writetest1", "a");
	if(f == NULL || fw == NULL)
		perror("cant open file\n");
	else{
		ret1 = Fread(str1, 1, 1508, f);
		str1[ret1] = '\0';
		Fclose(f);
		strcat(str2, str1);
		Fwrite(str1, strlen(str1), 1, fw);
		Fclose(fw);
		f = Fopen("writetest1", "r");
		if(f == NULL){
			perror(" ");
			flag = 1;
		}
		if(!flag){
			Fread(str1, 1555, 1, f);
			str1[1555] = '\0';
			if(strcmp(str1, str2) == 0)
				printf("Success\n");
			else
				printf("Failure\n");
		}
		printf("test case trying to write to a file open in read mode\n");
		Fseek(f, 0, SEEK_SET);
		/*Fseek tested later, for this testcase, please assume that fseek works properly*/
		Fwrite("this is test", 12, 1, f);
		Fclose(f);
		f = Fopen("writetest1", "r");
		if(f == NULL){
			perror("writetest1 ");
			flag = 1;
		}
		if(!flag){
			Fread(str1, 1555, 1, f);
			str1[1555] = '\0';
			/*str2 holds previous content of file before trying to write*/
			if(strcmp(str1, str2) == 0 && errno == EBADF)  
				printf("Success\n"); /*file content remains unmodified and errno set properly*/
			else
				printf("Failure\n");
			Fclose(f);
		}
		
		
	}
	printf("test case trying to read a file open in write mode\n");
	f = Fopen("writetest1", "w");
	if(f == NULL)
		perror(" ");
	else{
		strcpy(str5, str4);
		Fread(str4, 8, 1, f);
		/*str5 holds previous content of str4 before trying to read into str4*/
		if(strcmp(str5, str4) == 0 && errno == EBADF)  
			printf("Success\n"); /*string content remains unmodified and errno set properly*/
		else
			printf("Failure\n");
		Fclose(f);
	}
	printf("\n\n");
	return error;
}


int testFtell(){
	File *fr, *fw; /*my file functions*/
	FILE *frs, *fws;/*standard file functions*/
	char s0[2000], s1[2000];
	int i = 0, j = 0, k = 0;
	long r1, r2;
	fr = Fopen("test2", "r");
	frs = fopen("test2", "r");
	
	printf("TESTING Ftell\n");
	if(fr == NULL || frs == NULL)
		perror(" ");
	else{
		/*beginning of file: offset should be zero*/
		printf("case: text file\n");
		printf("case:beginning of file\n");
		r1 = Ftell(fr);
		r2 = ftell (frs);
		if(r1 == r2 && r1 == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		printf("case:smaller than buffer\n");/*value 12 is small enough to be inside buffer*/
		Fread(s0, 1, 12, fr);
		fread(s1, 1, 12, frs);
		r1 = Ftell(fr);
		r2 = ftell (frs);
		if(r1 == r2)
			printf("Success\n");
		else
			printf("Failure\n");
		printf("case:larger than buffer\n");
		Fread(s0, 1, 1495, fr);
		fread(s1, 1, 1495, frs);
		r1 = Ftell(fr);
		r2 = ftell (frs);
		if(r1 == r2)
			printf("Success\n");
		else
			printf("Failure\n");
		Fclose(fr);
		fclose(frs);
		
	}
	/*now testing on binary file*/
	fr = Fopen("file4", "r");
	frs = fopen("file4", "r");
	if(fr == NULL || frs == NULL)
		perror(" ");
	else{
		printf("case: binary file\n");
		printf("case:beginning of file\n");
		r1 = Ftell(fr);
		r2 = ftell (frs);
		if(r1 == r2 && r1 == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		printf("case: offset smaller than size of buffer\n");
		while(k < 30){
			Fread(&i, 1, sizeof(int), fr);
			fread(&j, 1, sizeof(int), frs);
			k++;
		}
		r1 = Ftell(fr);
		r2 = ftell (frs);
		if(r1 == r2 && r1 == 120)
			printf("Success\n");
		else
			printf("Failure\n");
		printf("case: offset larger than size of buffer\n");
		while(k < 258){
			Fread(&i, 1, sizeof(int), fr);
			fread(&j, 1, sizeof(int), frs);
			k++;
		}
		r1 = Ftell(fr);
		r2 = ftell (frs);
		if(r1 == r2 && r1 == 1032)
			printf("Success\n");
		else
			printf("Failure\n");
		Fclose(fr);
		fclose(frs);
	
	}
	printf("\n\n");
	return 1;
}

int testFseek(){
	printf("TESTING Fseek\n");
	printf("testing file smaller than buffer size\n");
	char a[1024], s[32], d[32] = "Stefanie Maria ", g[32] = "aria Steffi Graf (born", e1[16] = "ime list .", e2[16];
	char e3[32] = "f (born 14 June 1969)", e4[32];
	File *f = Fopen("test1", "r");
	int i = 9;
	char s1[2000], s2[16], s3[16] = "Albert Einstein", s4[32] = "ntum theory, which led to his", s5[32];
	
	/*this file is small enough to fit in buffer*/
	if(f == NULL)
		perror(" ");
	else{
		printf("case:negative position argument for SEEK_SET\n");//do it
		i = Fseek(f, -10, SEEK_SET);
		if(i == 1 && errno == EINVAL)
			printf("Succesfully handled\n");
		else
			printf("Test not passed\n"); 
			
		printf("case:bad input for argument whence\n");//do it
		i = Fseek(f, -10, 5);
		if(i == 1 && errno == EINVAL)
			printf("Succesfully handled\n");
		else
			printf("Test not passed\n"); 
			
		printf("testing SEEK_SET\n");
		Fread(a, 1, 1000, f); 
		Fseek(f, 0, SEEK_SET);
		Fread(s, 1, 15, f);
		s[15] = '\0';
		if(strcmp(s, d) == 0)
			printf("Success\n");
		else 
			printf("Failure\n");
		Fseek(f, 10, SEEK_SET);
		//printf("%d\n", i);
		Fread(s, 1, 22, f);
		s[22] = '\0';
		//printf("s = %s\ng = %s\n", s, g);
		if(strcmp(s, g) == 0)	
			printf("Success\n");
		else 
			printf("Failure\n");
			
			
		Fseek(f, 0, SEEK_SET);
		Fread(s, 1, 15, f);
		s[15] = '\0';
		if(strcmp(s, d) == 0)
			printf("Success\n");
		else 
			printf("Failure\n");	
			
			
		printf("Testing SEEK_END\n");
		Fseek(f, -11, SEEK_END);
		Fread(e2, 1, 10, f);
		e2[10] = '\0';
		if(strcmp(e1, e2) == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		
		printf("Testing SEEK_CUR\n");
		Fseek(f, 20, SEEK_SET);
		Fseek(f, 5, SEEK_CUR);
		Fread(e4, 1, 21, f);	
		e4[21] = '\0';
		if(strcmp(e3, e4) == 0)
			printf("Success\n");
		else
			printf("Failure\n");	
		Fclose(f);
	}
	
	f = Fopen("test2", "r");
	if(f == NULL)
		perror("test2:");
	else{
		printf("testing file larger than buffer size\n");
		Fread(s1, 1, 1508, f);
		printf("testing SEEK_SET\n");
		Fseek(f, 0, SEEK_SET);
		Fread(s2, 1, 15, f);
		s2[15] = '\0';
		if(strcmp(s2, s3) == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		
		printf("testing SEEK_CUR\n");
		Fseek(f, 1200, SEEK_CUR);
		Fread(s5, 1, 30, f);
		s5[30] = '\0';
		if(strcmp(s5, s4) == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		
		printf("Testing SEEK_END\n");
		Fseek(f, -10, SEEK_END);
		Fread(s5, 1, 8, f);
		s5[8] = '\0';
		if(strcmp(s5, "universe") == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		
		Fclose(f);
	}
	printf("\n\n");
}

int testFeof(){
	File *f;
	FILE *f1;
	char tes[1024], fromfile[1024], tes2[1024], ch;
	int i = 0, j = 0, error = 0;
	
	printf("TESTING Feof\n");
	printf("case:file smaller than buffer\n");
	f = Fopen("test1", "r");
	f1 = fopen("test1", "r");
	if(f == NULL || f1 == NULL)
		perror(" ");
	else{
		while(!Feof(f)){
			Fread(&ch, 1, 1, f);
			tes[i] = ch;
			i++;
		}
		tes[i] = '\0';
		Fclose(f);
		i = 0;
		while(!feof(f1)){
			fread(&ch, 1, 1, f1);
			tes2[i] = ch;
			i++;
		}
		tes2[i] = '\0';
		if(strcmp(tes, tes2) == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		fclose(f1);
	}
	
	printf("case: binary file larger than buffer\n");
	f = Fopen("file4", "r");
	f1 = fopen("file4", "r");
	if(f == NULL || f1 == NULL)
		perror(" ");
	else{
		while(!Feof(f) || !feof(f1)){
			Fread(&i, 1, sizeof(int), f);
			fread(&j, 1, sizeof(int), f1);
			if(i != j){
				error = 1;
				printf("Failure\n");
				break;
			}
		}
		if(!error)
			printf("Success\n");
		fclose(f1);
		Fclose(f);
	}
	
	printf("\n\n");
	return 1;
}

void testStdinouterr(){
	char s0[] = "This is test file used to test Fread and Stdin.", s2[64] = "Testing the working of stdout. This should appear on screen.", s1[64];;
	
	printf("TESTING Stdin\n");
	Fread(s1, 1, 47, Stdin);
	s1[47] = '\0';
	printf("%s\n", s1);
	if(strcmp(s0, s1) == 0)
		printf("Success\n");
	else
		printf("Failure\n");
	
	printf("Testing Stdout\n");
	Fwrite(s2, 1, strlen(s2), Stdout);
	/*stdout buffer gets flushed after fflush*/
	Fflush(Stdout);
	printf("\nTesting Stderr\n");
	Fwrite("writing to stderr is unbuffered. therefore Fflush() is not required.This should print on screen immediately\n", 1, 108, Stderr);
	printf("\n\n");
}

int testFgetorsetpos(){
	File *f;
	f = Fopen("file4", "r");
	int arr[10], i, j;
	char s0[64] = "Neutron stars are the smallest and densest stars", s1[8];
	Fpos_t st1, st2;
	
	printf("TESTING Fgetpos and Fsetpos\n");
	printf("Testing in read mode\n");
	/*Now testing in read mode*/
	if(f == NULL)
		perror("file3");
	else{
		Fgetpos(f, &st1);
		Fread(arr, sizeof(int), 10, f);
		Fgetpos(f, &st2);
		Fsetpos(f, &st1);
		Fread(&i, sizeof(int), 1, f);
		Fsetpos(f, &st2);
		Fread(&j, sizeof(int), 1, f);
		if(i == 260 && j == 250)
			printf("Success\n");
		else
			printf("Failure\n");
			
		Fclose(f);
	}
	/*Now testin in write mode*/
	printf("Testing in write mode\n");
	f = Fopen("testingF_post", "w");
	if(f == NULL)
		perror("testingF_post");
	else{
		Fgetpos(f, &st1);
		Fwrite(s0, 1, strlen(s0), f);
		Fsetpos(f, &st1);
		Fwrite("HHH", 1, 3, f);
		Fclose(f);
		Fopen("testingF_post", "r");
		if(f == NULL)
			perror("testingF_post");
		else{
			Fread(s1, 1, 7, f);
			s1[7] = '\0';
			if(strcmp(s1, "HHHtron") == 0)
				printf("Success\n");
			else
				printf("Failure\n");
			Fclose(f);
		}
	}
	printf("\n\n");
	return 1;
}

void testPlusmodes(){
	File *f;
	char s0[32], s1[8], s2[16];
	
	printf("TESTING plusmodes like \"r+\", \"w+\" and \"a+\"\n");
	/*testing "r+"*/
	printf("Testing \"r+\"\n");
	f = Fopen("writetest", "r+");
	if(f == NULL)
		perror("");
	else{
		Fread(s0, 1, 24, f);
		s0[24] = '\0';
		Fseek(f, 0, SEEK_SET);
		Fwrite("HERE", 1, 4, f);
		Fseek(f, 0, SEEK_SET);
		Fread(s0, 1, 24, f);
		s0[24] = '\0';
		if(strcmp(s0, "HEREard Phillips Feynman") == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		Fclose(f);
	}
	/*testing "w+" */
	printf("Testing \"w+\"\n");
	f = Fopen("testwa+", "w+");
	if(f == NULL)
		perror("");
	else{
		Fwrite("Marie Sklodowska Curie", 1, 22, f);
		Fseek(f, 0, SEEK_SET);
		Fread(s1, 1, 5, f);
		s1[5] = '\0';
		if(strcmp(s1, "Marie") == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		Fclose(f);
	}
	
	/*testing "a+" */
	printf("Testing \"a+\"\n");
	f = Fopen("testwa+", "a+");
	if(f == NULL)
		perror("");
	else{
		Fwrite("Lise Meitner", 1, 12, f);
		Fseek(f, -12, SEEK_CUR);
		Fread(s2, 1, 12, f);
		s2[12] = '\0';
		if(strcmp(s2, "Lise Meitner") == 0)
			printf("Success\n");
		else
			printf("Failure\n");
		Fclose(f);
	}
}
