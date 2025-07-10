#pragma once

#include "../Utils/MDShmem.h"
#include "../Utils/MDupdate.h"
#include "../Utils/simdjson/simdjson.h"
#include "ShmemManager.h"
#include "../Utils/SymbolIDManager.h"

class RespProcessor {
private:

    ShmemManager* mShmemManager;
    SymbolIDManager* mSymIDManager;

    MDupdate    currentMD;

    static RespProcessor* uniqueInstance;
    RespProcessor(){;}

public:
    static RespProcessor* getInstance();
    void startUp();
    void shutDown();
    void process_resp(simdjson::dom::object obj);
};