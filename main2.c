#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


typedef double (*activation_function)(double);

struct in_data {
    const char *file_name_train;
    const char *file_name_test;
    int epoch_count;
    double training_step;
};


struct all_inputs_train {
    int count;
    int rows;
    int cols;
    double *input;
    char *characters;
    char *unique_characters;
    int unique_characters_count;
};

struct all_inputs_test {
    int count;
    int rows;
    int cols;
    double *input;
};

struct all_data_train {
    double *neurons;
    double *training_patterns;
    char *expected;
    int training_pattern_len;
    int training_pattern_count;
    int neurons_count;
    char *neurons_chars;
};

struct all_data_test {
    double *neurons;
    double *test_patterns;
    int test_pattern_len;
    int test_pattern_count;
    int neurons_count;
    char *neurons_chars;
};

double *gen_neuron(int size, double weight_min, double weight_max);

double *gen_training_patterns(int size, int count, double pattern_min, double pattern_max);

//void print_double_arr(int len, double *arr);

//void print_char_arr(int len, const char *arr[]);

double fRand(double fMin, double fMax);


double neuron_output(int size, double *weights, double *inputs, activation_function f);

double lineral_func(double in);


double get_average_result(double *inputs, double *neuron, int count, int size, activation_function func);

double get_average_expected(double *inputs, int count, int size);

void normalize_vec(double *vec, int length);

struct all_inputs_test parse_file_test(char *file, struct all_inputs_train allInputsTrain);

struct all_inputs_train parse_file_train(char *file);

//void normalize_input(struct all_inputs_train allInputs);

//void normalize_input_test(struct all_inputs_test allInputs);

char *read_file(const char *file_name);

//void train_neurons(struct all_data_train allData, activation_function func);

void test_neurons(struct all_data_test allData, activation_function func);

int main(int argc, char const *argv[]) {
    srand(5);
    if (argc < 4) {
        return -argc;
    }

    struct in_data in_data = {
            .file_name_train = argv[1],
            .file_name_test = argv[2],
    };

    struct all_inputs_train allInputsTrain;
    struct all_inputs_test allInputsTest;
    char *str = read_file(in_data.file_name_train);

    allInputsTrain = parse_file_train(str);

    str = read_file(in_data.file_name_test);

    allInputsTest = parse_file_test(str, allInputsTrain);

    free(str);

    struct all_data_train allData = {
            .training_patterns = allInputsTrain.input,
            .neurons_count = allInputsTrain.unique_characters_count,
            .neurons_chars = allInputsTrain.unique_characters,
            .training_pattern_count = allInputsTrain.count,
            .training_pattern_len = allInputsTrain.cols * allInputsTrain.rows,
            .expected = allInputsTrain.characters,
            .neurons = gen_neuron(allInputsTrain.unique_characters_count * allInputsTrain.rows * allInputsTrain.cols, 0,
                                  1),
    };
    struct all_data_test allDataTest = {
            .neurons = allData.neurons,
            .neurons_chars = allData.neurons_chars,
            .neurons_count = allData.neurons_count,
            .test_patterns = allInputsTest.input,
            .test_pattern_count = allInputsTest.count,
            .test_pattern_len = allInputsTest.cols * allInputsTest.rows,
    };


//    test_neurons(allDataTest, lineral_func);
    /*
     * Trochę to oszukane, albowiem zakładam że treningowy plik ma kazda literke zdefiniowana raz i robie cyk przerzut, ale bedzie dzialac
     */
    for(int i = 0; i < allData.neurons_count * allData.training_pattern_len; i++) {
        allDataTest.neurons[i] = allData.training_patterns[i];
    }
    for (int i = 0; i < allData.neurons_count; i++) {
        normalize_vec(&allDataTest.neurons[i*allDataTest.test_pattern_len], allDataTest.test_pattern_len);
    }
    for (int i = 0; i < allDataTest.test_pattern_count; i++) {
        normalize_vec(&allDataTest.test_patterns[i*allDataTest.test_pattern_len], allDataTest.test_pattern_len);
    }
    printf("\n");

//    train_neurons(allData, lineral_func);


    test_neurons(allDataTest, lineral_func);

    return 0;
}

void test_neurons(struct all_data_test allData, activation_function func) {
    for (int j = 0; j < allData.test_pattern_count; j++) {
        for (int k = 0; k < allData.neurons_count; k++) {
            double sum = 0;
            for (int l = 0; l < allData.test_pattern_len; l++) {
                sum += allData.test_patterns[j * allData.test_pattern_len + l] *
                       allData.neurons[k * allData.test_pattern_len + l];
            }
            sum = func(sum);
            printf("Neuron %d with char %c gave result %f for test pattern %d\n", k, allData.neurons_chars[k], sum, j);
        }
    }
}

//void train_neurons(struct all_data_train allData, activation_function func) {
//    for (int i = 0; i < allData.epoch_count; i++) {
//        for (int j = 0; j < allData.training_pattern_count; j++) {
//            for (int k = 0; k < allData.neurons_count; k++) {
//                double expected = allData.expected[j] == allData.neurons_chars[k];
//                double sum = 0;
//                for (int l = 0; l < allData.training_pattern_len; l++) {
//                    sum += allData.training_patterns[j * allData.training_pattern_len + l] *
//                           allData.neurons[k * allData.training_pattern_len + l];
//                }
//                sum = func(sum);
//                for (int l = 0; l < allData.training_pattern_len; l++) {
//                    allData.neurons[k * allData.training_pattern_len + l] =
//                            allData.neurons[k * allData.training_pattern_len + l]
//                            + allData.training_step * (expected - sum) *
//                              allData.training_patterns[j * allData.training_pattern_len + l];
//                }
//            }
//        }
//    }
//}

//void normalize_input(struct all_inputs_train allInputs) {
//    for (int i = 0; i < allInputs.count; i++) {
//        normalize_vec(allInputs.input + allInputs.rows * allInputs.cols * i, allInputs.rows * allInputs.cols);
//    }
//}
//
//void normalize_input_test(struct all_inputs_test allInputs) {
//    for (int i = 0; i < allInputs.count; i++) {
//        normalize_vec(allInputs.input + allInputs.rows * allInputs.cols * i, allInputs.rows * allInputs.cols);
//    }
//}

void normalize_vec(double *vec, int length) {
    int j;
    double count = 0;
    for (j = 0; j < length; j++) {
        count += vec[j];
    }
    count = sqrt(count);
    for (j = 0; j < length; j++) {
        vec[j] = vec[j] / count;
    }
}

char *read_file(const char *file_name) {
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

struct all_inputs_train parse_file_train(char *file) {
    struct all_inputs_train allInputs;
    char *c = &file[0];
    int newline = '\n';
    int metadata_counter = 0;
    int figure_offset = 0;
    int char_offset = 0;
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
            allInputs.characters = malloc(sizeof(char) * (allInputs.count) + 1);
            allInputs.unique_characters = malloc(sizeof(char) * (allInputs.count) + 1);
            allInputs.characters[allInputs.count] = '\0';
            metadata_counter++;
        } else {
            if ((*c >= 'A' && (*c <= 'Z')) || (*c >= 'a' && (*c <= 'z'))) {
                if (strchr(allInputs.characters, *c) == NULL) {
                    allInputs.unique_characters[allInputs.unique_characters_count] = *c;
                    allInputs.unique_characters_count++;
                }
                allInputs.characters[char_offset] = *c;
                char_offset++;
            }
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

struct all_inputs_test parse_file_test(char *file, struct all_inputs_train allInputsTrain) {
    struct all_inputs_test allInputs;
    allInputs.rows = allInputsTrain.rows;
    allInputs.cols = allInputsTrain.cols;
    allInputs.input = malloc(sizeof(short) * allInputs.count * allInputs.rows * allInputs.cols);
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

double *gen_neuron(int size, double weight_min, double weight_max) {
    double *weights = malloc(sizeof(double[size]));
    for (int i = 0; i < size; i++) {
        weights[i] = fRand(weight_min, weight_max);
    }
    return weights;
}


double fRand(double fMin, double fMax) {
    double f = (double) rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


double lineral_func(double in) {
    return in;
}