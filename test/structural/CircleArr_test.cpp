#include <gtest/gtest.h>
#include "CircleArr.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <random>
#include <deque>
#include <stdexcept>
#include <utility>

// constexpr tests

static_assert(
    []() constexpr -> bool {
      CircleArr<int> car;
      return car.Empty() && car.Size() == 0 && car.Capacity() == 0;
    }(),
    "default constructor test");

static_assert(
    []() constexpr -> bool {
      CircleArr<int> car{1, 0, 2, 9, 3, 8, 4, 7, 5, 6};
      return !car.Empty() && car.Size() == 10 && car.Capacity() == 10 && car[0] == 1 && car[1] == 0 && car[2] == 2 &&
             car[3] == 9 && car[4] == 3 && car[5] == 8 && car[6] == 4 && car[7] == 7 && car[8] == 5 && car[9] == 6;
    }(),
    "init_list constructor test");

static_assert(
    []() constexpr -> bool {
      std::array<int, 10> arr{1, 0, 2, 9, 3, 8, 4, 7, 5, 6};
      CircleArr<int> car(arr.begin(), arr.end());
      if (car.Size() != 10) {
        return false;
      }
      for (auto i = 0uz; i < 10; i++) {
        if (arr[i] != car[i]) {
          return false;
        }
      }
      return true;
    }(),
    "iterator constructor test");

static_assert(
    []() constexpr -> bool {
      CircleArr<int> car{1, 0, 2, 9, 3, 8, 4, 7, 5, 6};
      car.PushBack(5).PushFront(-5);
      int sum = std::accumulate(car.begin(), car.end(), 0);
      return sum == 45;
    }(),
    "sum test");

// runtime tests

enum class Operation : uint8_t { kPushBack, kPopBack, kPushFront, kPopFront, kReadAccess, kWriteAccess };

class IDequeComparisonTests : public testing::Test {
 public:
  Operation GetOperation() {
    return static_cast<Operation>(dist(rng) % 6);
  }

  std::uniform_int_distribution<uint8_t> dist;
  std::mt19937 rng;
};

TEST_F(IDequeComparisonTests, DequeComparisonTest) {
  std::deque<uint8_t> deq;
  CircleArr<uint8_t> car;
  for (size_t i = 0; i < 10'000'000; i++) {
    uint8_t dvalue = 0;
    uint8_t cvalue = 0;
    uint8_t index = 0;
    switch (GetOperation()) {
      case Operation::kPushBack:
        dvalue = dist(rng);
        deq.push_back(dvalue);
        car.PushBack(dvalue);
        cvalue = car.Back();
        break;
      case Operation::kPopBack:
        if (deq.size() == 0) {
          ASSERT_THROW(car.PopBackSafe(), std::out_of_range);
          car.Clear();
          break;
        }
        dvalue = deq.back();
        cvalue = car.PopBackValueSafe();
        deq.pop_back();
        break;
      case Operation::kPushFront:
        dvalue = dist(rng);
        deq.push_front(dvalue);
        car.PushFront(dvalue);
        cvalue = car.Front();
        break;
      case Operation::kPopFront:
        if (deq.size() == 0) {
          ASSERT_THROW(car.PopFrontSafe(), std::out_of_range);
          car.Clear();
          break;
        }
        dvalue = deq.front();
        cvalue = car.PopFrontValueSafe();
        deq.pop_front();
        break;
      case Operation::kReadAccess:
        if (deq.size() == 0) {
          ASSERT_THROW(std::as_const(car).At(0), std::out_of_range);
          break;
        }
        index = static_cast<uint8_t>(dist(rng) % deq.size());
        dvalue = std::as_const(deq).at(index);
        cvalue = std::as_const(car).At(index);
        break;
      case Operation::kWriteAccess:
        if (deq.size() == 0) {
          ASSERT_THROW(car.At(0) = 5, std::out_of_range);
          break;
        }
        dvalue = dist(rng);
        index = static_cast<uint8_t>(dist(rng) % deq.size());
        deq.at(index) = dvalue;
        cvalue = (car.At(index) = dvalue);
        break;
    }
    ASSERT_EQ(deq.size(), car.Size());
    ASSERT_EQ(dvalue, cvalue);
  }
}