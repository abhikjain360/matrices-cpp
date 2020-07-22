CXX 	= g++
CFLAGS 	= -I . -pthread -Wpedantic -Wall -Wextra -std=c++20
SRC 	= $(wildcard *.cpp)


app: $(SRC)
	$(CXX) $(CFLAGS) -O3 $^ -o main

test: $(SRC)
	$(CXX) $(CFLAGS) -O3 $^ -o main	&& ./main

debug: $(SRC)
	$(CXX) $(CFLAGS) -g $^ -o main && gdb ./main

clean:
	rm -f main matrix

time: $(SRC)
	$(CXX) $(CFLAGS) -O3 $^ -o main && time ./main
