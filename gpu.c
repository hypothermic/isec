// CUDA en GCC met NVPTX support

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <omp.h>
#include <gmp.h>
#include <malloc.h>
#include <stdint.h>

//#define DEBUG

const __uint32_t EXPECTED_RESULT = 777;
const __uint64_t MOD             = 9999999998987;
const __uint8_t  BASE            = 5;
const __uint32_t STEP            = 200000;
const __uint32_t CHUNK_SIZE      = 128;
const __uint64_t START_POW       = 6200008;
const __uint64_t MAX_POW         = UINT64_MAX-1; // openmp accepteert max niet.

__uint64_t lpow(__uint64_t uroot, __uint64_t exp);
static void _logf(FILE* dest, const char* format, ...);

int main(int argc, char* argv[]) {
    static mpz_t        result,
                        intermediate,
                        expected;

    mpz_init(result);
    mpz_init(intermediate);
    mpz_init(expected);

    mpz_init(result);
    mpz_init(intermediate);

    mpz_set_d(expected, EXPECTED_RESULT);

    _logf(stdout, "Start met brute-force\n");

    #pragma omp target teams distribute parallel for schedule(dynamic, CHUNK_SIZE) firstprivate(result, intermediate)
    for (uint_fast64_t pow = START_POW; pow <= MAX_POW; pow++) {

        mpz_ui_pow_ui(intermediate, BASE, pow);
        mpz_fdiv_r_ui(result, intermediate, MOD);

        #ifdef DEBUG
            sleep(1);

            #pragma omp critical
            _logf(stderr, "Thread %d op count %lu (calc %Zd, exp %Zd)\n", omp_get_thread_num(), pow, result, expected);
        #endif

        if (mpz_cmp(result, expected) == 0) {
            #pragma omp critical
            {
                _logf(stdout, "Key gevonden: %lu\n", pow);
                _exit(420);
            }
        }
    }

    _logf(stdout, "Alle threads zijn gestopt.\n");
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
 * Log naar een stream met een mooi tijdstip er voor.<br />
 * <br />
 * Support voor GMP format is aanwezig maar waarden met deze extensies worden niet gecheckt.
 */
static void _logf(FILE* dest, const char* format, ...) {
    va_list args;
    time_t  ltime;
    char*   lstr;
    size_t  lstrlen;

    va_start(args, format);

    ltime = time(NULL);
    lstr = asctime(localtime(&ltime));
    lstrlen = strlen(lstr);
    if (lstrlen > 0 && lstr[lstrlen-1] == '\n') {
        lstr[--lstrlen] = '\0';
    }

    fprintf(dest, "[%s] ", lstr);

    gmp_vfprintf(dest, format, args);
    fprintf(dest, "\n");

    va_end(args);
}