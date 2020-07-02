CXX 	= g++
CFLAGS 	= -I . -pthread -Wpedantic -Wall -Wextra -std=c++20
SRC 	= $(wildcard *.cpp)


app: $(SRC)
	$(CXX) $(CFLAGS) -O3 $^ -o main

run:
	./main

test:
	make && make run

debug:
	$(CXX) $(CFLAGS) -g $^ -o main && gdb ./main

clean:
	rm -f main

time:
	make && time ./main
