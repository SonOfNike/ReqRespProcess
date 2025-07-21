#pragma once

#include "../Utils/RespShmem.h"
#include "../Utils/Response.h"

class ShmemManager {
private:
    RespShmem* resp_shmem = nullptr;

    static ShmemManager* uniqueInstance;
    ShmemManager(){;}

public:
    static ShmemManager* getInstance();
    void startUp();
    void shutDown();
    void write_resp(const Response& _response);
};