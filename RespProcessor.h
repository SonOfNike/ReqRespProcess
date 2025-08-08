#pragma once

#include "../Utils/MDShmem.h"
#include "../Utils/Response.h"
#include "../Utils/simdjson/simdjson.h"
#include "ShmemManager.h"
#include "../Utils/SymbolIDManager.h"

class RespProcessor {
private:

    ShmemManager* mShmemManager;
    SymbolIDManager* mSymIDManager;

    Response    currentResponse;

    static RespProcessor* uniqueInstance;
    RespProcessor(){;}

public:
    static RespProcessor* getInstance();
    void startUp();
    void shutDown();
    void process_cancel(const simdjson::dom::object& obj);
    void process_fill(const simdjson::dom::object& obj);
    void process_order_reject(const simdjson::dom::object& obj);
    void process_cancel_reject(const simdjson::dom::object& obj);
    void process_replace(const simdjson::dom::object& obj);
    void process_order_confirm(const simdjson::dom::object& obj);
    void process_replace_reject(const simdjson::dom::object& obj);
};