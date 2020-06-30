CXX 	= g++
CFLAGS 	= -I . -g -pthread -Wpedantic -Wall -Wextra -std=c++17

SRC 	= $(wildcard *.cpp)


app: $(SRC)
	$(CXX) $(CFLAGS) $^ -o main

run:
	./main

test:
	make && make run

debug:
	make && gdb ./main

clean:
	rm -f main

time:
	make && time ./main
