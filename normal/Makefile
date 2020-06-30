CXX 	= g++
CFLAGS 	= -I . -g -Wpedantic -Wall -Wextra -std=c++17

SRC 	= $(wildcard *.cpp)


app: $(SRC)
	$(CXX) $(CFLAGS) $^ -o main

run:
	./main

debug:
	gdb ./main

clean:
	rm -f main

time:
	make && time ./main
