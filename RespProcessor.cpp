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

void RespProcessor::process_cancel(simdjson::dom::object _obj){
    currentResponse.clear();
    currentResponse.m_type = resp_type::CANCEL_CONFIRM;
    currentResponse.m_symbolId = mSymIDManager->getID(_obj["order"]["symbol"].get_string());
    currentResponse.m_order_id = stringViewToInt(_obj["order"]["client_order_id"].get_string()).value();

    mShmemManager->write_resp(currentResponse);
}

void RespProcessor::process_fill(simdjson::dom::object _obj){
    currentResponse.clear();
    currentResponse.m_type = resp_type::TRADE_CONFIRM;
    currentResponse.m_symbolId = mSymIDManager->getID(_obj["order"]["symbol"].get_string());
    currentResponse.m_order_id = stringViewToInt(_obj["order"]["client_order_id"].get_string()).value();
    std::string_view sv = _obj["price"].get_string();
    double value;
    std::from_chars(sv.data(), sv.data() + sv.length(), value);
    currentResponse.m_resp_price = Price(value * DOLLAR);
    sv = _obj["qty"].get_string();
    std::from_chars(sv.data(), sv.data() + sv.length(), value);
    currentResponse.m_resp_quant = Shares(value);

    std::string_view sv_side = _obj["order"]["side"].get_string();
    if(sv_side == "buy") currentResponse.m_side = side::BUY;
    else currentResponse.m_side = side::SELL;

    mShmemManager->write_resp(currentResponse);
}

void RespProcessor::process_order_reject(simdjson::dom::object _obj){
    currentResponse.clear();
    currentResponse.m_type = resp_type::ORDER_REJECT;
    currentResponse.m_symbolId = mSymIDManager->getID(_obj["order"]["symbol"].get_string());
    currentResponse.m_order_id = stringViewToInt(_obj["order"]["client_order_id"].get_string()).value();

    mShmemManager->write_resp(currentResponse);
}

void RespProcessor::process_cancel_reject(simdjson::dom::object _obj){
    currentResponse.clear();
    currentResponse.m_type = resp_type::CANCEL_REJECT;
    currentResponse.m_symbolId = mSymIDManager->getID(_obj["order"]["symbol"].get_string());
    currentResponse.m_order_id = stringViewToInt(_obj["order"]["client_order_id"].get_string()).value();

    mShmemManager->write_resp(currentResponse);
}

void RespProcessor::process_replace(simdjson::dom::object _obj){
    currentResponse.clear();
    currentResponse.m_type = resp_type::MODORDER_CONFIRM;
    currentResponse.m_symbolId = mSymIDManager->getID(_obj["order"]["symbol"].get_string());
    currentResponse.m_order_id = stringViewToInt(_obj["order"]["client_order_id"].get_string()).value();

    mShmemManager->write_resp(currentResponse);
}

void RespProcessor::process_order_confirm(simdjson::dom::object _obj){
    currentResponse.clear();
    currentResponse.m_type = resp_type::NEWORDER_CONFIRM;
    currentResponse.m_symbolId = mSymIDManager->getID(_obj["order"]["symbol"].get_string());
    currentResponse.m_order_id = stringViewToInt(_obj["order"]["client_order_id"].get_string()).value();

    mShmemManager->write_resp(currentResponse);
}

void RespProcessor::process_replace_reject(simdjson::dom::object _obj){
    currentResponse.clear();
    currentResponse.m_type = resp_type::MOD_REJECT;
    currentResponse.m_symbolId = mSymIDManager->getID(_obj["order"]["symbol"].get_string());
    currentResponse.m_order_id = stringViewToInt(_obj["order"]["client_order_id"].get_string()).value();

    mShmemManager->write_resp(currentResponse);
}
