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
	int window;	
	double top = 0;
	if(argc < 4)
	{
		fprintf(stderr, "Not enough inputs");
		exit(1);
	}
	else
	{
		//http://codewiki.wikidot.com/c:system-calls:open
		stream1 = open(argv[1], O_RDONLY);
		stream2 = open(argv[2], O_RDONLY);	
		window = atoi(argv[3]);	
	}
	int16_t sample1[window*4];
	int16_t sample2[window*4];
	unsigned cbBuffer1= window*sizeof(int16_t)*2;	
	unsigned cbBuffer2= window*sizeof(int16_t)*2;	
	double stream1_average[window*2];
	double stream2_average[window*2];
	double stream1_square = 0;
	double stream2_square = 0;
	//double stream1_sum;
	//double stream2_sum;
	double stream1_RMS = 0;
	double stream2_RMS = 0;
	double stream1_squared = 0;
	double stream2_squared = 0;

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
		}/*
		else if(got_stream1!=cbBuffer1)
		{
			fprintf(stderr, "\n got = %d and cbBuffer = %d ", got_stream1, cbBuffer1);
			fprintf(stderr, "%s : Did not receive expected number of bytes (1).\n", argv[0]);
			break;
		}
*/
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
		/*else if(got_stream2!=cbBuffer2)
		{
			fprintf(stderr, "\n got = %d and cbBuffer = %d ", got_stream2, cbBuffer2);
			fprintf(stderr, "%s : Did not receive expected number of bytes (2).\n", argv[0]);
			break;
		}
		*/
//http://rosettacode.org/wiki/Averages/Root_mean_square
		for (int i = 0;  i < window; i+=2) 
		{

			stream1_average[i] = (sample1[i] + sample1[(i)+1])/2; //AM[i] = (AL[i] + AR[i])/2 
			stream2_average[i] = (sample2[i] + sample2[(i)+1])/2; //BM[i] = (BL[i] + BR[i])/2 
			
			

		}


		for (int i = 0;  i < window; i++) 
		{

			stream1_square += stream1_average[i]*stream1_average[i]; //AM[i] = (AL[i] + AR[i])/2 
			stream2_square += stream2_average[i]*stream2_average[i]; //BM[i] = (BL[i] + BR[i])/2 

		}
	
			stream1_RMS = sqrt(stream1_square/window);	
			stream2_RMS = sqrt(stream2_square/window);	
			top += stream1_RMS*stream2_RMS;
			stream1_squared += stream1_RMS * stream1_RMS; //for bottom
			stream2_squared += stream2_RMS * stream2_RMS; //for bottom
	}

	//top = stream2_RMS * stream1_RMS;
	double bottom = sqrt(stream1_squared*stream2_squared);
	double r = top/bottom;
	printf("%lf\n", r);
	close(stream1);
	close(stream2);
	return 0;
}

