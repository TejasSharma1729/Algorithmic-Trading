#include <iostream>
#include <mutex>
#include <atomic>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <pthread.h>
#include <chrono>
#include "market.h"


int NUM_THREADS = 0; // Number of worker threads
extern const int NUM_STEPS;  // Number of time steps

// std::atomic<int> commonTimer = 0; // Shared timer
std::atomic<int> commonTimer(0); // For MAC/UNIX
std::mutex printMutex; // Mutex for thread-exclusive printing

// Function to simulate work for each worker thread
extern void* workerThread(void* arg);
extern int reader(int time);
extern int trader(std::string* message);
extern void* userThread(void* arg);
extern void* userTrader(void* arg);

int main(int argc, char** argv) {
    if (argc != 2) throw invalid_argument("Which testcase? Enter 1 - 5");
    int inputNum = stoi(argv[1]);
    if (inputNum < 1 || inputNum > 5) throw invalid_argument("Which testcase? Enter 1 - 5");
    if (inputNum == 1) NUM_THREADS = 2;
    else if (inputNum == 2) NUM_THREADS = 5;
    else if (inputNum == 3) NUM_THREADS = 10;
    else if (inputNum == 4) NUM_THREADS = 25;
    else NUM_THREADS = 50;

    std::cout << "TL" << std::endl;
    std::vector<pthread_t> clientThreads;
    for (int i=0;i<NUM_THREADS;i++){
        int* arg = new int [2];
        arg[0] = (i+1);
        arg[1] = inputNum;
        pthread_t clientThread;
        if (pthread_create(&clientThread, nullptr, &workerThread, (void*)arg) != 0) {
            perror("Thread creation error");
            continue;  // Continue listening for other connections
        }
    // Store the thread ID for later joining
        clientThreads.push_back(clientThread);
    }

    pthread_t clientThread;
    if (pthread_create(&clientThread, nullptr, &userThread,(void*) &NUM_THREADS) != 0) {
        perror("Thread creation error");
    }
    clientThreads.push_back(clientThread);

    // Join all worker threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(clientThreads[i], nullptr);
    }

    std::cout << "!@" << std::endl; // Indicates the end of market input

    pthread_join(clientThreads[NUM_THREADS], nullptr);

    std::cout << "@!" << std::endl; // Indicates the end of user input

    market marketMaker(argc, argv);
    std::cout << "Successfully Initiated!" << std::endl;
    marketMaker.start();

    return 0;
}
