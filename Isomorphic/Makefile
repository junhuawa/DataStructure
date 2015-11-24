test: ExpressionTree.c Tree.c Tree.h Queue.c Queue.h
	gcc -o test ExpressionTree.c Tree.c Tree.h Queue.c Queue.h -Wall

mainTest: main.c 
	gcc -o mainTest main.c 

ut: utQueue Tree.c Tree.h
	@gcc -o utTest Tree.c Tree.h Queue.c Queue.h -Wall -D__UT__
	@echo "Running Unit Test:"
	./utTest

utQueue: Queue.c Queue.h
	@gcc -o utQueueTest Queue.c Queue.h -Wall -D__UT_QUEUE__
	@echo "Running Unit Test:"
	./utQueueTest

.PHONY: clean
clean: 
	rm -rf test
	rm -rf utTest
	rm -rf utQueueTest
	rm -rf mainTest
