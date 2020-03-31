#ifndef TASK_LEVEL_PARALLELIZATION_H_
#define TASK_LEVEL_PARALLELIZATION_H_

#include <climits>
#include <cstdint>

#ifndef __SYNTHESIS__

#include <thread>
#include <vector>

#include <glog/logging.h>

#define HLS_STREAM_THREAD_SAFE

#else  // __SYNTHESIS__

struct dummy {
  template <typename T>
  dummy& operator<<(const T&) {
    return *this;
  }
};

#define LOG(level) dummy()
#define LOG_IF(level, cond) dummy()
#define LOG_EVERY_N(level, n) dummy()
#define LOG_IF_EVERY_N(level, cond, n) dummy()
#define LOG_FIRST_N(level, n) dummy()

#define DLOG(level) dummy()
#define DLOG_IF(level, cond) dummy()
#define DLOG_EVERY_N(level, n) dummy()

#define CHECK(cond) \
  assert(cond);     \
  dummy()
#define CHECK_NE(lhs, rhs) \
  assert((lhs) != (rhs));  \
  dummy()
#define CHECK_EQ(lhs, rhs) \
  assert((lhs) != (rhs));  \
  dummy()
#define CHECK_GE(lhs, rhs) \
  assert((lhs) >= (rhs));  \
  dummy()
#define CHECK_GT(lhs, rhs) \
  assert((lhs) > (rhs));   \
  dummy()
#define CHECK_LE(lhs, rhs) \
  assert((lhs) <= (rhs));  \
  dummy()
#define CHECK_LT(lhs, rhs) \
  assert((lhs) < (rhs));   \
  dummy()
#define CHECK_NOTNULL(ptr) (ptr)
#define CHECK_STREQ(lhs, rhs) dummy()
#define CHECK_STRNE(lhs, rhs) dummy()
#define CHECK_STRCASEEQ(lhs, rhs) dummy()
#define CHECK_STRCASENE(lhs, rhs) dummy()
#define CHECK_DOUBLE_EQ(lhs, rhs) dummy()

#define VLOG_IS_ON(level) false
#define VLOG(level) dummy()
#define VLOG_IF(level, cond) dummy()
#define VLOG_EVERY_N(level, n) dummy()
#define VLOG_IF_EVERY_N(level, cond, n) dummy()
#define VLOG_FIRST_N(level, n) dummy()

#endif  // __SYNTHESIS__

#include <ap_int.h>

#include "tlp/mmap.h"
#include "tlp/stream.h"
#include "tlp/synthesizable/traits.h"
#include "tlp/synthesizable/util.h"

namespace tlp {

template <typename T, uint64_t N>
struct vec_t {
  template <typename U>
  operator vec_t<U, N>() {
#pragma HLS inline
    vec_t<U, N> result;
    for (uint64_t i = 0; i < N; ++i) {
      result.set(i, static_cast<U>(get(i)));
    }
    return result;
  }
  static constexpr uint64_t length = N;
  static constexpr uint64_t bytes = length * sizeof(T);
  static constexpr uint64_t bits = bytes * CHAR_BIT;
  ap_uint<bits> data = 0;
  // T& operator[](uint64_t idx) { return *(reinterpret_cast<T*>(&data) + idx);
  // }
  void set(uint64_t idx, T val) {
#pragma HLS inline
    data.range((idx + 1) * widthof<T>() - 1, idx * widthof<T>()) =
        reinterpret_cast<ap_uint<widthof<T>()>&>(val);
  }
  T get(uint64_t idx) const {
#pragma HLS inline
    return reinterpret_cast<T&&>(static_cast<ap_uint<widthof<T>()>>(
        data.range((idx + 1) * widthof<T>() - 1, idx * widthof<T>())));
  }
  T operator[](uint64_t idx) const {
#pragma HLS inline
    return get(idx);
  }
};

}  // namespace tlp

#endif  // TASK_LEVEL_PARALLELIZATION_H_