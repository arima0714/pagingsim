gcc_test:
	gcc -o test.out -DUNIT_TEST -Wall -pthread pagingsim_test.c pagingsim.c util.c -L/usr/local/lib -lcunit

gcc_main:
	gcc -o main.out -Wall -pthread pagingsim.c main.c util.c

gcc:
	make gcc_test
	make gcc_main

execute_main:
	@./main.out 70 ./../trace-gcc.txt
	@rm main.out

execute_test:
	@./test.out
	@rm test.out

execute_git:
	git add .
	git commit -m "makeによる自動commit"
	git push

test_all:
	make gcc_test
	make gcc_main
	make execute_test
	make execute_main

format:
	nkf -Lu --overwrite *.c
	nkf -Lu --overwrite *.h
	indent --linux-style *.c
	indent --linux-style *.h