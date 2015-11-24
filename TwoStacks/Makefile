test: TwoStack.c Stack.c Stack.h
	gcc -o test TwoStack.c Stack.c Stack.h -Wall

ut: Stack.c Stack.h
	@gcc -o ut_test Stack.c Stack.h -Wall -D__UT__
	@echo "Running Unit Test:"
	./ut_test
.PHONY: clean
clean: 
	rm -rf test
	rm -rf ut_test
