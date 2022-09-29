
all: MyFind

MyFind: MyFind.cpp
	g++ -std=c++17 -Wall -Werror -o MyFind MyFind.cpp

clean:
	rm -f MyFind
