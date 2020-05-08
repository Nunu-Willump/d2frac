#include <stdio.h>
#include <stdlib.h>

#include "d2frac.h"

int output_guess(struct fraction_t* frac, void* last_denom_vp) {
	long int* last_denom = (long int*)last_denom_vp;

	if (*last_denom != 0 && *last_denom * 50 < frac->denominator) {
		puts("^ You probably want this one :)");
		*last_denom = 0;
	}
	else if (*last_denom != 0) *last_denom = frac->denominator;

	printf("%ld/%ld\n", frac->numerator, frac->denominator);

	return 1;
}

int main(int argc, char** argv) {
	struct fraction_t frac;
	double d = argc >= 2? atof(argv[1]) : 3.141592653589793238462643383279502884;
	int ret;
	long int last_denom = 1;

	puts("Here are some different guesses:");

	ret = d2frac_cb(d, &frac, 30, output_guess, (void*)&last_denom);
	if (!ret) {
		fprintf(stderr, "Couldn't convert %f to a fraction\n", d);
		return 1;
	}

	if (last_denom != 0) {
		puts("^ You probably want this one :)");
	}
	return 0;
}

