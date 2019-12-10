#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

const __uint16_t EXPECTED_RESULT = 777;
const __uint64_t mod             = 9999999998987;
const __uint8_t  root            = 5;

__uint64_t lpow(__uint64_t uroot, __uint64_t exp);
void flog(FILE* stream, const char* format, ...);

int main() {
    __uint64_t result  = 0;
    __uint64_t current = 0;

    flog(stdout, "Start met brute-force\n");

    while (result != EXPECTED_RESULT) {
        result = lpow(root, current) % mod;
        current++;
    }

    flog(stdout, "Key gevonden: %lu\n", current);
    return 0;
}

__uint64_t lpow(__uint64_t uroot, __uint64_t exp) {
    int result = 1;

    while (exp) {
        result *= uroot;
        exp--;
    }

    return result;
}

void flog(FILE* file, const char* format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(file, "[%lu] ", (__uint64_t) time(NULL));
    vfprintf(file, format, args);
    fprintf(file, "\n");

    va_end(args);
}