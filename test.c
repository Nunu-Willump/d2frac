#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <sys/random.h>

#include "d2frac.h"

// stolen from https://lemire.me/blog/2013/12/26/fastest-way-to-compute-the-greatest-common-divisor/
unsigned int gcd(unsigned int u, unsigned int v) {
    int shift;
    if (u == 0) return v;
    if (v == 0) return u;
    shift = __builtin_ctz(u | v);
    u >>= __builtin_ctz(u);
    do {
        v >>= __builtin_ctz(v);
        if (u > v) {
            unsigned int t = v;
            v = u;
            u = t;
        }  
        v = v - u;
    } while (v != 0);
    return u << shift;
}

int main(int argc, char** argv) {
	struct timespec current_time;
	long long unsigned int start_usec;
	long long unsigned int end_usec;

	int ret;

	int num_tests;
	if (argc < 2 || (num_tests = atoi(argv[1])) == 0) {
		num_tests = 512;
	}

	uint16_t random_buf[num_tests * 2];
	int random_buf_size = num_tests * 2 * 2;

	ret = clock_gettime(CLOCK_REALTIME, &current_time);
	if (ret == 0) {
		start_usec = current_time.tv_sec * 1000000 + current_time.tv_nsec / 1000;
	}
	else {
		start_usec = 0;
	}

	ssize_t random_ret = getrandom(random_buf, random_buf_size, 0);
	if (random_ret < random_buf_size) {
		fprintf(stderr, "Couldn't read enough random bytes\n");
		return 1;
	}

	for (int i = 0; i < num_tests; i++) {
		struct fraction_t frac;
		unsigned short num = random_buf[i * 2];
		unsigned short den = random_buf[i * 2 + 1];
		unsigned int num_den_gcd = gcd(num, den);
		num /= num_den_gcd;
		den /= num_den_gcd;
		double d = num / (double)den;

		ret = d2frac(d, &frac, 51);
		if (!ret) {
			printf("Couldn't convert %f to the fraction %hd/%hd\n",
					d, num, den);
		}
		else if (frac.numerator != num || frac.denominator != den) {
			printf("Wrongly converted %f... to the fraction %ld/%ld"
					" instead of %hd/%hd\n", d, frac.numerator, frac.denominator,
					num, den);
		}
		else {
			printf("Correctly converted %f to %ld/%ld\n",
					d, frac.numerator, frac.denominator);
		}
	}

	ret = clock_gettime(CLOCK_REALTIME, &current_time);
	end_usec = current_time.tv_sec * 1000000 + current_time.tv_nsec / 1000;

	if (ret == 0 && start_usec != 0) {
		long long unsigned int diff_usec = end_usec - start_usec;

		printf("Went through %d tests in %llu.%06llu seconds\n",
				num_tests, diff_usec / 1000000, diff_usec % 1000000);
	}

	return 0;
}

