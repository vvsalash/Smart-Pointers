#pragma once

#include "sw_fwd.h"  // Forward declaration

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
    template <typename Y>
    friend class SharedPtr;

    template <typename Y>
    friend class WeakPtr;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() {
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    WeakPtr(const WeakPtr& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
    }

    WeakPtr(WeakPtr&& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        other.control_block_ = nullptr;
        other.ptr_ = nullptr;
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) {
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (this == &other) {
            return *this;
        }
        DecreaseCounter();
        control_block_ = other.control_block_;
        ptr_ = other.ptr_;
        IncreaseCounter();
        return *this;
    }
    WeakPtr& operator=(WeakPtr&& other) {
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

    ~WeakPtr() {
        DecreaseCounter();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        DecreaseCounter();
        control_block_ = nullptr;
        ptr_ = nullptr;
    }

    void Swap(WeakPtr& other) {
        std::swap(control_block_, other.control_block_);
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

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

    bool Expired() const {
        return UseCount() == 0;
    }
    SharedPtr<T> Lock() const {
        if (Expired()) {
            return SharedPtr<T>();
        } else {
            return SharedPtr<T>(*this);
        }
    }

private:
    ControlBlock* control_block_;
    T* ptr_;

    void IncreaseCounter() {
        if (control_block_ == nullptr) {
            return;
        }
        control_block_->IncreaseWeakCounter();
    }

    void DecreaseCounter() {
        if (control_block_ == nullptr) {
            return;
        }
        control_block_->DecreaseWeakCounter();
        if (UseCount() == 0 && UseWeakCount() == 0) {
            delete control_block_;
        }
        control_block_ = nullptr;
        ptr_ = nullptr;
    }
};
