

#C = /usr/bin/clang-3.6
CPP = /usr/bin/clang++-3.6
#CPP = /usr/bin/g++

filter: test.cpp filter.hpp
	$(CPP) -std=c++11 -I/home/breitsbw/local/include/ test.cpp -o filter

test: filter
	./filter generate input.json
	./filter input.json output.json
