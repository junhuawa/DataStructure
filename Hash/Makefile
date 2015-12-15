test: main.c hash.c hash.h
	gcc -o test main.c hash.c hash.h -lm -Wall
	./test < ./input.txt

uTest: hash.c hash.h
	gcc -o uTest hash.c hash.h -lm -Wall -D__UT__
	./uTest

.PHONY: clean
clean:
	rm -rf test
	rm -rf uTest
