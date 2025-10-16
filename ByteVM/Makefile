CCFlags = -Wall -Wextra -O3 -s -g -std=c++20

vm: 
	g++ $(CCFlags) GVM.cpp -o gvm.o

assembler:
	g++ $(CCFlags) ./assembler/assembler.cpp -o assembler.o

run:
	./gvm.o

clean:
	rm ./gvm.o