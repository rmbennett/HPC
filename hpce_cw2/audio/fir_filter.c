#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
 /* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>
#define NUM_SAMPLES 512
//Output left = ch1left+ch2left /2 
//Output right = ch1right+ch2right /2 

int main(int argc, char *argv[])
{	
	int stream1;
	int stream2;
	

	if(argc < 3)
	{
		fprintf(stderr, "Not enough inputs");
		exit(1);
	}
	else
	{
		//http://codewiki.wikidot.com/c:system-calls:open
		stream1 = open(argv[1], O_RDONLY);
		stream2 = open(argv[2], O_RDONLY);		
	}
	int16_t sample1[NUM_SAMPLES];
	int16_t sample2[NUM_SAMPLES];
	int16_t output_stream[NUM_SAMPLES];
	unsigned cbBuffer1=sizeof(stream1);	// size in bytes of 
	unsigned cbBuffer2=sizeof(stream2);	
	while(1)
	{
		// Buffer containing one sample (left and right, both 16 bit).
		
		// Read one sample from input
		int got_stream1=read(stream1, sample1, cbBuffer1);
		if(got_stream1<0)
		{
			fprintf(stderr, "%s : Read from stdin failed, error=%s.", argv[0], strerror(errno));
			exit(1);
		}
		else if(got_stream1==0)
		{
			break;	 // end of file
		}
		else if(got_stream1!=cbBuffer1)
		{
			fprintf(stderr, "\n got = %d and cbBuffer = %d ", got_stream1, cbBuffer1);
			fprintf(stderr, "%s : Did not receive expected number of bytes.\n", argv[0]);
			break;
		}

		// Read one sample from input
		int got_stream2=read(stream2, sample2, cbBuffer2);
		if(got_stream2<0)
		{
			fprintf(stderr, "%s : Read from stdin failed, error=%s.", argv[0], strerror(errno));
			exit(1);
		}
		else if(got_stream2==0)
		{
			break;	 // end of file
		}
		else if(got_stream2!=cbBuffer2)
		{
			fprintf(stderr, "\n got = %d and cbBuffer = %d ", got_stream2, cbBuffer2);
			fprintf(stderr, "%s : Did not receive expected number of bytes.\n", argv[0]);
			break;
		}

		for (int i = 0;  i < sizeof(sample2); ++i) {
			output_stream[i] = sample2[i]/2 + sample1[i]/2;
		}

		int done=write(STDOUT_FILENO, output_stream, cbBuffer2);
		if(done<0)
		{
			fprintf(stderr, "%s : Write to stdout failed, error=%s.", argv[0], strerror(errno));
			exit(1);
		}
		else if(done!=cbBuffer2)
		{
			fprintf(stderr, "%s : Could not read requested number of bytes from stream.\n", argv[0]);
		}


	}

	close(stream1);
	close(stream2);
	return 0;
}

