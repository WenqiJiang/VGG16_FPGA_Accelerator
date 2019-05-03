#include <stdio.h>

void load_data(char const* fname, float* array, int length) {

    FILE *data_file;
    data_file = fopen(fname, "r");

    if (data_file == NULL) {
        printf("ERROR: cannot open file: %s\n", fname);
    }

    for(int i = 0; i < length; i++)
    {
        fscanf(data_file,"%40f", &array[i]);
    }

    fclose(data_file);
}