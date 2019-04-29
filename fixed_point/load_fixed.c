#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FIXED_LEN 16

// no problem: 2359296, 8
// problem: 
#define DATA_NUM 102760448

#define MAIN
// #define PRINT
//#define DEBUG
//#define SINGLE_SCAN

unsigned short str2fixed(char* input, int data_len) {
	/* scan a string of signed fixed point number of length 16,
	 * e.g. '0011000111100000' and store it using unsigned short */
	/* multiply chars by 10 ** 15 to 10 ** 0 */

	/* store 0 and 1 using, e.g. 16 bits to avoid overflow when doing multiplication */
	unsigned short us_input[data_len];
	for (int i = 0; i < data_len; i++) {
		/* convert the number from char to unsigned short */
		if (input[i] == '0') 
			us_input[i] = 0;
		else if (input[i] == '1')
			us_input[i] = 1;
		else
			return 0;
	}
	
	unsigned short output = 
		us_input[0] * 32768 + 
		us_input[1] * 16384 + 
		us_input[2] * 8192 + 
		us_input[3] * 4096 + 
		us_input[4] * 2048 + 
		us_input[5] * 1024 + 
		us_input[6] * 512 + 
		us_input[7] * 256 + 
		us_input[8] * 128 + 
		us_input[9] * 64 + 
		us_input[10] * 32 + 
		us_input[11] * 16 + 
		us_input[12] * 8 + 
		us_input[13] * 4 + 
		us_input[14] * 2 + 
		us_input[15] * 1;	

	return output;
}

void load_data(char const* fname, char* bin_char) {

    FILE *data_file;
    data_file = fopen(fname, "r");

    if (data_file == NULL) {
        printf("ERROR: cannot open file: %s\n", fname);
        return;
    }

    fscanf(data_file,"%s", bin_char);

    fclose(data_file);
}

#ifdef MAIN
int main() {
	clock_t start = clock();

	char* fname = "../weights_bin/weights_13.txt";
	char* bin_char = malloc(DATA_NUM * FIXED_LEN * sizeof(char));
	unsigned short fixed_point[DATA_NUM]; 
	
	#ifdef DEBUG
	printf("init:\n");
	for (int i = 0; i < DATA_NUM; i++)
		printf("%s\n", bin_char[i]);
	#endif
	load_data(fname, bin_char);
	
	for (int i = 0; i < DATA_NUM; i ++)
		{
			char substr[17];
			strncpy(substr, &bin_char[i * FIXED_LEN], FIXED_LEN);
			substr[16] = 0;
			
			#ifdef DEBUG
			for (int j = 0; j < FIXED_LEN; j++) {
				printf("%c ", bin_char[i]);
			}
			printf("\n");
			#endif

			fixed_point[i] = str2fixed(&bin_char[i * FIXED_LEN], FIXED_LEN);

			#ifdef PRINT
			printf("number %d:\n", i);
			printf("%s\n", substr);
			printf("unsigned short: %u\n", fixed_point[i]);
			#endif
		}
		
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("time consumed:\t%f", seconds);
}
#endif 

#ifdef SINGLE_SCAN
void load_data(char const* fname, char* bin_char, int numbers) {

    FILE *data_file;
    data_file = fopen(fname, "r");

    if (data_file == NULL) {
        printf("ERROR: cannot open file: %s\n", fname);
        exit(1);
    }

    fscanf(data_file,"%s", bin_char);

    fclose(data_file);
}


int main() {
	char* fname = "bin_sample.txt";
	char bin_char[FIXED_LEN];
	
	printf("init: %s", bin_char);
	load_data(fname, bin_char, 2);
	
	printf("whole str: %s\none by one:", bin_char);
	for (int j = 0; j < FIXED_LEN; j++) {
		printf("%c ", bin_char[j]);
	}
}
#endif
