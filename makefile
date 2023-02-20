#This line is a comment, led by '#', which is like // and /**/ but specifically for makefiles.
#make -s will silence all commands, stopping them from cluttering the console
#make  =  label that can be called as a command by "make" without the "".
make:
	#The next line cannot be left blank because
	#  the label "make:" will be treated as irrelevant to the code below.
	g++ -Wall collatz.c -pthread -lrt -o collatz
	#Compiler arguments
	#g++       = compiler to use
	#-Wall     = Show all warnings.
	#collatz.c = input file
	#?
	#-lrt does something with shared memory...?
	#-o        = Signal that the next argument will be an output file
	#collatz   = Name of the compiled file to output
	
	./collatz 127 1
	#Executable (Execution) arguments
	#./collatz = Filepath of executable file
	#127       = Collatz starting # (I call it "seed")
	#1         = Debug value (0=false, allOtherInts=true)



#test0  =  label that can be called as a command by "make test0" without the "".
	#Typing "test0" as a command won't work.
#This specific make has debug disabled
test0:
	g++ -Wall collatz.c -pthread -lrt -o collatz
	
	./collatz 44444444 0
	./collatz -44444444 0
	./collatz -1 0
	./collatz 0 0
	./collatz 1 0
	./collatz 127 0
	./collatz 64 0
	#Runs executable file with 7 different sets of executable arguments


#test1  =  label that can be called as a command by "make test1" without the "".
	#Typing "test1" as a command won't work.
#This specific make has debug enabled
test1:
	g++ -Wall collatz.c -pthread -lrt -o collatz
	
	./collatz 44444444 1
	./collatz -44444444 1
	./collatz -1 1
	./collatz 0 1
	./collatz 1 1
	./collatz 127 1
	./collatz 64 1
	#Runs executable file with 7 different sets of executable arguments
