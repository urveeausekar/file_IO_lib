project:function.o testsuite.o myFiles.h
	cc function.o testsuite.o -o project
	cat teststdin | ./project
function.o:function.c myFiles.h
	cc -c -Wall function.c
testsuite.o:testsuite.c myFiles.h
	cc -c -Wall testsuite.c
