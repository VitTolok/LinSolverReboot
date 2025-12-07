module;

#include <stdexcept>
#include <utility>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <initializer_list>
#include <memory>
#include <iterator>
#include <compare>

export module CircleArr;

export template <class Stored>
  requires std::same_as<Stored, std::decay_t<Stored>>
class CircleArr {
 public:
  using SizeT = std::size_t;
  constexpr CircleArr() noexcept : storage_(nullptr), capacity_(0uz), begin_(0uz), size_(0uz) {
  }
  constexpr explicit CircleArr(SizeT capacity) : CircleArr() {
    Reserve(capacity);
  }
  constexpr CircleArr(SizeT capacity, const Stored &value) : CircleArr(capacity) {
    for (SizeT i = 0; i < capacity_; i++) {
      PushBack(value);
    }
  }
  constexpr CircleArr(SizeT capacity, Stored &&value) : CircleArr(capacity) {
    for (SizeT i = 0; i + 1 < capacity_; i++) {
      PushBack(value);
    }
    if (capacity_ > 0) [[likely]] {
      PushBack(std::move(value));
    }
  }
  constexpr CircleArr(std::initializer_list<Stored> init) : CircleArr(init.size()) {
    for (const auto &value : init) {
      PushBack(value);
    }
  }

  template <std::forward_iterator Iter>
  constexpr CircleArr(Iter begin, const Iter &end) : CircleArr() {
    while (begin != end) {
      PushBack(*begin);
      begin++;
    }
  }

  constexpr CircleArr(const CircleArr &other) : CircleArr(other.size_) {
    for (SizeT i = 0; i < other.size_; i++) {
      PushBack(other[i]);
    }
  }
  constexpr CircleArr(CircleArr &&other) noexcept
      : storage_(other.storage_), capacity_(other.capacity_), begin_(other.begin_), size_(other.size_) {
    other.storage_ = nullptr;
    other.capacity_ = 0;
    other.begin_ = 0;
    other.size_ = 0;
  }

  constexpr CircleArr &operator=(const CircleArr &other) {
    ValidStateGuarantees();
    if (this == &other) [[unlikely]] {
      return *this;
    }
    Clear();
    Reserve(other.size_);
    for (SizeT i = 0; i < other.size_; i++) {
      PushBack(other[i]);
    }
    return *this;
  }
  constexpr CircleArr &operator=(CircleArr &&other) noexcept {
    ValidStateGuarantees();
    if (this == &other) [[unlikely]] {
      return *this;
    }
    Deallocate();
    storage_ = std::exchange(other.storage_, nullptr);
    capacity_ = std::exchange(other.capacity_, 0);
    begin_ = std::exchange(other.begin_, 0);
    size_ = std::exchange(other.size_, 0);
    return *this;
  }

  constexpr Stored &operator[](SizeT pos) noexcept {
    CapacityGuarantees();
    if (pos < capacity_) {
      if (begin_ + pos < capacity_) {
        return storage_[begin_ + pos];
      }
      return storage_[begin_ + pos - capacity_];
    }
    std::unreachable();
  }
  constexpr const Stored &operator[](SizeT pos) const noexcept {
    CapacityGuarantees();
    if (pos < capacity_) {
      if (begin_ + pos < capacity_) {
        return storage_[begin_ + pos];
      }
      return storage_[begin_ + pos - capacity_];
    }
    std::unreachable();
  }

  constexpr Stored &At(SizeT pos) {
    ValidStateGuarantees();
    if (pos >= size_) {
      throw std::out_of_range("position out of range");
    }
    return (*this)[pos];
  }
  constexpr const Stored &At(SizeT pos) const {
    ValidStateGuarantees();
    if (pos >= size_) {
      throw std::out_of_range("position out of range");
    }
    return (*this)[pos];
  }

  constexpr Stored &Front() noexcept {
    NonEmptyGuarantees();
    return (*this)[0];
  }
  constexpr const Stored &Front() const noexcept {
    NonEmptyGuarantees();
    return (*this)[0];
  }
  constexpr Stored &Back() noexcept {
    NonEmptyGuarantees();
    return (*this)[size_ - 1];
  }
  constexpr const Stored &Back() const noexcept {
    NonEmptyGuarantees();
    return (*this)[size_ - 1];
  }

  constexpr CircleArr &PushBack(const Stored &value) {
    ValidStateGuarantees();
    ExpandIfFull();
    std::construct_at(&((*this)[size_]), value);
    size_++;
    return *this;
  }
  constexpr CircleArr &PushBack(Stored &&value) {
    ValidStateGuarantees();
    ExpandIfFull();
    std::construct_at(&((*this)[size_]), std::move(value));
    size_++;
    return *this;
  }
  template <class... Args>
  constexpr CircleArr &EmplaceBack(Args &&...args) {
    ValidStateGuarantees();
    ExpandIfFull();
    std::construct_at(&((*this)[size_]), std::forward<Args>(args)...);
    size_++;
    return *this;
  }
  constexpr CircleArr &PushFront(const Stored &value) {
    ValidStateGuarantees();
    ExpandIfFull();
    std::construct_at(&((*this)[capacity_ - 1]), value);
    begin_ = begin_ == 0 ? capacity_ - 1 : begin_ - 1;
    size_++;
    return *this;
  }
  constexpr CircleArr &PushFront(Stored &&value) {
    ValidStateGuarantees();
    ExpandIfFull();
    std::construct_at(&((*this)[capacity_ - 1]), std::move(value));
    begin_ = begin_ == 0 ? capacity_ - 1 : begin_ - 1;
    size_++;
    return *this;
  }
  template <class... Args>
  constexpr CircleArr &EmplaceFront(Args &&...args) {
    ValidStateGuarantees();
    ExpandIfFull();
    std::construct_at(&((*this)[capacity_ - 1]), std::forward<Args>(args)...);
    begin_ = begin_ == 0 ? capacity_ - 1 : begin_ - 1;
    size_++;
    return *this;
  }

  constexpr CircleArr &PopBack() noexcept {
    NonEmptyGuarantees();
    if constexpr (!std::is_trivially_destructible_v<Stored>) {
      std::destroy_at(&Back());
    }
    size_--;
    return *this;
  }
  constexpr CircleArr &PopBackSafe() {
    ValidStateGuarantees();
    if (Empty()) [[unlikely]] {
      throw std::out_of_range("array empty");
    }
    return PopBack();
  }
  constexpr Stored PopBackValue() noexcept {
    NonEmptyGuarantees();
    Stored value = std::move(Back());
    PopBack();
    return value;
  }
  constexpr Stored PopBackValueSafe() {
    ValidStateGuarantees();
    if (Empty()) [[unlikely]] {
      throw std::out_of_range("array empty");
    }
    return PopBackValue();
  }

  constexpr CircleArr &PopFront() noexcept {
    NonEmptyGuarantees();
    if constexpr (!std::is_trivially_destructible_v<Stored>) {
      std::destroy_at(&Front());
    }
    begin_ = begin_ + 1 == capacity_ ? 0 : begin_ + 1;
    size_--;
    return *this;
  }
  constexpr CircleArr &PopFrontSafe() {
    ValidStateGuarantees();
    if (Empty()) [[unlikely]] {
      throw std::out_of_range("array empty");
    }
    return PopFront();
  }
  constexpr Stored PopFrontValue() noexcept {
    NonEmptyGuarantees();
    Stored value = std::move(Front());
    PopFront();
    return value;
  }
  constexpr Stored PopFrontValueSafe() {
    ValidStateGuarantees();
    if (Empty()) [[unlikely]] {
      throw std::out_of_range("array empty");
    }
    return PopFrontValue();
  }

  constexpr bool Empty() const noexcept {
    return size_ == 0;
  }
  constexpr SizeT Size() const noexcept {
    return size_;
  }
  constexpr SizeT Capacity() const noexcept {
    return capacity_;
  }

  constexpr CircleArr &Reserve(SizeT new_capacity) {
    ValidStateGuarantees();
    if (new_capacity <= capacity_) {
      return *this;
    }
    Stored *new_storage = std::allocator<Stored>{}.allocate(new_capacity);
    SizeT size = size_;
    try {
      for (SizeT i = 0; i < size; i++) {
        std::construct_at(new_storage + i, PopFrontValue());
      }
    } catch (...) {
      std::allocator<Stored>{}.deallocate(new_storage, new_capacity);
      throw;
    }
    Deallocate();
    storage_ = new_storage;
    capacity_ = new_capacity;
    begin_ = 0;
    size_ = size;
    return *this;
  }

  constexpr CircleArr &Clear() noexcept {
    ValidStateGuarantees();
    if constexpr (!std::is_trivially_destructible_v<Stored>) {
      while (!Empty()) {
        PopBack();
      }
    } else {
      size_ = 0;
    }
    begin_ = 0;
    return *this;
  }
  constexpr CircleArr &Deallocate() noexcept {
    ValidStateGuarantees();
    Clear();
    if (storage_ != nullptr) {
      std::allocator<Stored>{}.deallocate(storage_, capacity_);
    }
    storage_ = nullptr;
    capacity_ = 0;
    return *this;
  }

  constexpr ~CircleArr() noexcept {
    Deallocate();
  }

  template <class ArrayRef, class ElementRef>
  class Iterator {
   public:
    using PtrDiffT = std::ptrdiff_t;
    friend class CircleArr<Stored>;
    constexpr Iterator() noexcept : arr_(nullptr), pos_(0) {
    }
    constexpr Iterator(const Iterator &other) = default;
    constexpr Iterator(Iterator &&other) = default;
    constexpr Iterator &operator=(const Iterator &other) = default;
    constexpr Iterator &operator=(Iterator &&other) = default;
    constexpr ~Iterator() = default;

    constexpr Iterator &operator++() noexcept {
      pos_++;
      return *this;
    }
    constexpr Iterator operator++(int) noexcept {
      auto copy = *this;
      pos_++;
      return copy;
    }
    constexpr Iterator &operator--() noexcept {
      pos_--;
      return *this;
    }
    constexpr Iterator operator--(int) noexcept {
      auto copy = *this;
      pos_--;
      return copy;
    }

    constexpr auto operator<=>(const Iterator &other) const noexcept -> std::strong_ordering {
      return pos_ <=> other.pos_;
    }
    constexpr bool operator==(const Iterator &other) const noexcept {
      return pos_ == other.pos_;
    }

    constexpr ElementRef &operator*() const noexcept {
      return (*arr_)[static_cast<SizeT>(pos_)];
    }
    constexpr ElementRef *operator->() const noexcept {
      return &((*arr_)[static_cast<SizeT>(pos_)]);
    }

    constexpr Iterator &operator+=(PtrDiffT shift) noexcept {
      pos_ += shift;
      return *this;
    }
    constexpr Iterator &operator-=(PtrDiffT shift) noexcept {
      pos_ -= shift;
      return *this;
    }

    constexpr Iterator operator+(PtrDiffT shift) const noexcept {
      auto copy = *this;
      copy += shift;
      return copy;
    }
    constexpr Iterator operator-(PtrDiffT shift) const noexcept {
      auto copy = *this;
      copy -= shift;
      return copy;
    }
    friend constexpr Iterator operator+(PtrDiffT shift, const Iterator &iter) noexcept {
      return iter + shift;
    }

    constexpr ElementRef &operator[](PtrDiffT shift) const noexcept {
      return *(*this + shift);
    }

    constexpr PtrDiffT operator-(const Iterator &other) const noexcept {
      return pos_ - other.pos_;
    }

   private:
    ArrayRef *arr_;
    PtrDiffT pos_;
    constexpr Iterator(ArrayRef &arr, PtrDiffT pos) noexcept : arr_(&arr), pos_(pos) {
    }
  };

  using RAIterator = Iterator<CircleArr<Stored>, Stored>;
  using ConstIterator = Iterator<const CircleArr<Stored>, const Stored>;

  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr RAIterator begin() noexcept {
    return RAIterator(*this, 0);
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr RAIterator end() noexcept {
    return RAIterator(*this, static_cast<std::ptrdiff_t>(size_));
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr RAIterator begin() const noexcept {
    return cbegin();
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr RAIterator end() const noexcept {
    return cend();
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr ConstIterator cbegin() const noexcept {
    return ConstIterator(*this, 0);
  }
  // NOLINTNEXTLINE(readability-identifier-naming)
  constexpr ConstIterator cend() noexcept {
    return ConstIterator(*this, static_cast<std::ptrdiff_t>(size_));
  }

 private:
  Stored *storage_;
  SizeT capacity_;
  SizeT begin_;
  SizeT size_;

  constexpr void ExpandIfFull() {
    ValidStateGuarantees();
    if (capacity_ == 0) {
      Reserve(8);
      return;
    }
    if (size_ == capacity_) {
      Reserve(2 * capacity_);
    }
  }

  constexpr void ValidStateGuarantees() const noexcept {
    if ((begin_ >= capacity_ && capacity_ > 0) || size_ > capacity_ || ((capacity_ == 0) != (storage_ == nullptr))) {
      std::unreachable();
    }
  }
  constexpr void CapacityGuarantees() const noexcept {
    ValidStateGuarantees();
    if (capacity_ == 0) {
      std::unreachable();
    }
  }
  constexpr void NonEmptyGuarantees() const noexcept {
    CapacityGuarantees();
    if (size_ == 0) {
      std::unreachable();
    }
  }
};