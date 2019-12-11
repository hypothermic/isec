#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <omp.h>
#include <gmp.h>
#include <malloc.h>

//#define DEBUG

const __uint32_t EXPECTED_RESULT = 777;
const __uint64_t MOD             = 9999999998987;
const __uint8_t  BASE            = 5;
const __uint32_t STEP            = 200000;

__uint64_t lpow(__uint64_t uroot, __uint64_t exp);
static void _logf(FILE* dest, const char* format, ...);

int main(int argc, char* argv[]) {
    __uint8_t           lock          = 0,
                        local_lock    = 0;
    __uint64_t          current       = 6200008,
                        local_current = 0;
    __uint64_t          last_step     = time(NULL);
    const time_t        start_time    = last_step;

    static mpz_t        result,
                        intermediate;
    static const mpz_t  expected;
    #pragma omp threadprivate(result, intermediate)

    mpz_init(result);
    mpz_init(intermediate);
    mpz_init(expected);

    mpz_set_d(expected, EXPECTED_RESULT);

    _logf(stdout, "Start met brute-force\n");

    #pragma omp parallel /*num_threads(8)*/ private (local_current, local_lock)
    {
        mpz_init(result);
        mpz_init(intermediate);

        while (!local_lock) {

            #pragma omp atomic capture
            {
                local_current = current;
                current++;
            }

            #pragma omp atomic read
            local_lock = lock;

            mpz_ui_pow_ui(intermediate, BASE, local_current);
            mpz_fdiv_r_ui(result, intermediate, MOD);

            #ifdef DEBUG
                sleep(1);

                #pragma omp critical
                _logf(stderr, "Thread %d op count %lu (calc %Zd, %Zd)\n", omp_get_thread_num(), local_current, result, intermediate);
            #endif

            if (local_current % STEP == 0) {
                #pragma omp critical
                {
                    _logf(stdout, "%d combinaties geprobeerd in de laatste %lu seconden (%d totaal)",
                          STEP, time(NULL) - last_step, current);
                    last_step = time(NULL);
                }
            }

            if (mpz_cmp(result, expected) == 0) {
                #pragma omp atomic update
                lock++;

                #pragma omp critical
                _logf(stdout, "Key gevonden na %d seconden: %lu\n", start_time - last_step, local_current);
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