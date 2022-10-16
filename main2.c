#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct in_data {
    const char *file_name;
};


struct all_inputs {
    int count;
    int rows;
    int cols;
    double *input;
};

struct all_data {
    struct in_data in_data;
    double *neuron;
    double *training_patterns;
};

double *gen_neuron(int size, double weight_min, double weight_max);

double *gen_training_patterns(int size, int count, double pattern_min, double pattern_max);

void print_double_arr(int len, double *arr);

void print_char_arr(int len, const char *arr[]);

double fRand(double fMin, double fMax);

typedef double (*activation_function)(double);

double neuron_output(int size, double *weights, double *inputs, activation_function f);

double lineral_func(double in);

void train_all_epoch(int size, int count, int epoch_count, double *neuron, double *inputs, double training_step,
                     activation_function f);

double get_average_result(double *inputs, double *neuron, int count, int size, activation_function func);

double get_average_expected(double *inputs, int count, int size);

struct all_inputs parse_file(char *file);
void normalize_input(struct all_inputs allInputs);

char *read_file(char *file_name);

int main(int argc, char const *argv[]) {
    srand(5);
    if (argc < 2) {
        return -argc;
    }

    struct in_data in_data = {
            .file_name = argv[1],
    };

    struct all_inputs allInputs = parse_file(read_file(in_data.file_name));

    normalize_input(allInputs);

    return 0;
}

void normalize_input(struct all_inputs allInputs) {
    int i,j;
    for (i = 0; i < allInputs.count; i++) {
        double count = 0;
        for (j = 0; j <allInputs.cols * allInputs.cols; j++) {
            count += allInputs.input[i * allInputs.cols * allInputs.rows + j];
        }
        count = sqrt(count);
        for (j = 0; j <allInputs.cols * allInputs.cols; j++) {
            allInputs.input[i * allInputs.cols * allInputs.rows + j] = allInputs.input[i * allInputs.cols * allInputs.rows + j]/count;
        }
    }
}

char *read_file(char *file_name) {
    FILE *fp;
    long lSize;
    char *buffer;

    fp = fopen(file_name, "rb");
    if (!fp) {
        perror(file_name);
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    lSize = ftell(fp);
    rewind(fp);

    buffer = calloc(1, lSize + 1);
    if (!buffer) {
        fclose(fp);
        fputs("memory alloc fails", stderr);
        exit(1);
    }
    if (1 != fread(buffer, lSize, 1, fp))
        fclose(fp);
    return buffer;
}

struct all_inputs parse_file(char *file) {
    struct all_inputs allInputs;
    char *c = &file[0];
    int newline = '\n';
    int metadata_counter = 0;
    int figure_offset = 0;
    while (c != NULL && (*c != '\0')) {
        if (*c == ';') {
            c = strchr(c, newline);
        } else if (*c == ' ' || *c == '\n' || *c == '\t' || *c == '\r') {
        } else if (metadata_counter == 0) {
            allInputs.count = *c - '0';
            metadata_counter++;
        } else if (metadata_counter == 1) {
            allInputs.cols = *c - '0';
            metadata_counter++;
        } else if (metadata_counter == 2) {
            allInputs.rows = *c - '0';
            allInputs.input = malloc(sizeof(short) * allInputs.count * allInputs.rows * allInputs.cols);
            metadata_counter++;
        } else {
            if (*c == '#') {
                allInputs.input[figure_offset] = 1;
                figure_offset++;
            }
            if (*c == '-') {
                allInputs.input[figure_offset] = 0;
                figure_offset++;
            }
        }
        c = c + 1;
    }
    return allInputs;
}

void print_double_arr(int len, double *arr) {
    printf("len = %i\n", len);
    for (int i = 0; i < len; i++) {
        printf("%f ", arr[i]);
    }
    printf("\n");
}

void print_char_arr(int len, const char *arr[]) {
    printf("len = %i\n", len);
    for (int i = 0; i < len; i++) {
        printf("%s ", arr[i]);
    }
    printf("\n");
}

double *gen_neuron(int size, double weight_min, double weight_max) {
    double *weights = malloc(sizeof(double[size]));
    for (int i = 0; i < size; i++) {
        weights[i] = fRand(weight_min, weight_max);
    }
    return weights;
}

// Oczekiwany wynik - ostatni element tabeli
double *gen_training_patterns(int size, int count, double pattern_min, double pattern_max) {
    size = size + 1;
    double *arr = malloc(sizeof(double *) * count * size);
    for (long long i = 0; i < count; i++) {
        for (int j = 0; j < size; j++) {
            arr[i * size + j] = fRand(pattern_min, pattern_max);
        }
    }
    return arr;
}

double fRand(double fMin, double fMax) {
    double f = (double) rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

double neuron_output(int size, double *weights, double *inputs, activation_function f) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += weights[i] * inputs[i];
    }
    return f(sum);
}

void change_weights(int size, double *neuron, double *inputs, double received, double training_step) {
    for (int i = 0; i < size; i++) {
        neuron[i] = neuron[i] + training_step * (inputs[size] - received) * inputs[i];
    }
}

void train_one_pattern(int size, double *neuron, double *inputs, double training_step, activation_function f) {
    double out = neuron_output(size, neuron, inputs, f);
    change_weights(size, neuron, inputs, out, training_step);
}

void train_all_patterns(int size, int count, double *neuron, double *inputs,
                        double training_step, activation_function f) {
    for (long i = 0; i < count; i++) {
        double *sub_arr = malloc((size + 1) * sizeof(double));
        for (int j = 0; j < size + 1; j++) {
            sub_arr[j] = inputs[i * size + j];
        }
        train_one_pattern(size, neuron, sub_arr, training_step, f);
    }
}

void train_all_epoch(int size, int count, int epoch_count, double *neuron, double *inputs, double training_step,
                     activation_function f) {
    for (int i = 0; i < epoch_count; i++) {
        train_all_patterns(size, count, neuron, inputs, training_step, f);
    }
}

double get_average_expected(double *inputs, int count, int size) {
    double sum = 0;
    for (long i = 0; i < count; i++) {
        sum += inputs[i * size + size] / count;
    }
    return sum;
}

double get_average_result(double *inputs, double *neuron, int count, int size, activation_function func) {
    double sum = 0;
    for (long i = 0; i < count; i++) {
        double *sub_arr = malloc((size + 1) * sizeof(double));
        for (int j = 0; j < size + 1; j++) {
            sub_arr[j] = inputs[i * size + j];
        }
        sum += neuron_output(size, neuron, sub_arr, func) / count;
    }
    return sum;
}

double lineral_func(double in) {
    return in;
}