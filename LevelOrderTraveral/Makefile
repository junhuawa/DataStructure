test: main.c Tree.c Tree.h
	gcc -o test main.c Tree.c Tree.h Queue.c Queue.h -Wall

ut: utQueue Tree.c Tree.h
	@gcc -o ut_test Tree.c Tree.h Queue.c Queue.h -Wall -D__UT__
	@echo "Running Unit Test:"
	./ut_test

utQueue: Queue.c Queue.h
	@gcc -o ut_queue_test Queue.c Queue.h -Wall -D__UT_QUEUE__
	@echo "Running Unit Test:"
	./ut_queue_test

.PHONY: clean
clean: 
	rm -rf test
	rm -rf ut_test
	rm -rf ut_queue_test
