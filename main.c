#include <stdio.h>
#include <stdlib.h>

struct in_data {
    int N;
    int M;
    int epoch_count;
    double training_step;
    double weight_min;
    double weight_max;
    double training_in_min;
    double training_in_max;
};

struct all_data_train {
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

int main(int argc, char const *argv[]) {
    srand(5);
    if (argc < 9) {
        return -argc;
    }

    struct in_data in_data = {
            .N = strtol(argv[1], NULL, 0),
            .M = strtol(argv[2], NULL, 0),
            .epoch_count = strtol(argv[3], NULL, 0),
            .training_step = strtod(argv[4], NULL),
            .weight_min = strtod(argv[5], NULL),
            .weight_max = strtod(argv[6], NULL),
            .training_in_min = strtod(argv[7], NULL),
            .training_in_max = strtod(argv[8], NULL)
    };

    struct all_data_train all_data = {
            .in_data = in_data,
            .neuron = gen_neuron(in_data.N, in_data.weight_min, in_data.weight_max),
            .training_patterns = gen_training_patterns(in_data.N, in_data.M, in_data.training_in_min,
                                                       in_data.training_in_max)};

    printf("out: %f, expected: %f\n",
           get_average_result(all_data.training_patterns, all_data.neuron, in_data.M, in_data.N, lineral_func),
           get_average_expected(all_data.training_patterns, in_data.M, in_data.N));

    train_all_epoch(in_data.N, in_data.M, in_data.epoch_count, all_data.neuron, all_data.training_patterns,
                    in_data.training_step, &lineral_func);

    printf("out: %f, expected: %f\n",
           get_average_result(all_data.training_patterns, all_data.neuron, in_data.M, in_data.N, lineral_func),
           get_average_expected(all_data.training_patterns, in_data.M, in_data.N));


    return 0;
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