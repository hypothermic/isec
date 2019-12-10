#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <omp.h>
#include <gmp.h>

#define DEBUG 0

const __uint16_t EXPECTED_RESULT = 777;
const __uint64_t mod             = 9999999998987;
const __uint8_t  root            = 5;
const __uint32_t step            = 25000;

__uint64_t lpow(__uint64_t uroot, __uint64_t exp);
void flog(FILE* stream, const char* format, ...);

int main() {
    __uint8_t    lock          = 0,
                 local_lock    = 0;
    __uint64_t   current       = 0,
                 local_current = 0;
    __uint64_t   last_step     = time(NULL);

    static mpz_t result,
                 intermediate;
    #pragma omp threadprivate(result, intermediate)


    flog(stdout, "Start met brute-force\n");

    #pragma omp parallel private (local_current, local_lock)
    {
        mpz_init(result);
        mpz_init(intermediate);

        while (!local_lock) {

            #pragma omp critical
            {

                local_current = current;
                current++;

                local_lock = lock;
            }

            mpz_ui_pow_ui(intermediate, root, local_current);
            mpz_mod_ui(result, intermediate, mod);

            if (DEBUG) {
                sleep(1);

                #pragma omp critical
                gmp_fprintf(stdout, "Thread %d op count %lu (calc %Zd)\n", omp_get_thread_num(), local_current, result);
            }

            if (local_current % step == 0) {
                #pragma omp critical
                {
                    flog(stdout, "%d stappen voltooid in de laatste %lu seconden (%d totaal)", step,
                         time(NULL) - last_step,
                         current);
                    last_step = time(NULL);
                }
            }

            if (mpz_cmp_ui(result, EXPECTED_RESULT) == 0) {
                #pragma omp atomic update
                lock++;

                #pragma omp critical
                gmp_fprintf(stdout, "[%lu] Key gevonden: %Zd\n", time(NULL), result);
            }
        }
    }

    flog(stdout, "Alle threads zijn gestopt.\n");
    return 0;
}

/**
 * Oude exponent functie voordat we GMP gebruikten.
 *
 * @deprecated
 */
__uint64_t lpow(__uint64_t uroot, __uint64_t exp) {
    int result = 1;

    while (exp) {
        result *= uroot;
        exp--;
    }

    return result;
}

/**
 * Log naar een stream met een mooi tijdstip er voor
 */
void flog(FILE* file, const char* format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(file, "[%lu] ", (__uint64_t) time(NULL));
    vfprintf(file, format, args);
    fprintf(file, "\n");

    va_end(args);
}