#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CSV_PATH "data/data.csv"
#define DEFAULT_SENSOR_COL 1

typedef struct {
    int rows;
    int cols;
    int capacity;
    double *values;
} CsvData;

static void free_csv(CsvData *data) {
    free(data->values);
    data->values = NULL;
    data->rows = 0;
    data->cols = 0;
    data->capacity = 0;
}

static void trim_in_place(char *s) {
    char *start = s;
    char *end;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }
    if (*start == '\0') {
        s[0] = '\0';
        return;
    }
    end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }
    *end = '\0';
    if (start != s) {
        memmove(s, start, (size_t)(end - start) + 1);
    }
}

static char *duplicate_string(const char *src) {
    size_t len = strlen(src) + 1;
    char *copy = (char *)malloc(len);
    if (copy) {
        memcpy(copy, src, len);
    }
    return copy;
}

static int ensure_capacity(CsvData *data, int required_rows) {
    int new_capacity = data->capacity > 0 ? data->capacity : 32;
    while (new_capacity < required_rows) {
        new_capacity *= 2;
    }
    if (new_capacity == data->capacity) {
        return 1;
    }
    double *new_values = (double *)realloc(data->values, (size_t)new_capacity * data->cols * sizeof(double));
    if (!new_values) {
        return 0;
    }
    data->values = new_values;
    data->capacity = new_capacity;
    return 1;
}

static int count_columns(const char *line) {
    int count = 1;
    const char *p = line;
    while (*p) {
        if (*p == ',') {
            count++;
        }
        p++;
    }
    return count;
}

static int parse_csv(const char *path, CsvData *data) {
    FILE *fp = fopen(path, "r");
    char line[4096];
    int header_cols = 0;
    int row = 0;

    if (!fp) {
        fprintf(stderr, "Unable to open CSV file: %s\n", path);
        return 0;
    }

    if (!fgets(line, sizeof(line), fp)) {
        fprintf(stderr, "CSV file is empty: %s\n", path);
        fclose(fp);
        return 0;
    }

    trim_in_place(line);
    header_cols = count_columns(line);
    data->cols = header_cols;
    data->rows = 0;
    data->capacity = 0;
    data->values = NULL;

    while (fgets(line, sizeof(line), fp)) {
        char *cursor;
        int col = 0;
        char *copy = duplicate_string(line);
        if (!copy) {
            fclose(fp);
            free_csv(data);
            return 0;
        }
        trim_in_place(copy);
        if (copy[0] == '\0') {
            free(copy);
            continue;
        }

        if (!ensure_capacity(data, data->rows + 1)) {
            fprintf(stderr, "Out of memory while loading CSV.\n");
            free(copy);
            fclose(fp);
            free_csv(data);
            return 0;
        }

        cursor = strtok(copy, ",");
        while (cursor != NULL) {
            char *end = NULL;
            double value;
            trim_in_place(cursor);
            if (col < data->cols) {
                value = strtod(cursor, &end);
                if (end != cursor) {
                    data->values[data->rows * data->cols + col] = value;
                } else {
                    data->values[data->rows * data->cols + col] = NAN;
                }
            }
            cursor = strtok(NULL, ",");
            col++;
        }

        while (col < data->cols) {
            data->values[data->rows * data->cols + col] = NAN;
            col++;
        }

        row++;
        data->rows++;
        free(copy);
    }

    fclose(fp);
    return 1;
}

static double compute_mean(const CsvData *data, int column) {
    double sum = 0.0;
    int count = 0;
    for (int i = 0; i < data->rows; ++i) {
        double value = data->values[i * data->cols + column];
        if (!isnan(value)) {
            sum += value;
            count++;
        }
    }
    return count > 0 ? sum / count : NAN;
}

static double compute_stddev(const CsvData *data, int column, double mean) {
    double sum_sq = 0.0;
    int count = 0;
    for (int i = 0; i < data->rows; ++i) {
        double value = data->values[i * data->cols + column];
        if (!isnan(value)) {
            double diff = value - mean;
            sum_sq += diff * diff;
            count++;
        }
    }
    return count > 1 ? sqrt(sum_sq / count) : 0.0;
}

static int count_anomalies(const CsvData *data, int column, double mean, double stddev) {
    int anomalies = 0;
    double threshold = stddev > 0.0 ? 3.0 * stddev : 0.0;
    for (int i = 0; i < data->rows; ++i) {
        double value = data->values[i * data->cols + column];
        if (!isnan(value)) {
            if (fabs(value - mean) > threshold) {
                anomalies++;
            }
        }
    }
    return anomalies;
}

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : DEFAULT_CSV_PATH;
    CsvData data;
    int sensor_col = DEFAULT_SENSOR_COL;
    double mean;
    double stddev;
    double min_val = 0.0;
    double max_val = 0.0;
    int has_values = 0;

    memset(&data, 0, sizeof(data));
    if (!parse_csv(path, &data)) {
        return 1;
    }

    if (data.rows <= 0 || data.cols <= 1) {
        fprintf(stderr, "No usable sensor rows found in %s\n", path);
        free_csv(&data);
        return 1;
    }

    if (argc > 2) {
        sensor_col = atoi(argv[2]);
    }
    if (sensor_col < 1 || sensor_col >= data.cols) {
        sensor_col = DEFAULT_SENSOR_COL;
    }

    for (int i = 0; i < data.rows; ++i) {
        double value = data.values[i * data.cols + sensor_col];
        if (!isnan(value)) {
            if (!has_values) {
                min_val = max_val = value;
                has_values = 1;
            } else {
                if (value < min_val) {
                    min_val = value;
                }
                if (value > max_val) {
                    max_val = value;
                }
            }
        }
    }

    if (!has_values) {
        fprintf(stderr, "No numeric values found for column %d\n", sensor_col);
        free_csv(&data);
        return 1;
    }

    mean = compute_mean(&data, sensor_col);
    stddev = compute_stddev(&data, sensor_col, mean);

    printf("Industrial monitoring summary\n");
    printf("Source file: %s\n", path);
    printf("Rows: %d\n", data.rows);
    printf("Sensor column: %d\n", sensor_col);
    printf("Mean: %.4f\n", mean);
    printf("Stddev: %.4f\n", stddev);
    printf("Min: %.4f\n", min_val);
    printf("Max: %.4f\n", max_val);
    printf("Anomalies (|x - mean| > 3*stddev): %d\n", count_anomalies(&data, sensor_col, mean, stddev));

    free_csv(&data);
    return 0;
}
