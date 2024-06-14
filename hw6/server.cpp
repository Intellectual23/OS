#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#define SHM_KEY 0x123

struct SharedData {
    int data[5];
    bool ready;
    bool finished;
};

int shmid;
SharedData* sharedData;

void cleanUp() {
    shmdt(sharedData);
    shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}

void signalHandler(int sig) {
    cleanUp();
}

int main() {
    signal(SIGINT, signalHandler);

    shmid = shmget(SHM_KEY, sizeof(SharedData), 0644|IPC_CREAT);
    if (shmid == -1) {
        return 1;
    }

    sharedData = (SharedData*) shmat(shmid, NULL, 0);
    if (sharedData == (void*) -1) {
        return 1;
    }

    sharedData->ready = false;
    sharedData->finished = false;

    while (!sharedData->finished) {
        if (sharedData->ready) {
            for (int i = 0; i < N; i++) {
                std::cout << "Received: " << sharedData->data[i] << std::endl;
            }
            sharedData->ready = false;
        }
        sleep(1);
    }

    cleanUp();
    return 0;
}
