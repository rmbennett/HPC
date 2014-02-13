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
#define CHANNELS 2


double * coefficients;



int main(int argc, char *argv[])
{	
	//int stream1;
	//int stream2;
	
	FILE * coeffFile;

	if(argc < 2)
	{
		fprintf(stderr, "Not enough inputs");
		exit(1);
	}
	else
	{
		coeffFile = fopen(argv[1], "r");		
	}


	double tmp;
	int counter = 0;
	
	
	if (coeffFile) 
	{
    	while (fscanf(coeffFile, "%lf", &tmp)!=EOF)
        	counter++;
	    coefficients = malloc(counter*sizeof(double));
    	if (coefficients == NULL)
    	{
    		fprintf(stderr, "Not enough inputs\n");
    		exit(1);
    	}
    	rewind(coeffFile);
    	for(int i=0; i < counter; i++)
    	{	
			if(fscanf(coeffFile, "%lf", &coefficients[i]) == EOF)
    		{
	    		fprintf(stderr, "Failed\n");
    		}
    	}
    	
	}
	fclose(coeffFile);

	int16_t *samples = malloc(counter*sizeof(int16_t)*2); //Data read into here
	int16_t *oldBuffer = malloc(counter*sizeof(int16_t)*2);	//For buffer overflow.
	int16_t *output_stream = malloc(counter*sizeof(int16_t)*2); //Data piped to STDOUT
	int16_t *intermediate_stream = malloc(counter*sizeof(int16_t)*2); //Intermediate stream for copying old_buffer to samples
	unsigned cbBuffer= counter*sizeof(int16_t)*2;
	int end = (cbBuffer/(sizeof(int16_t)*CHANNELS));

	while(1)
	{	
		// Read sample from input
		int bytes_read = 0;
		while(bytes_read != cbBuffer)
		{
			int got=read(STDIN_FILENO, samples+(bytes_read/sizeof(int16_t)), cbBuffer-bytes_read);
			if(got<0)
			{
				fprintf(stderr, "%s : Read from stdin failed, error=%s.", argv[0], strerror(errno));
				exit(1);
			}
			else if(got==0)
			{
				break;	 // end of file //at some point deal with EO_Input
			}
		
		bytes_read += got;
		}
		
		for(int i = 0; i < end; ++i)
		{
			
			//FIR Filter reference - github.com/lawliet89/
			int16_t *read_ptr = samples + i*2; 
			double leftChannel = 0; 
			double rightChannel = 0;

			for(int j = 0; j<= i; ++j)
			{
				leftChannel += *(read_ptr)*coefficients[j];
				rightChannel += *(read_ptr + 1)*coefficients[j];		
				read_ptr = read_ptr-2;
			}
			
			read_ptr = oldBuffer+(CHANNELS*counter)-2;

			for(int j = i+1; j < counter; ++j)
			{
				leftChannel += *(read_ptr)*coefficients[j];
				rightChannel += *(read_ptr + 1)*coefficients[j];
				read_ptr = read_ptr-2;
			}

			output_stream[i*2] = (int16_t)leftChannel; //left output channel takes even values
			output_stream[i*2 + 1] = (int16_t)rightChannel; // right output channel takes odd values;
		}

		// Copy one sample to output
		int done=write(STDOUT_FILENO, output_stream, cbBuffer);
		if(done<0)
		{
			fprintf(stderr, "%s : Write to stdout failed, error=%s.", argv[0], strerror(errno));
			exit(1);
		}
		else if(done!=cbBuffer)
		{
			fprintf(stderr, "%s : Could not read requested number of bytes from stream.\n", argv[0]);
		}

		intermediate_stream = samples;
		samples = oldBuffer;
		oldBuffer = intermediate_stream;
	}

	
	return 0;
}