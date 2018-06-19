// Copyright 2018 bianchui. All rights reserved.
#include "../src/xxprofile.hpp"
#include <thread>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

__thread int a = 0;
pthread_key_t g_key;

void* static_thread(void* param) {
    for (uint32_t i = 0; i < 100; ++i) {

        uint32_t aa = a++;
        printf("%d:%d:%d\n", xxprofile::GetTid(), i, aa);
    }

    return NULL;
}

int main(int argc, const char * argv[]) {
    xxprofile::XXProfileTimer::InitTiming();

    XX_PROFILE_SCOPE_FUNCTION();

    printf("%d\n", sizeof(std::vector<void*>));

    printf("Hello, World!\n");

    pthread_t pt;
    pthread_create(&pt, NULL, static_thread, NULL);
    pthread_create(&pt, NULL, static_thread, NULL);
    pthread_create(&pt, NULL, static_thread, NULL);

    static_thread(NULL);

    sleep(1);
    std::cout << a << std::endl;
    std::cout << std::this_thread::get_id() << std::endl;
    std::cout << pthread_self() << std::endl;
    return 0;
}
