#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class WeakPtr;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() {
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    SharedPtr(std::nullptr_t) {
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    explicit SharedPtr(T* ptr) {
        control_block_ = new ControlBlockPtr<T>(ptr);
        ptr_ = ptr;
    }

    template <typename Y>
    explicit SharedPtr(Y* ptr) {
        control_block_ = new ControlBlockPtr<Y>(ptr);
        ptr_ = ptr;
    }

    SharedPtr(const SharedPtr& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
    }

    SharedPtr(SharedPtr&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    explicit SharedPtr(ControlBlockObject<T>* ptr) {
        control_block_ = ptr;
        ptr_ = ptr->GetPointer();
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        control_block_ = other.control_block_;
        ptr_ = ptr;
        IncreaseCounter();
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }
        DecreaseCounter();
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }
        DecreaseCounter();
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        DecreaseCounter();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        DecreaseCounter();
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    void Reset(T* ptr) {
        DecreaseCounter();
        control_block_ = new ControlBlockPtr<T>(ptr);
        ptr_ = ptr;
    }

    template <typename Y>
    void Reset(Y* ptr) {
        DecreaseCounter();
        control_block_ = new ControlBlockPtr<Y>(ptr);
        ptr_ = ptr;
    }

    void Swap(SharedPtr& other) {
        std::swap(control_block_, other.control_block_);
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    size_t UseCount() const {
        if (control_block_ == nullptr) {
            return 0;
        }
        return control_block_->GetSharedCount();
    }
    size_t UseWeakCount() const {
        if (control_block_ == nullptr) {
            return 0;
        }
        return control_block_->GetWeakCount();
    }
    explicit operator bool() const {
        return ptr_ != nullptr;
    }

private:
    ControlBlock* control_block_;
    T* ptr_;

    void IncreaseCounter() {
        if (control_block_ == nullptr) {
            return;
        }
        control_block_->IncreaseSharedCounter();
    }

    void DecreaseCounter() {
        if (control_block_ == nullptr) {
            return;
        }
        control_block_->DecreaseSharedCounter();
        if (UseCount() == 0 && UseWeakCount() == 0) {
            delete control_block_;
        }
        control_block_ = nullptr;
        ptr_ = nullptr;
    }
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr<T>(new ControlBlockObject<T>(std::forward<Args>(args)...));
}

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis();
    SharedPtr<const T> SharedFromThis() const;

    WeakPtr<T> WeakFromThis() noexcept;
    WeakPtr<const T> WeakFromThis() const noexcept;
};
