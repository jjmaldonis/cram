#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
using namespace std;

// MAXVALUE is the N value the program will run to.
const unsigned long MAXVALUE = 10000000; //10 million
unsigned long N= 2; //size of the dominoes

unsigned long Mex(bool used_values[], unsigned long max);
//void MakeFalse(bool used_values[], unsigned long i);
//void MakeAllFalse(bool used_values[]);
int main()
{
    printf("HERE1");
	char writefilename[64];
	snprintf(writefilename, 64, "qsub_%lu", MAXVALUE);
	FILE* writefile;
	// Create the 4 arrays of size MAXVALUE and a boolean "used" array for mexing
	unsigned long sprague_n[MAXVALUE] = {0};
	unsigned long sprague_s[MAXVALUE] = {0};
	unsigned long sprague_l[MAXVALUE] = {0};
	unsigned long sprague_u[MAXVALUE] = {0};
	// Initallize all values to FALSE in used_values array.
	bool used_values[MAXVALUE] = { 0 };
	char line[64];
    printf("HERE2");

    snprintf(line, 64, "\0");

    // For now, just erase the previous file until I get it to read in and initialize the starting point.
    //writefile = fopen( writefilename, "w" );
    //fclose(writefile);

	unsigned long i = 0;
	unsigned long j = 0;

	// Open the file to read in the current contents if any exist.
    printf("HERE3");
	if(writefile = fopen( writefilename, "r" ))
    {
        printf("File open successfully.\n");
        while( fgets( line, 64, writefile ) != NULL )
        {
            char * pch;
        	pch = strtok (line,",");
        	if(pch != NULL) i = atoi( pch );
            pch = strtok (NULL, ",");
        	if(pch != NULL) sprague_n[i] = atoi( pch );
            pch = strtok (NULL, ",");
        	if(pch != NULL) sprague_s[i] = atoi( pch );
            pch = strtok (NULL, ",");
        	if(pch != NULL) sprague_l[i] = atoi( pch );
            pch = strtok (NULL, ",");
        	if(pch != NULL) sprague_u[i] = atoi( pch );
            //printf("%lu, %lu, %lu, %lu, %lu\n", i, sprague_n[i], sprague_s[i], sprague_l[i], sprague_u[i]);
        }
        fclose(writefile);
        // Rewrite file because we may have found a line at the end that was screwed up.
        writefile = fopen(writefilename, "w" );
        for(j = 0; j < i; j++)
		    fprintf(writefile, "%lu,%lu,%lu,%lu,%lu\n", j, sprague_n[j], sprague_s[j], sprague_l[j], sprague_u[j] );
        fclose(writefile);
        printf("Re-write file complete.\n")
    }
    else
    {
        printf("There was a problem opening the file.\n");
        writefile = fopen(writefilename, "w" );
        fclose(writefile);
    }
    printf("HERE4");

    // Ending this loop will leave us at the last i value read in. This data may not be correct because the data may not have finished writing, so we will start at this i value and overwrite it.

	// Open the file for appending.
	writefile = fopen( writefilename, "a" );

	// If the file already existed then we don't need to rewrite the initial values; so check before doing so.
	if( i == 0)
	{
        printf("File did not already exist, creating a new one.\n");
		// Create initial conditions
		for(i = 0; i < N; i++)
		{
			sprague_n[i] = 0;
			sprague_s[i] = 0;
			sprague_l[i] = 0;
			sprague_u[i] = 0;
		}
		sprague_n[N] = 1;
		sprague_s[N] = 1;
		sprague_l[N] = 1;
		sprague_u[N] = 0;
        i++;

		fprintf( writefile, "0,%lu,%lu,%lu,%lu\n", sprague_n[0], sprague_s[0], sprague_l[0], sprague_u[0] );
		fprintf( writefile, "1,%lu,%lu,%lu,%lu\n", sprague_n[1], sprague_s[1], sprague_l[1], sprague_u[1] );
		fprintf( writefile, "2,%lu,%lu,%lu,%lu\n", sprague_n[2], sprague_s[2], sprague_l[2], sprague_u[2] );
	}
    else
        printf("File already existed. i = %lu\n",i);

	// Calculate and write new values to file.
    // i is preset in the read-in.
    printf("i=%lu\n",i);
	for(i; i <= MAXVALUE; i++)
	{
		//sprague_n
        #pragma omp for
		for(unsigned long j = 0; j <= i-N; j++)
			used_values[(sprague_s[j] ^ sprague_s[i-j-N])] = true;
		sprague_n[i] = Mex(used_values, i);
		memset(used_values, 0, i);  //reset used_values array to empty

		//sprague_s
        #pragma omp for
		for(unsigned long j = 0; j <= i - N; j++)
			used_values[(sprague_l[j] ^ sprague_s[i-j-N])] = true;
        #pragma omp for
		for(unsigned long j = 1; j <= i - N; j++)
			used_values[(sprague_u[j] ^ sprague_s[i-j-N])] = true;
		sprague_s[i] = Mex(used_values, i);
		memset(used_values, 0, i);  //reset used_values array to empty

		//sprague_l
        #pragma omp for
		for(unsigned long j = 0; j <= i-N; j++)
			used_values[(sprague_l[j] ^ sprague_l[i-j-N])] = true;
        #pragma omp for
		for(unsigned long j = 1; j <= i-N-1; j++)
			used_values[(sprague_u[j] ^ sprague_u[i-j-N])] = true;
		sprague_l[i] = Mex(used_values, i);
		memset(used_values, 0, i);  //reset used_values array to empty

		//sprague_u
        #pragma omp for
		for(unsigned long j = 0; j <= i-N-1; j++)
			used_values[(sprague_l[j] ^ sprague_u[i-j-N])] = true;
		sprague_u[i] = Mex(used_values, i);
		memset(used_values, 0, i);  //reset used_values array to empty

		// Send output to file for in the form N,n,s,l,u
		fprintf(writefile, "%lu,%lu,%lu,%lu,%lu\n", i, sprague_n[i], sprague_s[i], sprague_l[i], sprague_u[i] );
	}
	fclose(writefile);
	return 0;
}

unsigned long Mex(bool used_values[], unsigned long max)
//Minimum excluded value (i.e. Mex([0,1,2,4,7])=3 or Mex([1,4])=0 )
{
	for(unsigned long i = 0; i < max; i++)
	{
		if(used_values[i] != true)
			return i;
	}
	return MAXVALUE + 1;
}
