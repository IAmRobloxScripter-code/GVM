#pragma once 

#define GVM_MAJOR_VERSION 0
#define GVM_MINOR_VERSION 1
#define GVM_VERSION  (GVM_MINOR_VERSION * 10) + GVM_MINOR_VERSION

#include <string>
#include <vector>
#include <cstdint> 

class ByteCode{
    std::string FileName;
    std::vector<uint8_t> Bytes;
public:
    ByteCode(std::string FileName){
        this->FileName = FileName;
        this->InitFile();
    };

    void InitFile(){
        
    }


    void MakeNumber(double number){
    
        return Bytes.data();
    };

    uint64_t GetSize(){
        return Bytes.size();
    }
};

//Does your lsp not work or is that on my end?
//liveshare lsps are stupid
//lol