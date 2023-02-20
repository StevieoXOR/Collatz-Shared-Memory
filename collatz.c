/*Coded ~6 hours before 2/18/2023. Coded 12.5 consecutive hours (1400-0230) on 2/18/2023, then 8 hours (1500-2020,2200-0040) on 2/19/2023.*/
/*total: 26.5 hrs from 2/18/2023 to 2/20/2023*/
/*Issue to resolve for future projects: Why does snprintf() not work for concatenation even though sprintf() does?*/
/*Issue to resolve for this project: What specifically is the root cause of the SEGMENTATION FAULT when I kill the program in main in the child when seed<1?
 * I don't open the shared memory object in the child (so there's nothing to close), and (in the parent) I close any shared memory object that was opened.*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*For shared memory*/
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>



/*Global Variables*/
bool useInbuiltMethodsInsteadOfMyStrings = true;



int getNumDigits(int num, bool DEBUG)
{
	int numDigits = 0;
	int numCopy = num;

	while(numCopy!=0)
	{
		numDigits++;
		numCopy = numCopy/10;
	}
	if(DEBUG){printf("numDigits of %d: %d\n",num, numDigits);}
	return numDigits;
}

/*What happens if a string is input to num? I hope it doesn't happen, but prob compiler error*/
char* intToStr(int num, char* outputStr, bool DEBUG)
{
	const int numDigits = getNumDigits(num,DEBUG);

	/*Would create a local variable (ptrToCharArray, i.e. string), which should be deleted after
	 * its scope is exited (i.e. when this function exits, the pointer shouldn't be
	 * available any more. To fix, I moved this out of the function and into
	 * whatever called this function.*/
	//char str[numDigits+1];	/*Allocate (Make space for) random data*/
	
	/*Prepare the string for usage*/
	for(int i=0; i<(numDigits+1); i++)
		{outputStr[i] = 48;}	/*str[i] = nonNull# so there is no early string termination*/
	outputStr[numDigits] = '\0';	/*Last char is null-terminating character because it's
				  not added by default.*/
	
	int numCopy = num;
	
	/*for() has inverted i because it needs to count down because the
	 * least significant # will be on the leftmost digit of the array instead
	 * of the rightmost digit like you would expect any # to be.*/
	/*Start at i=BiggestArrayIndex, go until i==0 (SmallestArrayIndex)*/
	for(int i=numDigits-1; i>=0; i--)
	{
		outputStr[i] = numCopy%10;	/*str[i] = leastSignificantDigit*/
		/*Now each ASCII value is in str. I need to convert charVal1 into '1'*/
		
		/*if(str[i]==0){str[i] = '0';} then do that for every string element.
		 *charOffsetFor(0->Digits)Is48*/
		outputStr[i] += 48;

		numCopy /= 10;		/*num = num/10;   shiftBase10NumToRightBy1*/
	}


	if(DEBUG){printf("intToStr(%d): %s\n",num,outputStr);}
	return outputStr;
}

void printStrOneCharAtATime(char* str)
{
	/* if(currCharIsNotNull){printChar();} */
	for(int i=0; str[i]!='\0'; i++)
		{printf("%c",str[i]);}
		/*DON'T USE %s BECAUSE THIS GOES 1 CHAR AT A TIME INSTEAD OF MULTIPLE*/
}






/*Collatz Function*/
/*str_sequenceToWriteToSharedMem is appended to with each collatz() iteration*/
/*indexOfCurrChar example:
 * Given strSequence=="abcd``````````````````````" ('`' signifies extra space in strSequence),
 * indexOfCurrChar=3 (maybe 4 bc next available char to write)*/
int collatz(int seed, const bool DEBUG, char* str_sequenceToWriteToSharedMem, int indexOfCurrChar, const int MAX_COLLATZ_SEQUENCE_CHARS)
{
	if(seed==0)
	{
		printf("0 is not permitted. Only positive integers are permitted.\n");
		return -1;
	}
	/*Utilize the fact that -#s are not in the domain of the collatz theorem*/
	else if((seed*3+1) < 0)	/*if(operationCausesOverflowIntoNegativeRange){exitFunction();}*/
	{
		printf("Woah there, that becomes a big #. Try a smaller (but positive and nonzero) value.\n");
		return -1;
	}
	/*if(operationCausesOverflowIntoPositiveRange){exitFunction();}*/
	else if(((seed*3+1) > 0) && (seed<0))
	{
		printf("1) Negative #s are not permitted.\n");
		printf("2) That is a very large negative value. Try a smaller (but positive and nonzero) value.\n");
		return -1;
	}
	else if(seed<1)
	{
		printf("Both negative #s and 0 are not permitted. Only positive integers are permitted.\n");
		return -1;
	}


	int len_newShMemStr = 0;
	
	if(useInbuiltMethodsInsteadOfMyStrings)
	{
		/*I didn't use this single-line-commented-out stuff because I built my own and the default methods seemed complicated. Ironic in hindsight.*/
		/*Append int seed to string*/
		/*MIN_INT == -2^32 == -4294967296\0  which is 11 chars (10 digits+'\0'). If I used '-' for negative #s, I would need one more byte*/
		/*int maxNumBytesNeededByASCIIRepresentationOfOneInt = 11;
		UNUSED*/

		/*newStringLength_bytes = oldStringLength_bytes + 11_bytes+1byteForComma */
		/*int maxTotalNewStrLen_bytes = strlen(str_sequenceToWriteToSharedMem) + maxNumBytesNeededByASCIIRepresentationOfOneInt+1;
		if(DEBUG){printf("maxTotalNewStrLen_bytes: %d\n", maxTotalNewStrLen_bytes);}
		UNUSED*/

		/*if(1stCharIsDefaultValue (notA#)){setAllAllocatedDataTo_EndOfStringChar(NULL) (so concatenation happens with an empty array);}*/
		/*This is specifically for sprintf(). I have no idea if snprintf() needs this because it only ever printed the seed with nothing else.*/
		if(str_sequenceToWriteToSharedMem[0]==',')
		{
			for(int i=0; i<MAX_COLLATZ_SEQUENCE_CHARS; i++)
			{str_sequenceToWriteToSharedMem[i] = (char)NULL;}
		}

		int numCharsWritten = 0;
		if(DEBUG){printf("strSequence before sprintf(): %s\n",str_sequenceToWriteToSharedMem);}
							/*snprintf(updatedStr,len_updatedStr,  "strToWriteToUpdatedStr",stuffToReplaceTheFormatSpecifiers,formatReplacer2,...)*/
		/*WHY YOU NO WORK snprintf()?????????*/
		//printf("snprintf() chars written: %d\n", snprintf(str_sequenceToWriteToSharedMem, maxTotalNewStrLen_bytes,  "%s%d",str_sequenceToWriteToSharedMem,seed) );
		if(seed!=1){	numCharsWritten = sprintf(str_sequenceToWriteToSharedMem,  "%s%d,",str_sequenceToWriteToSharedMem,seed);}
		else/*last#*/{	numCharsWritten = sprintf(str_sequenceToWriteToSharedMem,  "%s%d", str_sequenceToWriteToSharedMem,seed);}
		if(DEBUG){printf("sprintf() chars written: %d\n", numCharsWritten);}
		if(DEBUG){printf("strSequence after sprintf(): %s\n",str_sequenceToWriteToSharedMem);}
	}
	else
	{
	
		int numDigitsInSeed = getNumDigits(seed, false/*DEBUG*/);

		/*Allocate str to be used as an output of the function intToStr()*/
		/*Allocate (Make space for) random data, what will become digits and a null char*/
		/*It's okay that this string is local because this string isn't output to whatever calls this collatz function*/
		char strTemp[numDigitsInSeed+1];


		/*It's okay that this string is local because this string isn't output to whatever calls this collatz function.*/
		/*The function intToStr() takes care of making the string usable, so all I had to do HERE was allocate space.
		 * HERE, I didn't have to ensure A) there is only one null char and also B) the null is at index numDigitsInSeed+1*/
		/*seed, but in string form instead of int*/
		char* seedAsStr     = intToStr(seed, strTemp, DEBUG);

		/*Example:
		 * Given strSequence=="abcd````````"  and  afterAppend_strSequence="abcd,123````",
		 * length_newSharedMemoryString==strlen("abcd,123")*/
		/*int*/ len_newShMemStr = (indexOfCurrChar+numDigitsInSeed+1);	/*+1 for comma separator between seeds*/
		if(DEBUG)
		{
			printf("Printing seedAsStr: ");
			printStrOneCharAtATime( seedAsStr );

			printf("\nCollatz Function: indexOfCurrCharInShMemStr=%d, ",indexOfCurrChar);
			printf("strlen(sequence)=%zu, ",  strlen(str_sequenceToWriteToSharedMem));
			printf("strlen(seedAsStr)=%zu\n", strlen(seedAsStr));
		}

		/*One loop iteration corresponds to writing one of seed's digits (or the
		 * comma separator) to Shared Memory*/
		/*indexOfCurrChar: index of the most-recently-written (rightmost)
		 * char in str_sequenceToWriteToSharedMem*/
		for(int i=indexOfCurrChar, j=0;   i<len_newShMemStr;   i++, j++)
		{
			if(j<numDigitsInSeed)
			{
				/*Append seed to str_sequenceToWriteToSharedMem*/
				str_sequenceToWriteToSharedMem[i] = seedAsStr[j];
				if(DEBUG){printf("Collatz Function: strSequence[%d] = seedAsStr[%d];\n",i,j);}
			}
			//UPDATE: Since I set strSequence to have all chars be commas by default, this else{} block isn't necessary
			//else
			//{
				/*Both ways below work, but there's no point in executing both, only one is needed to do the job*/
				/*str_sequenceToWriteToSharedMem[ i ] = ',';
				if(DEBUG){printf("Collatz Function: strSequence[%d] = ',';\n",i);}*/
				/*str_sequenceToWriteToSharedMem[ len_newShMemStr-1 ] = ',';
				if(DEBUG){printf("Collatz Function: strSequence[ len_newShMemStr-1 (%d) ] = ',';\n", len_newShMemStr-1);}*/
			//}
		}
		/*Don't overwrite even one byte past the allocated bounds for the array*/
		if(len_newShMemStr < MAX_COLLATZ_SEQUENCE_CHARS)
			{str_sequenceToWriteToSharedMem[ len_newShMemStr ] = '\0';}
			/*Make the char after "###," (could be different # of digits than 3 digits) into a null character to end the char array and turn it into a string*/
		
		if(seed==1)
		{
			/*-1 because last (of what is used) index (len_newShMemStr-1) is ',', so overwrite it to make it the end of the char array, turning it into a string*/
			str_sequenceToWriteToSharedMem[ len_newShMemStr-1 ] = '\0';
			/*len_newShMemStr is okay until len_newShMemStr == sizeOfAllocatedCharArray.
			 * At the max size of the string, it fails and illegally overwrites data it doesn't own.*/
		}
	}




	if(DEBUG){printf("Collatz Function: seed==%d --- Sequence so far: {%s}\n", seed, str_sequenceToWriteToSharedMem);}
	

	if(seed==1)
	{
		if(DEBUG){printf("Collatz Function finished: Reached seed==1\n");}
		else{printf("\n");}/*Prints when collatz function completely ends*/

		return 1;
	}
	
	/*All even #s will have no remainder when divided by two*/
	bool seedIsEven = (seed%2)==0;
	if(seedIsEven)
	{
		if(DEBUG){printf("Collatz Function: %d/2\n\n",seed);}
		return collatz( (seed>>1)/*(seed/2)*/, DEBUG, str_sequenceToWriteToSharedMem, len_newShMemStr, MAX_COLLATZ_SEQUENCE_CHARS );
	}
	else/*if(seedIsOdd)*/
	{
		if(DEBUG){printf("Collatz Function: 3(%d)+1\n\n",seed);}	
		return collatz( (3*seed)+1, DEBUG, str_sequenceToWriteToSharedMem, len_newShMemStr, MAX_COLLATZ_SEQUENCE_CHARS );
	}
}

/*Main Function - Starting Point*/
/*This program should be executed like so: ./collatz 176 0   or   ./collatz 1 0   or   ./collatz 93*/
/*args[0]==pathToExecutableFile, args[1] == # inputted when executing the already-compiled file*/
int main(int numExecArgs, char* stringArrayOfExecutableArgs[])
{
	int seed = 1;	/*1 lets the program not exit inside the child,
			  unlike 0 or negative #s.*/
			/*1 is also much less random than say 172,
			 * hopefully mentally warning the programmer.*/
	if(numExecArgs>1)
		{seed = atoi( stringArrayOfExecutableArgs[1] );}

	bool DEBUG = 0;	/*Default DEBUG is false*/	
	if(numExecArgs==3)
		{DEBUG = (bool)(  atoi( stringArrayOfExecutableArgs[2] )  );}
	if(numExecArgs>3)
	{
		printf("Hey, there is a maximum of 3 executable arguments"); 
		printf(" [programName,collatzSeed#,debugVal in this order]!\n");
		exit(1);	/*Exit Program with Faulty Program error code*/
	}

	

	/*Parent creates duplicate of itself (i.e. parent creates a child)*/
	pid_t p = fork();	/*pid = Process IDentifier*/

	if(p>0)/*if(notChildProcess)*/
	{
		if(DEBUG){printf("Parent Process entered:\n");}
		/*Wait for the only child to exit().
		  More children requires far more wait()s.*/
		wait(NULL);

		/*Open and read the Shared Memory Object created by the child process */
		const int   SH_MEM_SIZE = 4096;   /* the size (in bytes) of shared memory object */
		const char* SH_MEM_NAME = "Must have same name wherever this is opened";  /* name of the shared memory object */ 
	        int         shm_fd;		/* shared memory file descriptor    */
	        void*       ptrToShMem;		/* pointer to shared memory object  */
				 
		/* open the shared memory object */
		shm_fd = shm_open(SH_MEM_NAME, O_RDONLY, 0666);
				     
		/* memory map the shared memory object */
		ptrToShMem = mmap(0, SH_MEM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
					 
		/* read from the shared memory object */
		printf("STUFF READ FROM SHARED MEMORY: %s\n", (char*)ptrToShMem);
					     
		/* remove the shared memory object */
		shm_unlink(SH_MEM_NAME);



		if(DEBUG){printf("Parent Process exited.\n\n\n\n");}
		exit(0);	/*Parent exits*/
	}
	//else/*if(childProcess)*/	/*DO NOT USE ELSE BECAUSE IT LETS ONLY THE PARENT EXECUTE*/
	if(p==0) /*if(childProcess)*/
	{
		if(DEBUG){printf("Child Process entered:\nDebug Enabled: True\n");}
		else{	  printf("\n\nDebug Enabled: False\n");}
		printf("\n***** useInbuiltMethodsInsteadOfMyStrings: ");
		if(useInbuiltMethodsInsteadOfMyStrings){printf("true\n\n");}
		else{printf("false\n\n");}

		/*Get user input num*/
		/*fgets(varToStoreStrIn,#charsToRead,reader);*/
		/*atoi(string) == 0 if string can't be converted to an int.
		 * This isn't an issue with this program because 0 is an
		 * illegal value.
		 * atoi() Example:  atoi("123")==123*/		
		if(numExecArgs<2)
		{
			printf("Enter your input seed #: ");

			const char maxNumDigits_seed = 10;
			/*^Used "char" because it's around 2x shorter than "short",
			 * taking up less memory.
			 * Notice how it isn't #digits='0' bc '0'==48.*/
			
			/*char array that holds seed, but in string form*/
			/*Allocates maxNumDigits_seed bytes of random data.
			 * Is not a useful string because it's unknown whether
			 * there is a null or not, as well as where it is.*/
			char strSeed[ (int)(maxNumDigits_seed) ];
			
			/*Get user input from general cmdline*/
			/*It seems that fgets() appends a null to strSeed
			 * (strSeed is a char array) because it hasn't crashed*/
			fgets(strSeed,(int)(maxNumDigits_seed),stdin); /*From stdio.h*/
			seed = atoi(strSeed);	/*ASCII to int*/
		}
		
		/*THIS CAUSES A SEGFAULT BECAUSE IT EXITS THE PROGRAM BEFORE DEALLOCATING
		 * THE SHARED MEMORY, BUT IT HASN'T EVEN BEEN OPENED YET, SO HOW DOES IT FAIL????????????????????????????????????????????*/	
		//if(seed<1)
		//{
		//	printf("Entered # must be an integer greater than 0.\n");
			
		//	/* remove the shared memory object */
		//	//shm_unlink(SH_MEM_NAME);

		//	exit(1);
		//	exit(1);	/*Gently exit Program with error code 1*/
		//}



		if(DEBUG){printf("Child called collatz(%d)\n",seed);}
		printf("Starting seed for Collatz Sequence: %d\n",seed);
		const int MAX_COLLATZ_SEQUENCE_CHARS = 4096;
		if(DEBUG){printf("Allocated %d bytes (same # of chars) to strSequence\n", MAX_COLLATZ_SEQUENCE_CHARS);}

		/*Allocates enough memory for a pretty big sequence*/
		char str_sequenceToWriteToSharedMem[MAX_COLLATZ_SEQUENCE_CHARS];
		/*char str[5]; IS NOT NULL-TERMINATED BY DEFAULT, MEANING READING FROM str
		 * MIGHT NEVER STOP, LEADING TO A SEGMENTATION FAULT BECAUSE IT TRIES TO
		 * READ EVERY LAST BIT OF DATA IT'S GIVEN, AND THEN IT STILL KEEPS READING.*/
		

		/*for() is necessary because the random data present in the string
		 * could have nulls throughout, making the string shorter than
		 * intended. Only the FIRST null that is read marks the end of the
		 * string; any nulls after the 1st null are irrelevant.*/
		/*Random data from a newly allocated char[] could have the very
		 * first character as a null, making an empty, useless string.*/
		for(int i=0; i<MAX_COLLATZ_SEQUENCE_CHARS; i++)
			{str_sequenceToWriteToSharedMem[i] = 44;}/*(char)44=','*/
			/*str[i] = nonNullChar==anythingBut0;*/
		
		
		/*Set last char of str to be null, ensuring this is a terminating string*/
		str_sequenceToWriteToSharedMem[ MAX_COLLATZ_SEQUENCE_CHARS-1 ] = '\0';
		/*Now that ik the char array terminates (i.e. is a string), I can
		 * try to print it and get its length*/
		if(DEBUG)
		{
			printf("Main(): strlen(strSequence) (should be 1 fewer than allocated) = %zu\n", strlen(str_sequenceToWriteToSharedMem));
			printf("Main(): strSequence (pre collatz()) = {%s}\n\n\n", str_sequenceToWriteToSharedMem);
		}
		
		/*collatz() updates strSequence*/
		int success = collatz( seed, DEBUG, str_sequenceToWriteToSharedMem, 0/*indexOfCurrChar*/, MAX_COLLATZ_SEQUENCE_CHARS );
		
		/*This line can still print without crashing, but it's just ugly to look at because it takes up console space.*/
		if(DEBUG && success!=-1){printf("Main(): strSequence (post collatz()) = {%s}\n", str_sequenceToWriteToSharedMem);}


		/*Open Shared Memory object for writing, then write to it*/	
		const int   SH_MEM_SIZE = MAX_COLLATZ_SEQUENCE_CHARS;	/* the size (in bytes) of shared memory object */
		const char* SH_MEM_NAME = "Must have same name wherever this is opened";	/* name of the shared memory object */
		int   shm_fd;	/* shared memory file descriptor */
		void* ptr;	/* pointer to shared memory object */
					 
		/* create the shared memory object */
		shm_fd = shm_open(SH_MEM_NAME, O_CREAT | O_RDWR, 0666);
					     
		/* configure the size of the shared memory object */
		ftruncate(shm_fd, SH_MEM_SIZE);
						 
		/* memory map the shared memory object */
		ptr = mmap(0, SH_MEM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
						     
		/* write to the shared memory object */
		sprintf((char*)ptr, "%s", str_sequenceToWriteToSharedMem);		 
		ptr = NULL;	/*Destroy pointer*/



		if(DEBUG){printf("Child Process exited.\n");}
		exit(0);
	}
}
