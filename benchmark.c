/*-
 * Copyright 2013-2018 Alexander Peslyak
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h> /* for atoi() */
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sched.h>

#include "yespower.h"

int main(int argc, const char * const *argv)
{
    yespower_params_t params = {
        .N = 2048,
        .r = 32,
        .pers = (const uint8_t *)"Client Key",
        .perslen = 10
    };

    if (argc > 1)
        params.version = atoi(argv[1]);
    if (argc > 2)
        params.N = atoi(argv[2]);
    if (argc > 3)
        params.r = atoi(argv[3]);

    printf("version=%.1f N=%u r=%u\n",
        params.version * 0.1, params.N, params.r);

    printf("Will use %.2f KiB RAM\n", 0.125 * params.N * params.r);

    static __thread union {
        uint8_t u8[80];
        uint32_t u32[20];
    } src;
    yespower_binary_t dst;
    unsigned int i;

    for (i = 0; i < sizeof(src); i++)
        src.u8[i] = i * 3;

    if (yespower_tls(src.u8, sizeof(src), &params, &dst)) {
        puts("FAILED");
        return 1;
    }

    for (i = 0; i < sizeof(dst); i++)
        printf("%02x%c", dst.uc[i], i < sizeof(dst) - 1 ? ' ' : '\n');

    puts("Benchmarking 1 thread ...");

    clock_t clk_tck = sysconf(_SC_CLK_TCK);
    struct tms start_tms, end_tms;
    clock_t start = times(&start_tms), end;
    unsigned int n;
    unsigned long long count;
    uint32_t seed = start * 1812433253U;

    n = 1;
    count = 0;
    do {
        for (i = 0; i < n; i++) {
            yespower_binary_t *p = &dst;
            src.u32[19] = seed + (count + i);
            if (yespower_tls(src.u8, sizeof(src), &params, p)) {
                puts("FAILED");
                return 1;
            }
        }
        count += n;

        end = times(&end_tms);
        n <<= 1;
    } while (end - start < clk_tck * 2);

    clock_t start_v = start_tms.tms_utime + start_tms.tms_stime +
        start_tms.tms_cutime + start_tms.tms_cstime;
    clock_t end_v = end_tms.tms_utime + end_tms.tms_stime +
        end_tms.tms_cutime + end_tms.tms_cstime;

    printf("%llu H/s real, %llu H/s virtual "
        "(%llu hashes in %.2f seconds)\n",
        count * clk_tck / (end - start),
        count * clk_tck / (end_v - start_v),
        count, (double)(end - start) / clk_tck);

    return 0;
}
