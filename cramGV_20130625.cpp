#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
using namespace std;

// MAXVALUE is the N value the program will run to.
const unsigned long MAXVALUE = 10000000;
unsigned long N= 2; //size of the dominoes

unsigned long Mex(bool used_values[], unsigned long max);
void MakeFalse(bool used_values[], unsigned long i);
void MakeAllFalse(bool used_values[]);
int main()
{
	char writefilename[64];
	snprintf(writefilename, 64, "qsubGVs_%d", MAXVALUE);
	FILE* writefile;
    //ofstream writefile(writefilename);
	// Create the 4 arrays of size MAXVALUE and a boolean "used" array for mexing
	unsigned long sprague_n[MAXVALUE];
	unsigned long sprague_s[MAXVALUE];
	unsigned long sprague_l[MAXVALUE];
	unsigned long sprague_u[MAXVALUE];
	// Initallize all values to FALSE in used_values array.
	bool used_values[MAXVALUE] = { 0 };
	char buffer[64];

    // For now, just erase the previous file until I get it to read in and initialize the starting point.
    writefile = fopen( writefilename, "w" );
    fclose(writefile);

	// Open the file to read in the current contents if any exist.
	writefile = fopen( writefilename, "r" );
	unsigned long i = 0;
	while( fgets( buffer, 64, writefile ) != NULL )
	{
		sprague_n[i] = atoi( strtok (buffer,",") );
		sprague_s[i] = atoi( strtok (buffer,",") );
		sprague_l[i] = atoi( strtok (buffer,",") );
		sprague_u[i] = atoi( strtok (buffer,",") );
		i++;
	}
	fclose(writefile);

	// Open the file for writing.
	writefile = fopen( writefilename, "a" );

	// If the file already existed then we don't need to rewrite the initial values; so check before doing so.
	if( i == 0)
	{
        printf("File did not already exist, creating a new one.\n");
		// Create initial conditions
		for(unsigned long i = 0; i < N; i++)
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

		// Open the file for writing and write initial conditions to file.
		//writefile = fopen( writefilename, "r" );

		snprintf( buffer, 64, "0,%lu,%lu,%lu,%lu\n", sprague_n[0], sprague_s[0], sprague_l[0], sprague_u[0] );
        cout << buffer << endl;
		fprintf( writefile, buffer );
		fprintf( writefile, "1,%lu,%lu,%lu,%lu\n", sprague_n[1], sprague_s[1], sprague_l[1], sprague_u[1] );
		fprintf( writefile, "2,%lu,%lu,%lu,%lu\n", sprague_n[2], sprague_s[2], sprague_l[2], sprague_u[2] );
        //writefile << 0 << "," << sprague_n[0] << sprague_s[0] << sprague_l[0] << sprague_u[0] << endl;
        //writefile << 1 << "," << sprague_n[1] << sprague_s[1] << sprague_l[1] << sprague_u[1] << endl;
        //writefile << 2 << "," << sprague_n[2] << sprague_s[2] << sprague_l[2] << sprague_u[2] << endl;
		//writefile << 0 << "," << sprague_n[0] << "," << sprague_s[0] << "," << sprague_l[0] << "," << sprague_u[0] << endl;
		//writefile << 1 << "," << sprague_n[1] << "," << sprague_s[1] << "," << sprague_l[1] << "," << sprague_u[1] << endl;
		//writefile << 2 << "," << sprague_n[2] << "," << sprague_s[2] << "," << sprague_l[2] << "," << sprague_u[2] << endl;
	}
    else
        printf("File already existed.\n");

	// Calculate and write new values to file.
	for(unsigned long i = N+1; i <= MAXVALUE; i++)
	{
		//sprague_n
		for(unsigned long j = 0; j <= i-N; j++)
			used_values[(sprague_s[j] ^ sprague_s[i-j-N])] = true;
		sprague_n[i] = Mex(used_values, i);
		memset(used_values, 0, i);  //reset used_values array to empty

		//sprague_s
		for(unsigned long j = 0; j <= i - N; j++)
			used_values[(sprague_l[j] ^ sprague_s[i-j-N])] = true;
		for(unsigned long j = 1; j <= i - N; j++)
			used_values[(sprague_u[j] ^ sprague_s[i-j-N])] = true;
		sprague_s[i] = Mex(used_values, i);
		memset(used_values, 0, i);  //reset used_values array to empty

		//sprague_l
		for(unsigned long j = 0; j <= i-N; j++)
			used_values[(sprague_l[j] ^ sprague_l[i-j-N])] = true;
		for(unsigned long j = 1; j <= i-N-1; j++)
			used_values[(sprague_u[j] ^ sprague_u[i-j-N])] = true;
		sprague_l[i] = Mex(used_values, i);
		memset(used_values, 0, i);  //reset used_values array to empty

		//sprague_u
		for(unsigned long j = 0; j <= i-N-1; j++)
			used_values[(sprague_l[j] ^ sprague_u[i-j-N])] = true;
		sprague_u[i] = Mex(used_values, i);
		memset(used_values, 0, i);  //reset used_values array to empty

		// Send output to file for in the form N,n,s,l,u
		fprintf(writefile, "%lu,%lu,%lu,%lu,%lu\n", i, sprague_n[i], sprague_s[i], sprague_l[i], sprague_u[i] );
		// printf("%l,%l,%l,%l,%l\n", i, sprague_n[i], sprague_s[i], sprague_l[i], sprague_u[i] );
        //writefile << i << "," << sprague_n[i] << sprague_s[i] << sprague_l[i] << sprague_u[i] << endl;
		//writefile << i << "," << sprague_n[i] << "," << sprague_s[i] << "," << sprague_l[i] << "," << sprague_u[i] << endl;
		//flush file???
	}
	fclose(writefile);
	return 0;
}

unsigned long Mex(bool used_values[], unsigned long max)
//Minimum excluded value (i.e. Mex([0,1,2,4,7])=3 OR Mex([1,4])=0 )
{
	for(unsigned long i = 0; i < max; i++)
	{
		if(used_values[i] != true)
			return i;
	}
	return MAXVALUE + 1;
}
