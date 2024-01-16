#pragma once

#include <exception>
#include <memory>

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

class ControlBlock {
public:
    virtual ~ControlBlock() = default;

    void IncreaseSharedCounter() {
        ++shared_counter_;
    }

    void IncreaseWeakCounter() {
        ++weak_counter_;
    }

    void DecreaseSharedCounter() {
        --shared_counter_;
        if (shared_counter_ == 0) {
            Destroy();
        }
    }

    void DecreaseWeakCounter() {
        --weak_counter_;
    }

    size_t GetSharedCount() const {
        return shared_counter_;
    }

    size_t GetWeakCount() const {
        return weak_counter_;
    }

private:
    virtual void Destroy() = 0;

protected:
    size_t shared_counter_;
    size_t weak_counter_;
};

template <typename T>
class ControlBlockPtr : public ControlBlock {
public:
    ControlBlockPtr(T* ptr) {
        ptr_ = ptr;
        shared_counter_ = 1;
        weak_counter_ = 0;
    }

    ControlBlockPtr(const ControlBlockPtr& other) {
        ptr_ = other.ptr_;
        shared_counter_ = other.shared_counter_;
    }

    template <typename F>
    ControlBlockPtr(const ControlBlockPtr<F>& other) {
        ptr_ = other.ptr_;
        shared_counter_ = other.shared_counter_;
    }

    ControlBlockPtr& operator=(const ControlBlockPtr& other) {
        if (this == &other) {
            return *this;
        }
        ptr_ = other.ptr_;
        shared_counter_ = other.shared_counter_;
        return *this;
    }

    template <typename F>
    ControlBlockPtr& operator=(const ControlBlockPtr<F>& other) {
        if (this == &other) {
            return *this;
        }
        ptr_ = other.ptr_;
        shared_counter_ = other.shared_counter_;
        return *this;
    }

private:
    T* ptr_;

    void Destroy() override {
        delete ptr_;
    }
};

template <typename T>
class ControlBlockObject : public ControlBlock {
public:
    template <typename... Args>
    ControlBlockObject(Args&&... args) {
        ::new (&ptr_) T(std::forward<Args>(args)...);
        shared_counter_ = 1;
        weak_counter_ = 0;
    }

    T* GetPointer() {
        return reinterpret_cast<T*>(&ptr_);
    }

private:
    std::aligned_storage_t<sizeof(T), alignof(T)> ptr_;

    void Destroy() override {
        std::destroy_at(std::launder(reinterpret_cast<T*>(&ptr_)));
    }
};