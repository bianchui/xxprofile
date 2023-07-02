// Copyright 2018 bianchui. All rights reserved.
#define XX_ENABLE_PROFILE 1
#ifdef XX_PLATFORM_WINDOWS
#  define HAVE_PRETTY_FUNCTION 0
#endif//XX_PLATFORM_WINDOWS
#include "../src/xxprofile.hpp"
#include "../src/xxprofile_archive.hpp"
#include "../src/platforms/platform.hpp"
#include <thread>
#include <chrono>

#include <vector>
#include <thread>
#include <iostream>
#include <time.h>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
//#include <sys/syscall.h>
//#include <sys/types.h>
#include <assert.h>

#ifndef XX_PLATFORM_WINDOWS
__thread int a = 0;
#endif//XX_PLATFORM_WINDOWS

#define OPERATOR_0PARAM(op) int operator op() { XX_PROFILE_SCOPE_FUNCTION(); return 1; }
#define OPERATOR_1PARAM(op) int operator op(int a0) { XX_PROFILE_SCOPE_FUNCTION(); return 1; }
template <typename T>
void any_receiver(const T&/*v*/) {
    XX_PROFILE_SCOPE_FUNCTION();
}

class Operators {
public:
    Operators() {
        XX_PROFILE_SCOPE_FUNCTION();
    }
    ~Operators() {
        XX_PROFILE_SCOPE_FUNCTION();
    }

    // Arithmetic operators
    OPERATOR_1PARAM(=); // a = b
    OPERATOR_1PARAM(+); // a + b
    OPERATOR_1PARAM(-); // a - b
    OPERATOR_0PARAM(+); // +a
    OPERATOR_0PARAM(-); // -a
    OPERATOR_1PARAM(*); // a * b
    OPERATOR_1PARAM(/); // a / b
    OPERATOR_1PARAM(%); // a % b
    OPERATOR_0PARAM(++); // ++a
    OPERATOR_1PARAM(++); // a++
    OPERATOR_0PARAM(--); // --a
    OPERATOR_1PARAM(--); // a--
    
    //Comparison operators/relational operators
    OPERATOR_1PARAM(==); // a == b
    OPERATOR_1PARAM(!=); // a != b
    OPERATOR_1PARAM(<); // a < b
    OPERATOR_1PARAM(>); // a > b
    OPERATOR_1PARAM(<=); // a <= b
    OPERATOR_1PARAM(>=); // a >= b

    // Logical operators
    OPERATOR_0PARAM(!); // !a
    OPERATOR_1PARAM(&&); // a && b
    OPERATOR_1PARAM(||); // a || b

    // Bitwise operators
    OPERATOR_0PARAM(~); // ~a
    OPERATOR_1PARAM(&); // a & b
    OPERATOR_1PARAM(|); // a | b
    OPERATOR_1PARAM(^); // a ^ b
    OPERATOR_1PARAM(<<); // a << b
    OPERATOR_1PARAM(>>); // a >> b

    // Compound assignment operators
    OPERATOR_1PARAM(+=); // a+=
    OPERATOR_1PARAM(-=); // a-=
    OPERATOR_1PARAM(*=); // a*=
    OPERATOR_1PARAM(/=); // a/=
    OPERATOR_1PARAM(%=); // a%=
    OPERATOR_1PARAM(&=); // a&=
    OPERATOR_1PARAM(|=); // a|=
    OPERATOR_1PARAM(^=); // a^=
    OPERATOR_1PARAM(<<=); // a<<=
    OPERATOR_1PARAM(>>=); // a>>=

    // Member and pointer operators
    OPERATOR_1PARAM([]); // a[b]
    OPERATOR_0PARAM(*); // *a
    OPERATOR_0PARAM(&); // &a
    struct Sub {
        int test() {
            XX_PROFILE_SCOPE_FUNCTION();
            return 1;
        }
    };
    Sub* operator ->() {
        XX_PROFILE_SCOPE_FUNCTION();
        static Sub sub;
        return &sub;
    }
    OPERATOR_1PARAM(->*); // a->*b

    // Other operators
    OPERATOR_1PARAM(()); // a(b)
    int operator ,(int a) {
        XX_PROFILE_SCOPE_FUNCTION();
        return 1;
    }
};

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
typedef fun_ptr (*fun2_ptr)(fun_ptr);

void fun_get(const char buf[]) {
    XX_PROFILE_SCOPE_FUNCTION();
    AAA<std::map<int, int>> aa;
    AAA<std::set<int>> bb;
    AAA<std::unordered_map<int, int>> cc;
    AAA<std::unordered_set<int>> dd;
    AAA<int[4]> ee;
    AAA<fun_ptr> ff;
    //BBB<int[4]> gg;
    AAA<fun2_ptr> hh;
    AAA<std::string> ii;

    {
        auto glambda = [](int a, int&& b) { return a < b; };
        any_receiver(glambda);
    }


    int buf2[4];
    fun_get2(buf2);
    int buf5[5];
    ee.def(buf2, buf5);

    {
        { fun_get3(dd); }
        { any_receiver<int>(ee); }
        { std::set<int> r = bb; }
        { std::vector<bool> r = ee; }
    }
}

void test_operators() {
    XX_PROFILE_SCOPE_FUNCTION();
    Operators ops;
    {
        XX_PROFILE_SCOPE_NAME(Arithmetic);
        { any_receiver(ops = 10); }
        { any_receiver(ops + 10); }
        { any_receiver(ops - 10); }
        { any_receiver(+ops); }
        { any_receiver(-ops); }
        { any_receiver(ops * 10); }
        { any_receiver(ops / 10); }
        { any_receiver(ops % 10); }
        { any_receiver(ops++); }
        { any_receiver(++ops); }
        { any_receiver(ops--); }
        { any_receiver(--ops); }
    }

    {
        XX_PROFILE_SCOPE_NAME(Comparison_relational);
        { any_receiver(ops == 10); }
        { any_receiver(ops != 10); }
        { any_receiver(ops < 10); }
        { any_receiver(ops > 10); }
        { any_receiver(ops <= 10); }
        { any_receiver(ops >= 10); }
    }

    {
        XX_PROFILE_SCOPE_NAME(Logical);
        { any_receiver(!ops); }
        { any_receiver(ops && 10); }
        { any_receiver(ops || 10); }
    }

    {
        XX_PROFILE_SCOPE_NAME(Bitwise);
        { any_receiver(~ops); }
        { any_receiver(ops & 3); }
        { any_receiver(ops | 3); }
        { any_receiver(ops ^ 3); }
        { any_receiver(ops << 3); }
        { any_receiver(ops >> 3); }
    }

    {
        XX_PROFILE_SCOPE_NAME(Compound_assignment);
        { any_receiver(ops += 10); }
        { any_receiver(ops -= 10); }
        { any_receiver(ops *= 10); }
        { any_receiver(ops /= 10); }
        { any_receiver(ops %= 10); }
        { any_receiver(ops &= 10); }
        { any_receiver(ops |= 10); }
        { any_receiver(ops ^= 10); }
        { any_receiver(ops <<= 10); }
        { any_receiver(ops >>= 10); }
    }

    {
        XX_PROFILE_SCOPE_NAME(Member_pointer);
        { any_receiver(ops[1]); }
        { any_receiver(*ops); }
        { any_receiver(&ops); }
        { any_receiver(ops->*1); }
        { any_receiver(ops->test()); }
    }

    {
        XX_PROFILE_SCOPE_NAME(Other);
        { any_receiver(ops(1)); }
        //{ any_receiver(ops, 1); }
    }
}

void fun() {
    XX_PROFILE_SCOPE_FUNCTION();

    test_operators();

    fun_get("sdf");
    SSS sss;
    sss.abcde();
}

void* static_thread(uint32_t id) {
    static constexpr uint32_t kTestCount = 100000;
    const uint32_t start = id * kTestCount;

    if (true) {
        for (uint32_t i = 0; i < kTestCount; ++i) {
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
    std::thread threads[3];
    for (size_t i = 0; i < XX_ARRAY_COUNTOF(threads); ++i) {
        threads[i] = std::thread(static_thread, i);
    }
    static_thread(NULL);

    for (size_t i = 0; i < XX_ARRAY_COUNTOF(threads); ++i) {
        threads[i].join();
    }

    //sleep(1);
}

#ifndef XX_PLATFORM_WINDOWS
#  include "tests/tls_test.hpp"
#endif//XX_PLATFORM_WINDOWS

// test result
// 1000000 cycles
//       |    1     |   1  |    5     |   5  |    9     |   9  |
// ------|----------|------|----------|------|----------|------|
// none: |  6168866 | 336M |          |      |          |      |
// zlib: | 11291975 | 106M | 16611342 | 100M | 32523920 | 100M |
// lzo:  |          |      |          |      |          |      |
// lz4:  |  2921585 | 178M |  2247653 | 182M |  2676296 | 195M |
// lz4hc:|  4684854 | 164M |  5630633 | 164M |  6930826 | 163M |

int main(int argc, const char * argv[]) {
    //test::test_tls_thread_local();
    //test::test_tls_pthread();
    //return 0;
    
    XX_PROFILE_STATIC_INIT(NULL);
    
#define TEST_ATOMIC(x, y) { std::atomic_int x; printf("%d\n", y); }
    TEST_ATOMIC(i(0), i++);
    TEST_ATOMIC(i(0), ++i);
    TEST_ATOMIC(i(0), i.fetch_add(1));
    TEST_ATOMIC(i(1), i.fetch_sub(1));

    auto start = std::chrono::steady_clock::now();
    
    //testLoad();

    printf("Hello, World!\n");

    //test_cycles();
    //test_threads();
    static_thread(NULL);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "take: " << diff.count() << "s\n";
    //testSave();

    //std::cout << a << std::endl;
    //std::cout << std::this_thread::get_id() << std::endl;
    //std::cout << pthread_self() << std::endl;
    XX_PROFILE_STATIC_UNINIT();
    return 0;
}


