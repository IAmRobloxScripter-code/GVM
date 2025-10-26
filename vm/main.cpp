#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include "./ByteInfo.hpp"
#include "Vm.hpp"

#define VM_VERSION 1

int main(int argc, char** argv){
	if(argc < 2){
		std::cerr << "Vm: No input files\n";
		exit(1);
	}

	//Read bytes
	std::ifstream File(argv[1], std::ios::binary | std::ios::ate);
	if(!File.is_open()){
		std::cerr << "Vm: Could not open '" << argv[1] << "'\n";
		exit(1);
	}

	uint64_t FileSize = File.tellg();
	if(FileSize < 32){
		std::cerr << "Vm: Not a valid file '" << argv[1] << "'\n";
		exit(1);	
	};

	if(FileSize == 32){
		std::cerr << "Vm: Nothing to run '" << argv[1] << "'\n";
		exit(1);		
	}

	uint8_t* Header = new uint8_t[32];

	File.seekg(0, std::ios::beg);
	File.read((char*)Header, 32);
	if(Header[0] != 'g' || Header[1] != 'v' || Header[2] != 'm'){
		std::cerr << "Vm: Not a valid file '" << argv[1] << "'\n";
		exit(1);	
	}

	if(Header[3] > VM_VERSION){
		std::cerr << "Vm: Expected Version <=" << VM_VERSION / 10 << "." << VM_VERSION % 10 << "\n";
		exit(1);		
	}


	uint64_t ByteSize;
	std::memcpy(&ByteSize, &Header[4], 8);
	uint8_t* Bytes = new uint8_t[ByteSize];

	File.seekg(32, std::ios::beg);
	File.read((char*)Bytes, ByteSize);

	Vm(G_Header{.size = ByteSize, .Bytes = Bytes}).Execute();
}
