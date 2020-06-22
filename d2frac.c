#include "d2frac.h"

#include <stdint.h>
#include <float.h> /* for FLT_MAX */

/* used to get the bits of a double in compare_nprec;
 * pointer aliasing would be possible as well
 */
union u64_double {
	double d;
	uint64_t u;
};

/* compare the sign, exponent and up to prec_bits bits of the mantissa
 * (the implied first bit of the mantissa is excluded in prec_bits)
 *
 * prec_bits must be between 0 and 52, otherwise there might be weird results and
 * undefined behaviour.
 *
 * return value: positive if d1 > d2, negative if d1 < d2, and 0 if d1 == d2
 */
int compare_nprec(double d1, double d2, int prec_bits) {
	/* get two uint64_t's with the bits of the doubles. because both the ints and
	 * doubles are endian-dependent, the whole operation is actually endian-independent
	 */
	union u64_double d1_union, d2_union;
	d1_union.d = d1;
	d2_union.d = d2;

	/* an ieee 754 double consists of:
	 * - 1 sign bit
	 * - 11 exponent bits, which are (exp + 1023)
	 * - 52 mantissa bits, which are the mantissa except the actual mantissa
	 *       would have bit 53 set (it is implied as 1)
	 *
	 * thus, comparison is done between the numbers without the last
	 * (52 - prec_bits) bits
	 */

	return (d1_union.u - d2_union.u) >> (52 - prec_bits);
}

/*
 * unroll continued fraction
 */
void cfrac_to_frac(int* elems, int len, struct fraction_t* frac) {
	long unsigned int num = 1, den = 0;

	int i;
	for (i = len - 1; i >= 0; i--) {
		long unsigned int tmp = num;
		num = den + elems[i] * num;
		den = tmp;
	}

	frac->numerator = num;
	frac->denominator = den;
}

/* try to convert d into a fraction, with at least prec_bits of the mantissa
 * of d being equal to the mantissa of num/denom; TODO algorithm scaling
 *
 * returns 1 on success and 0 on failure (probably a bad value for d or prec_bits)
 */
int d2frac(double d, struct fraction_t* frac, int prec_bits) {
	/* with the golden ratio, which is the number with the longest contiunued
	 * fraction representation, floating point inaccuracies kick in after 38
	 * cf elements (tested in python)
	 */
	int cf[39];
	const int max_cf_len = sizeof(cf) / sizeof(*cf);
	int cf_len = 0;
	double d_left = d;

	if (prec_bits < 0 || prec_bits > 52) return 0;

	/* return early if d is NaN or +-inf */
	if (d != d) return 0;
	if (d > FLT_MAX || d < -FLT_MAX) return 0;

	/* recursively call d2frac with -d if d is negative, then fix resulting fraction */
	if (d < 0) {
		int ret = d2frac(-d, frac, prec_bits);
		if (ret == 1) {
			frac->numerator *= -1;
		}
		return ret;
	}

	for (; cf_len < max_cf_len; cf_len++) {
		cf[cf_len] = (int)d_left;
		cfrac_to_frac(cf, cf_len + 1, frac);
		if (compare_nprec(d, (double)(frac->numerator) / frac->denominator,
				prec_bits) == 0)
		{
			return 1;
		}
		d_left = 1 / (d_left - cf[cf_len]);
	}

	frac->numerator = 0;
	frac->denominator = 1;

	return 0;
}

/* same, except for the stuff mentioned in d2frac.h
 */
int d2frac_cb(double d, struct fraction_t* frac, int prec_bits,
		int (*cb)(struct fraction_t*, void*), void* arg)
{
	int cf[39];
	const int max_cf_len = sizeof(cf) / sizeof(*cf);
	int cf_len = 0;
	double d_left = d;
	int neg_factor = 1;

	if (prec_bits < 0 || prec_bits > 52) return 0;

	/* return early if d is NaN or +-inf */
	if (d != d) return 0;
	if (d > FLT_MAX || d < -FLT_MAX) return 0;

	/* since we need the correct (possible negative) fraction during the loop for
	 * the callback, we can't do the same trick as in d2frac(). Instead, so
	 * we don't need any additional slow if()s, multiply with neg_factor, which is
	 * 1 or -1 if d was negative or wasn't, respectively.
	 */
	if (d < 0) {
		neg_factor = -1;
		d *= -1;
		d_left = d;
	}

	for (; cf_len < max_cf_len; cf_len++) {
		cf[cf_len] = (int)d_left;
		cfrac_to_frac(cf, cf_len + 1, frac);

		frac->numerator *= neg_factor;

		if (cb(frac, arg) == 0) return 1;

		if (compare_nprec(d * neg_factor, (double)(frac->numerator) / frac->denominator,
				prec_bits) == 0)
		{
			return 1;
		}
		d_left = 1 / (d_left - cf[cf_len]);
	}

	frac->numerator = 0;
	frac->denominator = 1;

	return 0;
}

