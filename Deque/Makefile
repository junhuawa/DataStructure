test: Main.c Deque.c Deque.h
	gcc -o test Main.c Deque.c Deque.h -Wall

ut: Deque.c Deque.h
	@gcc -o ut_test Deque.c Deque.h -Wall -D__UT__
	@echo "Running Unit Test:"
	./ut_test
.PHONY: clean
clean: 
	rm -rf test
	rm -rf ut_test
