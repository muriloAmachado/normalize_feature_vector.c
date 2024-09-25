#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <time.h>
#include <xmmintrin.h>

//#ifndef _GNU_SOURCE
#define _GNU_SOURCE
//#endif

// Array definition
int LINES = 1000;
int ARRAY_SIZE = 1000;

// Função naïve para normalizar um vetor de características
void normalize_feature_vector(float* features, int length) {
    float sum = 0.0f;
    
    for (int i = 0; i < length; i++) {
        sum += features[i] * features[i];
    }

    __m128 vec = _mm_load_ss(&sum);
    __m128 inv_sqrt_vec = _mm_rsqrt_ss(vec);

    float inv_sqrt;
    _mm_store_ss(&inv_sqrt, inv_sqrt_vec);

    for (int i = 0; i < length; i++) {
        features[i] *= inv_sqrt;
    }
}

//Função para gerar valores flotas aleatórios para gravar no arquivo csv 
float gerar_valor_aleatorio(float min, float max) {
    return min + (float)rand() / (float)(RAND_MAX / (max - min));
}

void write_csv(){
    FILE *arquivo = fopen("data.csv", "w");
    if (arquivo == NULL) {
        printf("File opening error!\n");
    }

    srand(time(NULL));

    // Generating and writing arrays in CSV
    for (int i = 0; i < LINES; i++) {
        for (int j = 0; j < ARRAY_SIZE; j++) {
            float valor = gerar_valor_aleatorio(0.1f, 10.0f);  // Valores entre 0.1 e 10.0
            fprintf(arquivo, "%.4f", valor);  // Escreve com 4 casas decimais
            if (j < ARRAY_SIZE - 1) {
                fprintf(arquivo, ",");
            }
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);
}

// Função para ler dados de um arquivo CSV
float** read_csv(const char* filename, int* num_elements, int* num_dimensions) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Determine the number of elements and dimensions
    // Essa parte do código serve apenas para definir a quantidade de linhas e colunas do arquivo CSV
    // token = index da primeira linha
    // line = conteúdo das linhas
    // strtok = função para ler a linha até determinado ponto -> strtok(até onde ele segue, caracter de referência para ler até o primeiro parâmetro informado)
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

// Função para medir o tempo de execução usando a biblioteca 'resources'
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