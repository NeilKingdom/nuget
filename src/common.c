#include "common.h"

char *dtoa(const double d) {
    int tmp = d;
    int i, digits = 0;
    char *a = NULL;

    if (d == 0) {
        return strdup("0");
    }

    while (tmp) {
        tmp /= 10;
        digits++;
    }

    a = malloc((digits + 1) * sizeof(char));
    if (a == NULL) {
        perror("Failed to allocate memory for output string");
        exit(EXIT_FAILURE);
    }

    tmp = d;
    for (i = digits; i > 0; --i) {
        a[i - 1] = (tmp % 10) + '0';
        tmp /= 10;
    }
    a[digits] = '\0';

    return a;
}

double atod(const char * restrict a) {
    char c;
    double d = 0.0;
    unsigned counter;

    const char *integer = strtok(strdup(a), ".");
    const char *fraction = strtok(NULL, ".");

    counter = strlen(integer) - 1;
    while ((c = *integer++) != '\0') {
        if (c == '.') {
            break;
        } else if (!isdigit(c)) {
            return NAN;
        }

        d += (double)(c - '0') * pow(10, counter--);
    }

    counter = 1;
    while ((c = *fraction++) != '\0') {
        if (!isdigit(c)) {
            return NAN;
        }

        d += ((double)(c - '0') / pow(10, counter++));
    }

    return d;
}
