Title of Project: FILE I/O Library
Name: Urvee Ausekar
MIS: 111713007
Description:
This is implementation of following file library functions:
1.fopen
2.fclose
3.fread
4.fwrite
5.fgetpos
6.fsetpos
7.fseek
8.ftell
9.feof
I have named my FILE structure as File, the fpos_t sturcture as Fpos_t, and all of the function names start with capital F and have same spelling (Eg: Fopen,Feof,etc). I have used a char buffer of size 1024 bytes for buffering, and have implemented all functions using  buffer.In addition to above 9 functions, I have also written fflush because it is necessary for using stdout in fwrite .I have written the testsuite and some testfiles.
