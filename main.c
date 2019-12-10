#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <omp.h>

#define DEBUG 1

const __uint16_t EXPECTED_RESULT = 777;
const __uint64_t mod             = 9999999998987;
const __uint8_t  root            = 5;
const __uint32_t step            = 25000;

__uint64_t lpow(__uint64_t uroot, __uint64_t exp);
void flog(FILE* stream, const char* format, ...);

int main() {
    __uint64_t result        = 0;
    __uint64_t current       = 0,
               local_current = 0;
    __uint64_t last_step     = time(NULL);

    flog(stdout, "Start met brute-force\n");

    #pragma omp parallel private (local_current)
    while (result != EXPECTED_RESULT) {

        #pragma omp critical
        {
            local_current = current;
            current++;
        }
        //#pragma omp atomic read
        //local_current = current;
        //#pragma omp atomic update
        //current++;

        result = lpow(root, local_current) % mod;

        if (DEBUG) {
            sleep(1);
            flog(stdout, "Thread %d op count %lu (calc %lu)", omp_get_thread_num(), local_current, result);
        }

        if (local_current % step == 0) {
            flog(stdout, "%d stappen voltooid in de laatste %lu seconden (%d totaal)", step, time(NULL) - last_step,
                current);
            last_step = time(NULL);
        }
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