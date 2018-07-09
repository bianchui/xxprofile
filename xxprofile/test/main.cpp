// Copyright 2018 bianchui. All rights reserved.
#define XX_ENABLE_PROFILE 1
#include "../src/xxprofile.hpp"
#include "../src/xxprofile_archive.hpp"
#include "../src/platforms/platform.hpp"

#include <vector>
#include <thread>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <sys/syscall.h>
#include <sys/types.h>

__thread int a = 0;

template<typename T>
class AAA {
public:
    AAA() {
        {
            XX_PROFILE_SCOPE_NAME(TimerAAA);
            {
                XX_PROFILE_SCOPE_FUNCTION();
            }
        }
    }
    ~AAA() {
        XX_PROFILE_SCOPE_FUNCTION();
    }
    void abcd() {
        XX_PROFILE_SCOPE_FUNCTION();
    }
    template<typename T2>
    void def(T& t, T2& t2) {
        XX_PROFILE_SCOPE_FUNCTION();
    }
    int operator+(int t) {
        XX_PROFILE_SCOPE_FUNCTION();
        return 1;
    }
    int operator<<(int t) {
        XX_PROFILE_SCOPE_FUNCTION();
        return 1;
    }
    int operator  +=(int t) {
        XX_PROFILE_SCOPE_FUNCTION();
        return 1;
    }
    int operator-=(int t) {
        XX_PROFILE_SCOPE_FUNCTION();
        return 1;
    }
    int operator[](int t) {
        XX_PROFILE_SCOPE_FUNCTION();
        return 1;
    }
    operator int() {
        XX_PROFILE_SCOPE_FUNCTION();
        return 1;
    }
    operator int*() {
        XX_PROFILE_SCOPE_FUNCTION();
        static int a[3] = {1, 2, 3};
        return a;
    }
    operator T&() {
        XX_PROFILE_SCOPE_FUNCTION();
        return a;
    }
    operator std::vector<bool>() {
        XX_PROFILE_SCOPE_FUNCTION();
        return std::vector<bool>();
    }
    int operator ->*(int a) {
        XX_PROFILE_SCOPE_FUNCTION();
        return 1;
    }
    AAA& operator <<=(int a) {
        XX_PROFILE_SCOPE_FUNCTION();
        return *this;
    }
    //operator int(*)(int)() {
    //    return NULL;
    //}
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

template<typename T>
class BBB {
public:
    BBB() {
        XX_PROFILE_SCOPE_FUNCTION();
        aaa.abcd();
    }
    ~BBB() {
        XX_PROFILE_SCOPE_FUNCTION();
    }

protected:
    AAA<std::vector<T>> aaa;
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

#define Real_SAVE 1
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
    if (false) {
        for (uint32_t i = 0; i < 100000; ++i) {
            char namebuf[1024];
            sprintf(namebuf, "hahahahaha%d", i);
            xxprofile::SName name(namebuf);
        }
    }
#if Real_SAVE
    xxprofile::SName::Serialize(&tag, ar);
#endif
}

template <size_t len>
inline void fun_get2(const int (&buf)[len]) {
    XX_PROFILE_SCOPE_FUNCTION();
}

inline void fun_get3(int i[]) {
    XX_PROFILE_SCOPE_FUNCTION();
}

typedef int(*fun_ptr)(int);

void fun_get(const char buf[]) {
    XX_PROFILE_SCOPE_FUNCTION();
    AAA<std::map<int, int>> aa;
    AAA<std::set<int>> bb;
    AAA<std::unordered_map<int, int>> cc;
    AAA<std::unordered_set<int>> dd;
    AAA<int[4]> ee;
    AAA<fun_ptr> ff;
    //BBB<int[4]> gg;

    int buf2[4];
    fun_get2(buf2);
    int buf5[5];
    ee.def(buf2, buf5);

    { int r = ee + 10; }
    { auto r = ee += 10; }
    { auto r = ee[1]; }
    { fun_get3(dd); }
    { int r = ee; }
    { int r = ee << 3; }
    { std::set<int> r = bb; }
    { std::vector<bool> r = ee; }
    { int r = ee->*1; }
    { ee<<=1; }
}

void fun() {
    XX_PROFILE_SCOPE_FUNCTION();

    fun_get("sdf");
    SSS sss;
    sss.abcde();
}

void* static_thread(void* param) {

    uint32_t start = (uint32_t)(uintptr_t)param;
    if (true) {
        for (uint32_t i = 0; i < 1000; ++i) {
            fun();
            char namebuf[1024];
            sprintf(namebuf, "hahahahaha%d", start + i);
            //xxprofile::SName name(namebuf);
            XX_PROFILE_INCREASE_FRAME();
        }
    }
    return NULL;
}

void test_cycles() {
    uint64_t cycles = xxprofile::Timer::Cycles64();
    for (uint32_t i = 0; i < 1000000; ++i) {
        uint64_t cycles2 = xxprofile::Timer::Cycles64();
        assert(cycles != cycles2);
        cycles = cycles2;
    }
}

void test_threads() {
    pthread_t pt[3] = {0};
    for (size_t i = 0; i < XX_ARRAY_COUNTOF(pt); ++i) {
        pthread_create(pt + i, NULL, static_thread, (void*)(100000 * i));
    }
    static_thread(NULL);

    for (size_t i = 0; i < XX_ARRAY_COUNTOF(pt); ++i) {
        pthread_join(pt[i], NULL);
    }

    //sleep(1);
}

int main(int argc, const char * argv[]) {
    XX_PROFILE_STATIC_INIT();
    //testLoad();

    printf("Hello, World!\n");

    //test_cycles();
    //test_threads();
    static_thread(NULL);


    //testSave();

    //std::cout << a << std::endl;
    //std::cout << std::this_thread::get_id() << std::endl;
    //std::cout << pthread_self() << std::endl;
    XX_PROFILE_STATIC_UNINIT();
    return 0;
}


