// Copyright 2019 bianchui. All rights reserved.
#ifndef tests_tls_test_hpp
#define tests_tls_test_hpp

#include <pthread.h>
#include <thread>
#include <chrono>

namespace test {

class ThreadLocalObject {
 public:
  ThreadLocalObject() { printf("%d: ABC\n", std::this_thread::get_id()); }
  ~ThreadLocalObject() { printf("%d: ~ABC\n", std::this_thread::get_id()); }
};

static thread_local ThreadLocalObject g_threadLocal;
static thread_local int g_test_tls_test = 0;
static volatile int g_add = 1;
static pthread_key_t g_key;

static void test_tls_thread_local_thread() {
  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < 1000000; ++i) {
    g_test_tls_test += g_add;
  }
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> diff = end - start;
  printf("tls_thread_local: %fs\n", diff.count());
  printf("g_test_tls_test: %d\n", g_test_tls_test);
}

static void test_tls_thread_local() {
  std::thread thread(test_tls_thread_local_thread);
  thread.join();
}

static void tls_pthread_callback(void* v) {}

static void test_tls_pthread() {
  pthread_key_create(&g_key, tls_pthread_callback);
  int a = 0;
  pthread_setspecific(g_key, &a);
  auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < 1000000; ++i) {
    int* p = (int*)pthread_getspecific(g_key);
    *p += g_add;
  }
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> diff = end - start;
  printf("tls_pthread: %fs\n", diff.count());
  pthread_key_delete(g_key);
}

}//namespace test

#endif//tests_tls_test_hpp
