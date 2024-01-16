#pragma once

#include "compressed_pair.h"
#include "deleters.h"

#include <cstddef>  // std::nullptr_t

template <typename T>
struct DefaultDeleter {
    DefaultDeleter() = default;

    template <class F>
    DefaultDeleter(DefaultDeleter<F>&&) noexcept {
    }

    ~DefaultDeleter() = default;

    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0);
        static_assert(!std::is_void<T>::value);
        delete ptr;
    }
};

// Primary template
template <typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) {
        data_.GetFirst() = ptr;
    }

    UniquePtr(T* ptr, Deleter deleter) {
        data_.GetFirst() = ptr;
        data_.GetSecond() = std::forward<Deleter>(deleter);
    }

    template <typename F, typename S>
    UniquePtr(UniquePtr<F, S>&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::forward<S>(other.GetDeleter());
    }

    UniquePtr(UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <typename F, typename S>
    UniquePtr& operator=(UniquePtr<F, S>&& other) noexcept {
        Reset(other.Release());
        data_.GetSecond() = std::forward<S>(other.GetDeleter());
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }
    UniquePtr& operator=(UniquePtr& other) = delete;
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* released = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return released;
    }

    void Reset(T* ptr = nullptr) {
        T* old_ptr = data_.GetFirst();
        data_.GetFirst() = ptr;
        if (old_ptr != nullptr) {
            GetDeleter()(old_ptr);
        }
    }

    void Swap(UniquePtr& other) {
        std::swap(data_, other.data_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return data_.GetFirst() != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *(data_.GetFirst());
    }
    T* operator->() const {
        return data_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> data_;
};

template <typename T>
struct DefaultDeleter<T[]> {
    DefaultDeleter() = default;

    template <class F>
    DefaultDeleter(DefaultDeleter<F>&&) noexcept {
    }

    ~DefaultDeleter() = default;

    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0);
        static_assert(!std::is_void<T>::value);
        delete[] ptr;
    }
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) {
        data_.GetFirst() = ptr;
    }

    UniquePtr(T* ptr, Deleter deleter) {
        data_.GetFirst() = ptr;
        data_.GetSecond() = std::forward<Deleter>(deleter);
    }

    template <typename F, typename S>
    UniquePtr(UniquePtr<F, S>&& other) noexcept {
        data_.GetFirst() = other.Release();
        data_.GetSecond() = std::forward<S>(other.GetDeleter());
    }

    UniquePtr(UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        Reset(other.Release());
        data_.GetSecond() = std::move(other.GetDeleter());
        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) {
        Reset();
        return *this;
    }
    UniquePtr& operator=(UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* released = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return released;
    }

    void Reset(T* ptr = nullptr) {
        T* old_ptr = data_.GetFirst();
        data_.GetFirst() = ptr;
        if (old_ptr != nullptr) {
            GetDeleter()(old_ptr);
        }
    }

    void Swap(UniquePtr& other) {
        std::swap(data_, other.data_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return data_.GetFirst() != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *(data_.GetFirst());
    }
    T* operator->() const {
        return data_.GetFirst();
    }

    T& operator[](size_t index) {
        return data_.GetFirst()[index];
    }

    T& operator[](size_t index) const {
        return data_.GetFirst()[index];
    }

private:
    CompressedPair<T*, Deleter> data_;
};
