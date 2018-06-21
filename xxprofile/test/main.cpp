// Copyright 2018 bianchui. All rights reserved.
#include "../src/xxprofile.hpp"
#include "../src/xxprofile_archive.hpp"

#include <vector>
#include <thread>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include <sys/syscall.h>
#include <sys/types.h>

__thread int a = 0;
pthread_key_t g_key;

void* static_thread(void* param) {
    for (uint32_t i = 0; i < 100; ++i) {

        uint32_t aa = a++;
        //printf("%d:%d:%d\n", xxprofile::GetTid(), i, aa);
    }

    return NULL;
}

template<typename T>
class AAA {
public:
    AAA() {
        XX_PROFILE_SCOPE_FUNCTION();
    }
    ~AAA() {
        XX_PROFILE_SCOPE_FUNCTION();
    }
    void abcd() {
        XX_PROFILE_SCOPE_FUNCTION();
    }
    T a;
};

class SSS {
public:
    SSS() {
        XX_PROFILE_SCOPE_FUNCTION();
    }
    ~SSS() {
        XX_PROFILE_SCOPE_FUNCTION();
    }

    void abcde() {
        XX_PROFILE_SCOPE_FUNCTION();
        aaa.abcd();
    }

protected:
    AAA<std::vector<std::vector<std::vector<int>>>> aaa;
};

void testLoad() {
    xxprofile::Archive ari;
    ari.open("test.xxprofile", false);

    xxprofile::Archive aro;
    aro.open("test1.xxprofile", true);
    xxprofile::SName::IncrementSerializeTag tag = {0};

    while (!ari.eof()) {
        xxprofile::SName::Serialize(NULL, ari);
        xxprofile::SName::Serialize(&tag, aro);
    }
}

#define Real_SAVE 0
void testSave() {
    XX_PROFILE_SCOPE_FUNCTION();
    SSS sss;
    sss.abcde();


    {
        xxprofile::SName name("asdfasdfadfas");
    }
    {
        xxprofile::SName name("hhahahaha");
    }
#if Real_SAVE
    xxprofile::SName::IncrementSerializeTag tag = {0};
    xxprofile::Archive ar;
    ar.open("test.xxprofile", true);
    xxprofile::SName::Serialize(&tag, ar);
#endif
    {
        xxprofile::SName name("asdfasdfadfas");
    }
#if Real_SAVE
    xxprofile::SName::Serialize(&tag, ar);
#endif
    {
        xxprofile::SName name("asdfasdfadfas1");
    }
    {
        xxprofile::SName name("asdfasdfadfas2");
    }
    for (uint32_t i = 0; i < 100000; ++i) {
        char namebuf[1024];
        sprintf(namebuf, "hahahahaha%d", i);
        xxprofile::SName name(namebuf);
    }
#if Real_SAVE
    xxprofile::SName::Serialize(&tag, ar);
#endif
}

int main(int argc, const char * argv[]) {
    xxprofile::XXProfile::StaticInit();

    testSave();
    //testLoad();

    printf("Hello, World!\n");

    if (false) {
        pthread_t pt;
        pthread_create(&pt, NULL, static_thread, NULL);
        pthread_create(&pt, NULL, static_thread, NULL);
        pthread_create(&pt, NULL, static_thread, NULL);

        static_thread(NULL);

        sleep(1);
    }

    std::cout << a << std::endl;
    std::cout << std::this_thread::get_id() << std::endl;
    std::cout << pthread_self() << std::endl;
    return 0;
}


