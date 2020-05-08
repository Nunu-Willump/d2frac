# makefile is just here for convenience; I actually didn't ever properly learn to
# write them so this might recompile things unnecessarily a lot (probably not
# a problem with this code size tho)

release:
	gcc -s -no-pie -O3 -Wall -Wextra -Wpedantic -std=c89 d2frac.c -c -o d2frac.o
	ar rcs libd2frac.a d2frac.o

debug:
	gcc -Wall -Wextra -Wpedantic -Werror -std=c89 d2frac.c -c -o d2frac.o
	ar rcs libd2frac.a d2frac.o

recover-fration: release
	gcc -s -no-pie -O3 -Wall -Wextra -Wpedantic -std=c89 recover-fraction.c -L./ -ld2frac -o recover-fraction

recover-fration-debug: debug
	gcc -Wall -Wextra -Wpedantic -Werror -std=c89 recover-fraction.c -L./ -ld2frac -o recover-fraction

test:
	gcc -Wall -Wextra -Wpedantic -Werror -std=gnu99 test.c -L./ -ld2frac -o test

clean:
	rm -f d2frac.o libd2frac.a recover-fraction test

