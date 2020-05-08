Since guessing a fraction from a double is a task that might be needed by
programmers too lazy to learn the maths behind it, here's a quite optimized
implementation in c89 (which compiles without any problems in c99 and c11 as
well). Though, i'm not that good at math, so it might be possible to make an
even faster implementation if I can figure out how to better calculate the
simplified form of continued fractions.

If you want to include this library in your project, either copy-paste the code
(only d2frac.c and d2frac.h) or, if you're on linux with gcc, do a make release
and link with the resulting libd2frac.a. This library doesn't depend on
anything besides an environment which uses IEEE 754 64-bit floats for the c
type "double"; Though, I'll probably add support for arbitrary precision floats
later (using a library like mpfr) which would probably be fully platform
independant.

To use this in your code, just do something like the following:

    
    #include "d2frac.h"
    ...
    struct fraction_t frac;
    int precision_bits = 51; /* must be 0 < x < 52; otherwise there'll be UB */
    int ret = d2frac(3.14159, &frac, precision_bits);
    if (!ret) {
        /* fail */
    }
    else {
        /* do something with frac.numerator and frac.denominator */
    }

In case you want to see guesses besides the final one, theres a `d2frac_cb`
function which does the same as `d2frac` except it additionally takes a
parameter for a callback function and a void pointer to pass to the callback.
The callback will also be called with the final guess.

The callback function must have the form
`int function(struct fraction_t*, void*)`, where the two parameters are self
explanatory and the return value indicates whether to continue generating
better guesses. A return value of 0 will cause `d2frac_cb` to immediately
return the current guess. For a code example for using this function, see
recover-fraction.c. recover-fraction is a simple program which will print all
the guesses for the floating point value given as the first argument, or pi if
it is called without any arguments. It will also try to guess the correct
fraction if later guesses become not what the user intended because he rounded
the number to a few decimal places.

