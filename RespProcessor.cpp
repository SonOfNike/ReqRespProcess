#include "RespProcessor.h"
#include "../Utils/Time_functions.h"
// #include <iostream>
// #include <sys/mman.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>

RespProcessor* RespProcessor::uniqueInstance = nullptr;

RespProcessor* RespProcessor::getInstance(){
    if(uniqueInstance == nullptr){
        uniqueInstance = new RespProcessor();
    }
    return uniqueInstance;
}

void RespProcessor::startUp(){
    mShmemManager = ShmemManager::getInstance();
    mSymIDManager = SymbolIDManager::getInstance();
}

void RespProcessor::shutDown(){
    
}

void RespProcessor::process_resp(simdjson::dom::object _obj){
    currentMD.m_type = md_type::QUOTE;
    currentMD.m_symbolId = mSymIDManager->getID(_obj["S"].get_string());
    currentMD.m_bid_price = Price(_obj["bp"].get_double() * 1000);
    currentMD.m_ask_price = Price(_obj["ap"].get_double() * 1000);
    currentMD.m_bid_quant = Shares(_obj["bs"].get_int64() * 100);
    currentMD.m_ask_quant = Shares(_obj["as"].get_int64() * 100);

    //Timestamp conversion
    currentMD.m_timestamp = parse_timestring(_obj["t"].get_string());
    mShmemManager->write_MD(currentMD);
}
