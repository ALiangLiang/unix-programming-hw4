CPP          = g++
FLAGS        = -Os -ffunction-sections -fdata-sections -lgtest -pthread -lgcov -fprofile-arcs -ftest-coverage --coverage
BIN          = main
OBJ          = hw4.o

.PHONY: all clean watch test tests

all: clean $(BIN) run

clean:
	rm -rfv $(OBJ) main.o tests *.gcno

run:
	./main

$(BIN): $(OBJ) main.o
	$(CPP) $(OBJ) main.o -o main -pthread -Os -ffunction-sections -fdata-sections

test: clean tests
	./test

watch:
	echo "\033[0;33mwatch...\033[0;37m"; \
	watchmedo auto-restart --patterns="*.cpp" --interval 3 make test

tests: tests.o hw4_test.o $(OBJ)
	$(CPP) tests.o hw4_test.o $(OBJ) -o test -lgtest -pthread -lgcov

tests.o: tests.cpp
	$(CPP) -c tests.cpp -o tests.o $(FLAGS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(FLAGS)

hw4.o: hw4.cpp
	$(CPP) -c hw4.cpp -o hw4.o $(FLAGS)

hw4_test.o: hw4_test.cpp
	$(CPP) -c hw4_test.cpp -o hw4_test.o $(FLAGS)

gcov :
	gcov -b tests.cpp
	cat tests.cpp.gcov
