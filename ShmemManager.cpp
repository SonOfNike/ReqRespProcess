#include "ShmemManager.h"
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

ShmemManager* ShmemManager::uniqueInstance = nullptr;

ShmemManager* ShmemManager::getInstance(){
    if(uniqueInstance == nullptr){
        uniqueInstance = new ShmemManager();
    }
    return uniqueInstance;
}

void ShmemManager::startUp(){
    int shm_fd;
    size_t shm_size = sizeof(RespShmem);

    // Create or open the shared memory object
    shm_fd = shm_open(RESP_shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
    }

    // Map the shared memory object into the process's address space
    resp_shmem = (RespShmem*)mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (resp_shmem == MAP_FAILED) {
        perror("mmap");
    }
    close(shm_fd);
}

void ShmemManager::shutDown(){
    if (munmap(resp_shmem, sizeof(RespShmem)) == -1) {
        perror("munmap");
    }
}

void ShmemManager::write_resp(const Response& _response){
    resp_shmem->m_queue[resp_shmem->next_write_index] = _response;
    resp_shmem->next_write_index++;
    if(resp_shmem->next_write_index >= RESP_QUEUE_SIZE){
        resp_shmem->next_write_index = 0;
        resp_shmem->next_write_page++;
    }
}
