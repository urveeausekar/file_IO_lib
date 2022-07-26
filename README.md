# File I/O Library

A project to implement standard file I/O functions from the Standard C library, using Linux system calls like open(), read(), write(), lseek(), etc. 

Functions Implemented:
	
	The following library functions have been implemented :
		- File *Fopen(const char *path, const char *mode)
    - unsigned long Fread(void *ptr, unsigned long size, unsigned long nmem, File *fp)
		- unsigned long Fwrite(void *ptr, unsigned long size, unsigned long nmem, File *fp)
		- int Fclose(File *fp)
		- long Ftell(File *fp)
		- int Fseek(File *fp, long pos, int whence)
		- int Feof(File *fp)
		- int Fsetpos(File *fp, Fpos_t *position)
		- int Fgetpos(File *fp, Fpos_t *position)
		- int Fflush(File *fp)
    
   To avoid redefining the standard functions, the names are slightly different i.e Fopen() corresponds to fopen() and so on.
   
    
Files :

	myFiles.h  
    - Contains definitions for struct File (my version of the standard struct FILE), struct Fpos_t (my version of the standard struct fpos_t) and typedefs.
	function.c
		- Contains the implementation code for the functions.
	testsuite.c
		- A testsuite for the this project.
    
  All other files contain test data for the testsuite.
    
-------------------------------------------------------------------------------------	
How to build, run and test the project:
-------------------------------------------------------------------------------------

Requirements: gcc, make

In the folder which contains all the source files, run the following commands on the terminal:

	1) make
	This will compile and link the code, and will run the testsuite.
  The testsuite can be run again by typing:
  1) ./project
