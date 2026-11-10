#ifndef TESTS_UNIT_TESTS_MEMORY_LEAK_INC_
#define TESTS_UNIT_TESTS_MEMORY_LEAK_INC_

#include <iostream>
#include <map>
#include <mutex>

#include "src/common/eebus_malloc.h"

typedef struct AllocInfo AllocInfo;

struct AllocInfo {
  size_t size;
  std::string file_name;
  int line;
};

static std::mutex heap_mutex;
static size_t heap_used = 0;
std::map<void*, AllocInfo> heap_used_table;

void* test_malloc(size_t size, const char* file_name, int line) {
  std::lock_guard l(heap_mutex);
  void* const p = malloc(size);
  if (p == nullptr) {
    return nullptr;
  }

  heap_used_table[p] = {size, std::string{file_name}, line};
  heap_used += size;
  return p;
}

void test_free(void* p) {
  std::lock_guard l(heap_mutex);
  free(p);
  heap_used -= heap_used_table[p].size;
  heap_used_table.erase(p);
}

void CheckForMemoryLeaks() {
  if (heap_used != 0) {
    std::cout << "Memory leaks detected for:\n";
    for (const auto& [key, value] : heap_used_table) {
      std::cout << value.file_name << ", " << value.line << "\n";
    }
  }

  heap_used_table.clear();
  heap_used = 0;
}

#endif  // TESTS_UNIT_TESTS_MEMORY_LEAK_INC_
