

#C = /usr/bin/clang-3.6
CPP = /usr/bin/clang++-3.6
#CPP = /usr/bin/g++

test: test.cpp iir_filter.hpp
	$(CPP) -std=c++11 -I/home/breitsbw/local/include/ test.cpp -o test
