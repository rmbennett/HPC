#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>

int main(int argc, char *argv[])
{	
	int n;
	if(argc > 1)
	{
		n = atoi(argv[1]);
		printf("n = %d \n", n );
	}
	else
	{
		n = 512;
		printf("n = %d \n", n );
	}


	while(1){
		// Buffer containing one sample (left and right, both 16 bit).
		int16_t samples[n*2];
		unsigned cbBuffer=sizeof(samples);	// size in bytes of 
		
		// Read one sample from input
		int got=read(STDIN_FILENO, samples, cbBuffer);
		if(got<0)
		{
			fprintf(stderr, "%s : Read from stdin failed, error=%s.", argv[0], strerror(errno));
			exit(1);
		}
		else if(got==0)
		{
			break;	 // end of file
		}
		else if(got!=cbBuffer)
		{
			fprintf(stderr, "\n got = %d and cbBuffer = %d ", got, cbBuffer);
			fprintf(stderr, "%s : Did not receive expected number of bytes.\n", argv[0]);
			break;
		}
		
		// Copy one sample to output
		int done=write(STDOUT_FILENO, samples, cbBuffer);
		if(done<0)
		{
			fprintf(stderr, "%s : Write to stdout failed, error=%s.", argv[0], strerror(errno));
			exit(1);
		}
		else if(done!=cbBuffer)
		{
			fprintf(stderr, "%s : Could not read requested number of bytes from stream.\n", argv[0]);
		}
	}

	return 0;
}
