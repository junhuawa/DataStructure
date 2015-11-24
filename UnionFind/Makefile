test: main.c 
	gcc -o test main.c -Wall 
	./test < input.txt

ut: UnionFind.c
	gcc -o unittest UnionFind.c -Wall -D__UT__
	./unittest

.PHONY:clean
clean:
	rm -rf test
