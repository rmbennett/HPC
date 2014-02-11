/*********/
// Sinegen and Sin_init Code taken from RTDSP Lab2 Code - submitted by Richard Bennett in 2013

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

// PI defined here for use in code
#define PI 3.141592653589793
// SINE_TABLE_SIZE - number of values in the sine lookup table - defined here for use in code
#define SINE_TABLE_SIZE 256
#define NUM_CHANNELS 2
#define NUM_SAMPLES 512

//* Sampling frequency in HZ. Must only be set to 8000, 16000, 24000 32000, 44100 (CD standard), 48000 or 96000 */
int sampling_freq = 44100;
// Array of data used by sinegen to generate sine. These are the initial values.
float y[3] = {0,0,0};
float x[1] = {1}; // impulse to start filter
float a0 = 1.4142; // coefficients for difference equation
float b0 = 0.707;
// Holds the value of the current sample
float sample;
float sine_freq = 500.0;

// An array of floats containing SINE_TABLE_SIZE elements
float table[SINE_TABLE_SIZE];
// Step variable for use in the quarter sinegen function
float step4 = 0;
// Modified step value to within Sine wave table values - for use in quarter sine wave table
float table_value;



/********************************** sine_init() **************************/	
void sine_init4()
{
	// Fill the lookup table with 256 sine data points across one quarter cycle.
	int j;
	for(j=0; j < SINE_TABLE_SIZE; j++)
	{
		table[j] = sin(((0.5*PI*j)/SINE_TABLE_SIZE));
	}
}



/********************************** sinegen4() **************************/
float sinegen4(void)
{
/* This code produces a variable frequency sine wave, using a
quarter-sine-wave lookup table.
*
* */
	float wave4 = 0; // Initialise a variable to store the sine wave datapoint values in.
// To create a sine wave from the quarter sinewave table data.
//For values in the first sinewave quadrant - no adjustment to the step value needs to be made.
	if (step4 < (SINE_TABLE_SIZE))
	{
		table_value = step4;
		wave4 = table[(int)step4];
	}
//Second quadrant - step value must be adjusted to bring the value back into the range 0-255
	else if (step4 < (2*SINE_TABLE_SIZE) && (step4 >= SINE_TABLE_SIZE))
	{
		table_value = ((SINE_TABLE_SIZE-1)-(step4-SINE_TABLE_SIZE));

		wave4 = table[(int)((SINE_TABLE_SIZE-1)-(step4- SINE_TABLE_SIZE))];
	}
//Third quadrant - step value must be adjusted to bring the value back into the range 0-255 and the wave value negated
	else if (step4 < (3*SINE_TABLE_SIZE) && (step4 >= (2*SINE_TABLE_SIZE)) )
	{	
		table_value = (step4-(2*SINE_TABLE_SIZE));
		wave4 = -table[(int)(step4-(2*SINE_TABLE_SIZE))];
	}
//Fourth quadrant - step value must be adjusted to bring the value back into the range 0-255 and the wave value negated
	else if (step4 < (4*SINE_TABLE_SIZE) && (step4 >=(3*SINE_TABLE_SIZE)) )
	{
		table_value = ((SINE_TABLE_SIZE-1)-(step4-(3*SINE_TABLE_SIZE)));
		wave4 = -table[(int)((SINE_TABLE_SIZE-1)-(step4-(3*SINE_TABLE_SIZE)))];
	}
// assign step a value based on sampling frequency and desired output frequency to calculate next table value required.
	step4 += ((4*SINE_TABLE_SIZE)/(sampling_freq/sine_freq));
//To prevent step containing values greater than 4*SINE_TABLE_SIZE-1 which would cause the operation to overflow.
	if (step4 > ((4*SINE_TABLE_SIZE-1)))
	{
		step4 = step4 - (4*SINE_TABLE_SIZE-1);
	}
	
	return wave4;
}




int main(int argc, char *argv[])
{	
	
	if(argc > 1)
	{
		sine_freq = atoi(argv[1]);
//		printf("n = %d \n", n );
	}

	// initialises table of one quarter sinewave data
	sine_init4();
	int16_t samples[NUM_CHANNELS * NUM_SAMPLES];
    unsigned cbBuffer=sizeof(samples);
	// Loop endlessley generating a sine wave
	while(1)
	{

	// Calculate next sample from quarter sinewave data table
		for (int i = 0; i < NUM_CHANNELS * NUM_SAMPLES; i+=NUM_CHANNELS)
		{
			samples[i] = 3000*sinegen4();
			samples[i+1] = samples[i];
			//printf(" samples[i] = %d", samples[i]);
		}
		// Copy one sample to output
		int done=write(STDOUT_FILENO, samples, cbBuffer);
		//int done = cbBuffer;
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