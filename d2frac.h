#ifndef _D2FRAC_H_INCLUDED_
#define _D2FRAC_H_INCLUDED_

struct fraction_t {
	long int numerator;
	long int denominator;
};

/* try to make a good guess for a fraction that will result in d; return 1
 * on success on 0 on failure
 */
int d2frac(double d, struct fraction_t* frac, int prec_bits);
/* exactly the same, but call cb for each incremental fraction guess with the
 * updated fraction and arg; this includes the final guess.
 * If cb returns 0, stop guessing and immediately return 1.
 */
int d2frac_cb(double d, struct fraction_t* frac, int prec_bits,
		int (*cb)(struct fraction_t*, void*), void* arg);

#endif
