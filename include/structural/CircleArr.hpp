#include <compare>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

// Tests show this "optimization" is not worth it.

template <typename Stored>
class CircleArr {
 public:
  constexpr CircleArr() noexcept : storage_{}, meta_(0) {
  }
  constexpr explicit CircleArr(uint64_t capacity) : CircleArr() {
    Reserve(capacity);
  }
  constexpr CircleArr(uint64_t size, Stored value) : CircleArr(size) {
    if (size == 0) {
      return;
    }
    Reserve(size);
    if (IsDynamic()) {
      for (uint64_t i = 0; i + 1 < size; i++) {
        DynamicFitPushBack(value);
      }
      DynamicFitPushBack(std::move(value));
    } else {
      if constexpr (kStaticCapacity > 0) {
        for (uint64_t i = 0; i + 1 < size; i++) {
          StaticFitPushBack(value);
        }
        StaticFitPushBack(std::move(value));
      }
    }
  }
  constexpr CircleArr(std::initializer_list<Stored> init) : CircleArr(init.size()) {
    if (IsDynamic()) {
      for (auto &el : init) {
        DynamicFitPushBack(std::move(el));
      }
    } else {
      if constexpr (kStaticCapacity > 0) {
        for (auto &el : init) {
          StaticFitPushBack(std::move(el));
        }
      }
    }
  }

  constexpr CircleArr(const CircleArr<Stored> &other) : CircleArr(other.Size()) {
    if (IsDynamic()) {
      if (other.IsDynamic()) {
        for (uint32_t i = 0; i < DynamicCapacity(); i++) {
          DynamicFitPushBack(*other.DynamicGet(i));
        }
      } else {
        for (uint32_t i = 0; i < DynamicCapacity(); i++) {
          DynamicFitPushBack(*other.StaticGet(i));
        }
      }
    } else {
      if constexpr (kStaticCapacity > 0) {
        if (other.IsDynamic()) {
          for (uint32_t i = 0; i < other.DynamicSize(); i++) {
            StaticFitPushBack(*other.DynamicGet(i));
          }
        } else {
          for (uint32_t i = 0; i < other.StaticSize(); i++) {
            StaticFitPushBack(*other.StaticGet(i));
          }
        }
      }
    }
  }

  constexpr CircleArr(CircleArr<Stored> &&other)
      : storage_(std::exchange(other.storage_, Storage{})), meta_(std::exchange(other.meta_, 0)) {
  }

  template <std::forward_iterator Iter>
  constexpr CircleArr(Iter start, const Iter end) : CircleArr() {
    if constexpr (std::random_access_iterator<Iter>) {
      if (end - start < 0) [[unlikely]] {
        throw std::out_of_range("iterators out of order");
      }
      Reserve(static_cast<uint64_t>(end - start));
      if (IsDynamic()) {
        while (start != end) {
          DynamicFitPushBack(*start);
          start++;
        }
        return;
      }
      if constexpr (kStaticCapacity > 0) {
        while (start != end) {
          StaticFitPushBack(*start);
          start++;
        }
      }
      return;
    }
    while (start != end) {
      PushBack(*start);
      start++;
    }
  }

  constexpr CircleArr<Stored> &operator=(const CircleArr<Stored> &other) {
    *this = auto{other};
    return *this;
  }
  constexpr CircleArr<Stored> &operator=(CircleArr<Stored> &&other) {
    storage_ = std::exchange(other.storage_, Storage{});
    meta_ = std::exchange(other.meta_, 0);
    return *this;
  }

  constexpr Stored &operator[](uint64_t index) noexcept {
    if (IsDynamic()) {
      return *DynamicGet(index);
    }
    return *StaticGet(static_cast<uint32_t>(index));
  }
  constexpr const Stored &operator[](uint64_t index) const noexcept {
    if (IsDynamic()) {
      return *DynamicGet(index);
    }
    return *StaticGet(static_cast<uint32_t>(index));
  }
  constexpr Stored &At(uint64_t index) {
    if (index >= Size()) [[unlikely]] {
      throw std::out_of_range("subscript out of range");
    }
    return (*this)[index];
  }
  constexpr const Stored &At(uint64_t index) const {
    if (index >= Size()) [[unlikely]] {
      throw std::out_of_range("subscript out of range");
    }
    return (*this)[index];
  }

  constexpr CircleArr<Stored> &PushBack(Stored value) {
    if (IsDynamic()) {
      DynamicPushBack(std::move(value));
    } else {
      StaticPushBack(std::move(value));
    }
    return *this;
  }
  constexpr CircleArr<Stored> &PushFront(Stored value) {
    if (IsDynamic()) {
      DynamicPushFront(std::move(value));
    } else {
      StaticPushFront(std::move(value));
    }
    return *this;
  }

  constexpr CircleArr<Stored> &PopBack() noexcept {
    NonEmptyGuarantees();
    if (IsDynamic()) {
      DynamicPopBack();
    } else {
      StaticPopBack();
    }
    return *this;
  }
  constexpr CircleArr<Stored> &PopBackSafe() {
    ValidStateGuarantees();
    ThrowIfEmpty();
    PopBack();
    return *this;
  }
  constexpr Stored PopBackValue() noexcept(std::is_nothrow_move_constructible_v<Stored>) {
    NonEmptyGuarantees();
    if (IsDynamic()) {
      return DynamicPopBackValue();
    }
    return StaticPopBackValue();
  }
  constexpr Stored PopBackValueSafe() {
    ValidStateGuarantees();
    ThrowIfEmpty();
    return PopBackValue();
  }

  constexpr CircleArr<Stored> &PopFront() noexcept {
    NonEmptyGuarantees();
    if (IsDynamic()) {
      DynamicPopFront();
    } else {
      StaticPopFront();
    }
    return *this;
  }
  constexpr CircleArr<Stored> &PopFrontSafe() {
    ValidStateGuarantees();
    ThrowIfEmpty();
    PopFront();
    return *this;
  }
  constexpr Stored PopFrontValue() noexcept(std::is_nothrow_move_constructible_v<Stored>) {
    NonEmptyGuarantees();
    if (IsDynamic()) {
      return DynamicPopFrontValue();
    }
    return StaticPopFrontValue();
  }
  constexpr Stored PopFrontValueSafe() {
    ValidStateGuarantees();
    ThrowIfEmpty();
    return PopFrontValue();
  }

  constexpr Stored &Front() noexcept {
    NonEmptyGuarantees();
    if (IsDynamic()) {
      return *DynamicFront();
    }
    return *StaticFront();
  }
  constexpr const Stored &Front() const noexcept {
    NonEmptyGuarantees();
    if (IsDynamic()) {
      return *DynamicFront();
    }
    return *StaticFront();
  }
  constexpr Stored &FrontSafe() {
    ValidStateGuarantees();
    ThrowIfEmpty();
    return Front();
  }
  constexpr const Stored &FrontSafe() const {
    ValidStateGuarantees();
    ThrowIfEmpty();
    return Front();
  }
  constexpr Stored &Back() noexcept {
    NonEmptyGuarantees();
    if (IsDynamic()) {
      return *DynamicBack();
    }
    return *StaticBack();
  }
  constexpr const Stored &Back() const noexcept {
    NonEmptyGuarantees();
    if (IsDynamic()) {
      return *DynamicBack();
    }
    return *StaticBack();
  }
  constexpr Stored &BackSafe() {
    ValidStateGuarantees();
    ThrowIfEmpty();
    return Back();
  }
  constexpr const Stored &BackSafe() const {
    ValidStateGuarantees();
    ThrowIfEmpty();
    return Back();
  }

  constexpr size_t Size() const noexcept {
    if (IsDynamic()) {
      return DynamicSize();
    }
    return StaticSize();
  }
  constexpr size_t Empty() const noexcept {
    return Size() == 0;
  }
  constexpr size_t Capacity() const noexcept {
    if (IsDynamic()) {
      return DynamicCapacity();
    }
    return kStaticCapacity;
  }
  constexpr CircleArr<Stored> &Clear() noexcept {
    if (IsDynamic()) {
      DynamicClear();
    } else {
      StaticClear();
    }
    return *this;
  }

  constexpr ~CircleArr() noexcept {
    Clear();
    if (IsDynamic()) {
      std::allocator<Stored>{}.deallocate(DynamicStorage(), DynamicCapacity());
    }
  }

  constexpr CircleArr<Stored> &Reserve(uint64_t new_capacity) {
    ValidStateGuarantees();
    if (!IsDynamic()) {
      if (new_capacity <= kStaticCapacity) {
        return *this;
      }
      MoveToHeap(new_capacity);
      if (!IsDynamic()) {
        std::unreachable();
      }
      return *this;
    }
    if (new_capacity <= DynamicCapacity()) {
      return *this;
    }
    Stored *new_storage = std::allocator<Stored>{}.allocate(new_capacity);
    uint64_t new_size = 0;
    try {
      for (; DynamicSize() > 0; new_size++) {
        std::construct_at(&(new_storage[new_size]), std::move(DynamicPopFrontValue()));
      }
    } catch (...) {
      for (size_t i = 0; i < new_size; i++) {
        std::destroy_at(&(new_storage[i]));
      }
      std::allocator<Stored>{}.deallocate(new_storage, new_capacity);
      throw;
    }
    std::allocator<Stored>{}.deallocate(DynamicStorage(), DynamicCapacity());
    SetDynamicStorage(new_storage);
    SetDynamicCapacity(new_capacity);
    SetDynamicBegin(0);
    SetDynamicSize(new_size);
    return *this;
  }

  template <bool Const>
  class Iterator;

  using RAIterator = Iterator<false>;
  using ConstIterator = Iterator<true>;

  // NOLINTNEXTLINE (readability-identifier-naming)
  constexpr RAIterator begin() noexcept {
    ValidStateGuarantees();
    return RAIterator(*this, 0);
  }  // NOLINTNEXTLINE (readability-identifier-naming)
  constexpr RAIterator end() noexcept {
    ValidStateGuarantees();
    return RAIterator(*this, Size());
  }
  // NOLINTNEXTLINE (readability-identifier-naming)
  constexpr ConstIterator cbegin() const noexcept {
    ValidStateGuarantees();
    return ConstIterator(*this, 0);
  }  // NOLINTNEXTLINE (readability-identifier-naming)
  constexpr ConstIterator cend() const noexcept {
    ValidStateGuarantees();
    return ConstIterator(*this, Size());
  }
  // NOLINTNEXTLINE (readability-identifier-naming)
  constexpr ConstIterator begin() const noexcept {
    return cbegin();
  }  // NOLINTNEXTLINE (readability-identifier-naming)
  constexpr RAIterator end() const noexcept {
    return cend();
  }

 private:
  static constexpr uint64_t kStaticCapacity = 24 / sizeof(Stored);
  static constexpr uint64_t kPaddingSize = 24 - kStaticCapacity * sizeof(Stored);
  using StaticStored = std::conditional_t<(kStaticCapacity > 0), Stored, uint8_t>;
  alignas(uint64_t) union Storage {
    struct Dynamic {
      Stored *ptr;
      uint64_t capacity;
      uint64_t begin;
    } dynamic;
    StaticStored static_array[kStaticCapacity > 0 ? kStaticCapacity : 1];
    constexpr Storage() noexcept : static_array{} {};
    constexpr ~Storage() noexcept {};
  } storage_;
  uint64_t meta_;

  constexpr bool IsDynamic() const noexcept {
    return meta_ & (1ull << 63);
  }
  constexpr uint32_t StaticSize() const noexcept {
    if (IsDynamic()) {
      std::unreachable();
    }
    return static_cast<uint32_t>(meta_ >> 32);
  }
  constexpr void SetStaticSize(uint32_t value) noexcept {
    if (IsDynamic()) {
      std::unreachable();
    }
    meta_ = static_cast<uint32_t>(meta_) | (static_cast<uint64_t>(value) << 32);
  }
  constexpr bool StaticEmpty() const noexcept {
    return StaticSize() == 0;
  }
  constexpr uint32_t StaticBegin() const noexcept {
    if (IsDynamic()) {
      std::unreachable();
    }
    return static_cast<uint32_t>(meta_);
  }
  constexpr void SetStaticBegin(uint32_t value) noexcept {
    if (IsDynamic()) {
      std::unreachable();
    }
    meta_ = value | (meta_ & 0xff'ff'ff'ff'00'00'00'00);
  }
  constexpr Stored *StaticLocation(uint32_t index) noexcept {
    if (index >= kStaticCapacity || kStaticCapacity == 0) {
      std::unreachable();
    }
    return storage_.static_array + index;
  }
  constexpr const Stored *StaticLocation(uint32_t index) const noexcept {
    if (index >= kStaticCapacity || kStaticCapacity == 0) {
      std::unreachable();
    }
    return storage_.static_array + index;
  }

  constexpr Stored *DynamicStorage() noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    return storage_.dynamic.ptr;
  }
  constexpr const Stored *DynamicStorage() const noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    return storage_.dynamic.ptr;
  }
  constexpr void SetDynamicStorage(Stored *ptr) noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    storage_.dynamic.ptr = ptr;
  }
  constexpr uint64_t DynamicCapacity() const noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    return storage_.dynamic.capacity;
  }
  constexpr void SetDynamicCapacity(uint64_t value) noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    storage_.dynamic.capacity = value;
  }
  constexpr uint64_t DynamicBegin() const noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    return storage_.dynamic.begin;
  }
  constexpr void SetDynamicBegin(uint64_t value) noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    storage_.dynamic.begin = value;
  }
  constexpr uint64_t DynamicSize() const noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    return meta_ & ((1ull << 63) - 1);
  }
  constexpr void SetDynamicSize(uint64_t value) noexcept {
    if (!IsDynamic()) {
      std::unreachable();
    }
    meta_ = value | (1ull << 63);
  }
  constexpr bool DynamicEmpty() const noexcept {
    return DynamicSize() == 0;
  }

  constexpr void StaticValidStateGuarantees() const noexcept {
    if (IsDynamic() || ((kStaticCapacity > 0) && (StaticBegin() >= kStaticCapacity)) ||
        ((kStaticCapacity == 0) && (StaticBegin() != 0)) || StaticSize() > kStaticCapacity) {
      std::unreachable();
    }
  }
  constexpr void StaticCapacityGuarantees() const noexcept {
    StaticValidStateGuarantees();
    if constexpr (kStaticCapacity == 0) {
      std::unreachable();
    }
  }
  constexpr void StaticNonEmptyGuarantees() const noexcept {
    StaticCapacityGuarantees();
    if (StaticSize() == 0) {
      std::unreachable();
    }
  }

  constexpr void DynamicValidStateGuarantees() const noexcept {
    if ((!IsDynamic()) || (DynamicSize() > DynamicCapacity()) || (DynamicStorage() == nullptr) ||
        (DynamicCapacity() == 0) || (DynamicBegin() >= DynamicCapacity())) {
      std::unreachable();
    }
  }
  constexpr void DynamicNonEmptyGuarantees() const noexcept {
    DynamicValidStateGuarantees();
    if (DynamicSize() == 0) {
      std::unreachable();
    }
  }

  constexpr void ValidStateGuarantees() const noexcept {
    if (IsDynamic()) {
      DynamicValidStateGuarantees();
    } else {
      StaticValidStateGuarantees();
    }
  }
  constexpr void CapacityGuarantees() const noexcept {
    if (IsDynamic()) {
      DynamicValidStateGuarantees();
    } else {
      StaticCapacityGuarantees();
    }
  }
  constexpr void NonEmptyGuarantees() const noexcept {
    if (IsDynamic()) {
      DynamicNonEmptyGuarantees();
    } else {
      StaticNonEmptyGuarantees();
    }
  }

  constexpr Stored *StaticGet(uint32_t index) noexcept {
    StaticCapacityGuarantees();
    if (StaticBegin() + index >= kStaticCapacity) {
      return StaticLocation(StaticBegin() + index - kStaticCapacity);
    }
    return StaticLocation(StaticBegin() + index);
  }
  constexpr const Stored *StaticGet(uint32_t index) const noexcept {
    StaticCapacityGuarantees();
    if (StaticBegin() + index >= kStaticCapacity) {
      return StaticLocation(StaticBegin() + index - kStaticCapacity);
    }
    return StaticLocation(StaticBegin() + index);
  }

  constexpr Stored *StaticFront() noexcept {
    StaticCapacityGuarantees();
    return StaticGet(0);
  }
  constexpr const Stored *StaticFront() const noexcept {
    StaticCapacityGuarantees();
    return StaticGet(0);
  }
  constexpr Stored *StaticBack() noexcept {
    StaticNonEmptyGuarantees();
    return StaticGet(StaticSize() - 1);
  }
  constexpr const Stored *StaticBack() const noexcept {
    StaticNonEmptyGuarantees();
    return StaticGet(StaticSize() - 1);
  }

  constexpr void StaticPopFront() noexcept {
    StaticNonEmptyGuarantees();
    std::destroy_at(StaticFront());
    uint32_t new_begin = StaticBegin() + 1;
    if (new_begin == kStaticCapacity) {
      new_begin = 0;
    }
    SetStaticBegin(new_begin);
    SetStaticSize(StaticSize() - 1);
  }
  constexpr Stored StaticPopFrontValue() noexcept(std::is_nothrow_move_constructible_v<Stored>) {
    StaticNonEmptyGuarantees();
    Stored value(std::move(*StaticFront()));
    StaticPopFront();
    return value;
  }
  constexpr void StaticPopBack() noexcept {
    StaticNonEmptyGuarantees();
    std::destroy_at(StaticBack());
    SetStaticSize(StaticSize() - 1);
  }
  constexpr Stored StaticPopBackValue() noexcept(std::is_nothrow_move_constructible_v<Stored>) {
    StaticNonEmptyGuarantees();
    Stored value(std::move(*StaticBack()));
    StaticPopBack();
    return value;
  }

  constexpr Stored *DynamicGet(uint64_t index) noexcept {
    DynamicValidStateGuarantees();
    if (DynamicBegin() + index >= DynamicCapacity()) {
      return DynamicStorage() + (DynamicBegin() + index - DynamicCapacity());
    }
    return DynamicStorage() + (DynamicBegin() + index);
  }
  constexpr const Stored *DynamicGet(uint64_t index) const noexcept {
    DynamicValidStateGuarantees();
    if (DynamicBegin() + index >= DynamicCapacity()) {
      return DynamicStorage() + (DynamicBegin() + index - DynamicCapacity());
    }
    return DynamicStorage() + (DynamicBegin() + index);
  }

  constexpr Stored *DynamicFront() noexcept {
    DynamicValidStateGuarantees();
    return DynamicGet(0);
  }
  constexpr const Stored *DynamicFront() const noexcept {
    DynamicValidStateGuarantees();
    return DynamicGet(0);
  }
  constexpr Stored *DynamicBack() noexcept {
    DynamicNonEmptyGuarantees();
    return DynamicGet(DynamicSize() - 1);
  }
  constexpr const Stored *DynamicBack() const noexcept {
    DynamicNonEmptyGuarantees();
    return DynamicGet(DynamicSize() - 1);
  }

  constexpr void DynamicPopFront() noexcept {
    DynamicNonEmptyGuarantees();
    std::destroy_at(DynamicFront());
    uint64_t new_begin = DynamicBegin() + 1;
    if (new_begin == DynamicCapacity()) {
      new_begin = 0;
    }
    SetDynamicBegin(new_begin);
    SetDynamicSize(DynamicSize() - 1);
  }
  constexpr Stored DynamicPopFrontValue() noexcept(std::is_nothrow_move_constructible_v<Stored>) {
    DynamicNonEmptyGuarantees();
    Stored value(std::move(*DynamicFront()));
    DynamicPopFront();
    return value;
  }
  constexpr void DynamicPopBack() noexcept {
    DynamicNonEmptyGuarantees();
    std::destroy_at(DynamicBack());
    SetDynamicSize(DynamicSize() - 1);
  }
  constexpr Stored DynamicPopBackValue() noexcept(std::is_nothrow_move_constructible_v<Stored>) {
    DynamicNonEmptyGuarantees();
    Stored value(std::move(*DynamicBack()));
    DynamicPopBack();
    return value;
  }

  constexpr void StaticPushBack(Stored value) {
    StaticValidStateGuarantees();
    if (StaticSize() == kStaticCapacity) {
      MoveToHeap(std::max(2 * kStaticCapacity, static_cast<uint64_t>(1)));
      DynamicPushBack(std::move(value));
      return;
    }
    SetStaticSize(StaticSize() + 1);
    std::construct_at(StaticBack(), std::move(value));
  }
  constexpr void StaticFitPushBack(Stored value) noexcept(noexcept(std::construct_at(StaticBack(), std::move(value)))) {
    StaticCapacityGuarantees();
    if (StaticSize() == kStaticCapacity) {
      std::unreachable();
    }
    SetStaticSize(StaticSize() + 1);
    std::construct_at(StaticBack(), std::move(value));
  }
  constexpr void DynamicPushBack(Stored value) {
    DynamicValidStateGuarantees();
    if (DynamicSize() == DynamicCapacity()) {
      Reserve(2 * DynamicCapacity());
    }
    DynamicFitPushBack(std::move(value));
  }
  constexpr void DynamicFitPushBack(Stored value) noexcept(noexcept(std::construct_at(DynamicBack(),
                                                                                      std::move(value)))) {
    DynamicValidStateGuarantees();
    if (DynamicSize() == DynamicCapacity()) {
      std::unreachable();
    }
    SetDynamicSize(DynamicSize() + 1);
    std::construct_at(DynamicBack(), std::move(value));
  }

  constexpr void StaticPushFront(Stored value) {
    StaticValidStateGuarantees();
    if (StaticSize() == kStaticCapacity) {
      MoveToHeap(std::max(2 * kStaticCapacity, static_cast<uint64_t>(1)));
      DynamicPushFront(std::move(value));
      return;
    }
    StaticFitPushFront(std::move(value));
  }
  constexpr void StaticFitPushFront(Stored value) {
    StaticValidStateGuarantees();
    if (StaticSize() == kStaticCapacity) {
      std::unreachable();
    }
    uint32_t new_begin = StaticBegin();
    if (new_begin > 0) {
      new_begin--;
    } else {
      new_begin = kStaticCapacity - 1;
    }
    SetStaticBegin(new_begin);
    SetStaticSize(StaticSize() + 1);
    std::construct_at(StaticFront(), std::move(value));
  }
  constexpr void DynamicPushFront(Stored value) {
    DynamicValidStateGuarantees();
    if (DynamicSize() == DynamicCapacity()) {
      Reserve(2 * DynamicCapacity());
    }
    DynamicFitPushFront(std::move(value));
  }
  constexpr void DynamicFitPushFront(Stored value) {
    DynamicValidStateGuarantees();
    if (DynamicSize() == DynamicCapacity()) {
      std::unreachable();
    }
    uint64_t new_begin = DynamicBegin();
    if (new_begin > 0) {
      new_begin--;
    } else {
      new_begin = DynamicCapacity() - 1;
    }
    SetDynamicBegin(new_begin);
    SetDynamicSize(DynamicSize() + 1);
    std::construct_at(DynamicFront(), std::move(value));
  }

  constexpr void StaticClear() noexcept {
    StaticValidStateGuarantees();
    if constexpr (kStaticCapacity > 0) {
      while (!StaticEmpty()) {
        StaticPopBack();
      }
    }
  }
  constexpr void DynamicClear() noexcept {
    DynamicValidStateGuarantees();
    while (!DynamicEmpty()) {
      DynamicPopBack();
    }
  }

  constexpr void MoveToHeap(uint64_t new_capacity) {
    StaticValidStateGuarantees();
    Stored *new_storage = std::allocator<Stored>{}.allocate(new_capacity);
    uint64_t new_size = 0;
    if constexpr (kStaticCapacity > 0) {
      try {
        for (; 0 < StaticSize(); new_size++) {
          std::construct_at(&(new_storage[new_size]), std::move(StaticPopFrontValue()));
        }
      } catch (...) {
        for (size_t i = 0; i < new_size; i++) {
          std::destroy_at(&(new_storage[i]));
        }
        std::allocator<Stored>{}.deallocate(new_storage, new_capacity);
        throw;
      }
    }
    meta_ |= (1ull << 63);
    SetDynamicStorage(new_storage);
    SetDynamicCapacity(new_capacity);
    SetDynamicBegin(0);
    SetDynamicSize(new_size);
  }

  constexpr void ThrowIfEmpty() const {
    if (Empty()) [[unlikely]] {
      throw std::out_of_range("no element to pop");
    }
  }
};

static_assert(sizeof(CircleArr<int>) == 32);

template <class Stored>
template <bool Const>
class CircleArr<Stored>::Iterator {
 public:
  using ElementRefType = std::conditional_t<Const, const Stored, Stored>;
  using CArrRefType = std::conditional_t<Const, const CircleArr<Stored>, CircleArr<Stored>>;

  constexpr Iterator(CArrRefType &arr, uint64_t index) noexcept : arr_(arr), index_(index) {
  }

  constexpr ElementRefType &operator*() const noexcept {
    return arr_[index_];
  }
  constexpr ElementRefType &operator[](uint64_t shift) const noexcept {
    return arr_[index_ + shift];
  }

  constexpr Iterator &operator+=(int64_t shift) noexcept {
    if (shift < 0) {
      index_ -= -shift;
    } else {
      index_ += shift;
    }
    return *this;
  }
  constexpr Iterator &operator-=(int64_t shift) noexcept {
    return (*this) += (-shift);
  }
  constexpr Iterator operator+(int64_t shift) noexcept {
    auto copy{*this};
    copy += shift;
    return copy;
  }
  constexpr Iterator operator-(int64_t shift) noexcept {
    auto copy{*this};
    copy -= shift;
    return copy;
  }

  constexpr Iterator &operator++() noexcept {
    index_++;
    return *this;
  }
  constexpr Iterator operator++(int) noexcept {
    auto copy{*this};
    index_++;
    return copy;
  }
  constexpr Iterator &operator--() noexcept {
    index_--;
    return *this;
  }
  constexpr Iterator operator--(int) noexcept {
    auto copy{*this};
    index_--;
    return copy;
  }

  constexpr std::strong_ordering operator<=>(const Iterator<Const> &other) const noexcept {
    return index_ <=> other.index_;
  }
  constexpr bool operator==(const Iterator<Const> &other) const noexcept {
    return index_ == other.index_;
  }

 private:
  CArrRefType &arr_;
  uint64_t index_;
};