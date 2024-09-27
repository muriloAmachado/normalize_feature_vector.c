#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <time.h>

//#ifndef _GNU_SOURCE
#define _GNU_SOURCE
//#endif

// Array definition
int LINES = 1000;
int ARRAY_SIZE = 1000;

float Q_rsqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = *(long *) &y;                          
    i  = 0x5f375a86 - (i >> 1);                 //0x5f3759df -> Constante que foi utilizada no código fonte do Quake 3           
    y  = *(float *) &i;                         
    y  = y * (threehalfs - (x2 * y * y));      

    return y;
}

// Naive function to normalize a feature vector
void normalize_feature_vector(float* features, int length) {
    float sum = 0.0f;
    for (int i = 0; i < length; i++) {
        sum += features[i] * features[i];
    }
    float inv_sqrt = Q_rsqrt(sum);

    for (int i = 0; i < length; i++) {
        features[i] *= inv_sqrt;
    }
}

// Function to generate random float values to write to a CSV file
float generate_random_value(float min, float max) {
    return min + (float)rand() / (float)(RAND_MAX / (max - min));
}

// Write random values to a CSV file
void write_csv() {
    FILE *file = fopen("data.csv", "w");
    if (file == NULL) {
        printf("File opening error!\n");
        return;
    }

    srand(time(NULL));

    // Generating and writing arrays in CSV
    for (int i = 0; i < LINES; i++) {
        for (int j = 0; j < ARRAY_SIZE; j++) {
            float value = generate_random_value(0.1f, 10.0f);  // Values between 0.1 and 10.0
            fprintf(file, "%.4f", value);  // Write with 4 decimal places
            if (j < ARRAY_SIZE - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Function to read data from a CSV file
float** read_csv(const char* filename, int* num_elements, int* num_dimensions) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Determine the number of elements and dimensions
    *num_elements = 0;
    *num_dimensions = 0;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (*num_elements == 0) {
            char* token = strtok(line, ",");
            while (token) {
                (*num_dimensions)++;
                token = strtok(NULL, ",");
            }
        }
        (*num_elements)++;
    }
    rewind(file);

    // Allocate memory for the features
    float** features = (float**)malloc(*num_elements * sizeof(float*));
    for (int i = 0; i < *num_elements; i++) {
        features[i] = (float*)malloc(*num_dimensions * sizeof(float));
    }

    // Read the data
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        int j = 0;
        char* token = strtok(line, ",");
        while (token) {
            features[i][j++] = atof(token);
            token = strtok(NULL, ",");
        }
        i++;
    }

    fclose(file);
    return features;
}

// Function to measure execution time using the 'resources' library
void get_resource_usage(struct rusage* usage) {
    getrusage(RUSAGE_SELF, usage);
}

void print_resource_usage(const char* label, struct rusage* usage) {
    printf("%s\n", label);
    printf("User time: %ld.%06ld seconds\n", usage->ru_utime.tv_sec, usage->ru_utime.tv_usec);
    printf("System time: %ld.%06ld seconds\n", usage->ru_stime.tv_sec, usage->ru_stime.tv_usec);
    printf("Maximum resident set size: %ld kilobytes\n", usage->ru_maxrss);
}

int main() {

    write_csv();

    int num_elements, num_dimensions;
    float** features = read_csv("data.csv", &num_elements, &num_dimensions);

    struct rusage start_usage, end_usage;
    
    get_resource_usage(&start_usage);
    for (int i = 0; i < num_elements; i++) {
        normalize_feature_vector(features[i], num_dimensions);
    }
    get_resource_usage(&end_usage);

    printf("Normalized features:\n");
    for (int i = 0; i < num_elements; i++) {
        for (int j = 0; j < num_dimensions; j++) {
            printf("%f ", features[i][j]);
        }
        printf("\n");
    }

    printf("Execution time and resource usage:\n");
    print_resource_usage("Start Usage", &start_usage);
    print_resource_usage("End Usage", &end_usage);

    // Free allocated memory
    for (int i = 0; i < num_elements; i++) {
        free(features[i]);
    }
    free(features);

    return 0;
}
