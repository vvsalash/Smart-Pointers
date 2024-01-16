#pragma once

// Paste here your implementation of compressed_pair from seminar 2 to use in UniquePtr
#include <type_traits>
#include <utility>

// Me think, why waste time write lot code, when few code do trick.
enum TypeOfPair { SAME_BOTH_EMPTY, BOTH_EMPTY, SAME_NO_EMPTY, NO_EMPTY, FIRST_EMPTY, SECOND_EMPTY };

template <typename F, typename S, bool is_first_empty, bool is_second_empty, bool is_the_same_type>
struct CompressedPairDeclaration;

template <typename F, typename S, int type>
class CompressedPairType;

template <typename F>
struct IsCompressed {
    static constexpr int kValue = std::is_empty_v<F> && !std::is_final_v<F>;
};

template <typename F, typename S>
struct CompressedPairDeclaration<F, S, true, true, true> {
    static constexpr int kValue = SAME_BOTH_EMPTY;
};

template <typename F, typename S>
struct CompressedPairDeclaration<F, S, true, true, false> {
    static constexpr int kValue = BOTH_EMPTY;
};

template <typename F, typename S>
struct CompressedPairDeclaration<F, S, false, false, true> {
    static constexpr int kValue = SAME_NO_EMPTY;
};

template <typename F, typename S>
struct CompressedPairDeclaration<F, S, false, false, false> {
    static constexpr int kValue = NO_EMPTY;
};

template <typename F, typename S>
struct CompressedPairDeclaration<F, S, true, false, false> {
    static constexpr int kValue = FIRST_EMPTY;
};

template <typename F, typename S>
struct CompressedPairDeclaration<F, S, false, true, false> {
    static constexpr int kValue = SECOND_EMPTY;
};

template <typename F, typename S>
class CompressedPairType<F, S, SAME_BOTH_EMPTY> : public F {
public:
    CompressedPairType() : first_(), F() {
    }

    template <typename First, typename Second>
    CompressedPairType(First&& first, Second&& second)
        : first_(std::forward<First>(first)), S(std::forward<Second>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return *this;
    }

    const S& GetSecond() const {
        return *this;
    }

private:
    F first_;
};

template <typename F, typename S>
class CompressedPairType<F, S, BOTH_EMPTY> : public F, public S {
public:
    CompressedPairType() : F(), S() {
    }

    template <typename First, typename Second>
    CompressedPairType(First&& first, Second&& second)
        : F(std::forward<First>(first)), S(std::forward<Second>(second)) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    }

    S& GetSecond() {
        return *this;
    }

    const S& GetSecond() const {
        return *this;
    }
};

template <typename F, typename S>
class CompressedPairType<F, S, SAME_NO_EMPTY> {
public:
    CompressedPairType() : first_(), second_() {
    }

    template <typename First, typename Second>
    CompressedPairType(First&& first, Second&& second)
        : first_(std::forward<First>(first)), second_(std::forward<Second>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return second_;
    }

    const S& GetSecond() const {
        return second_;
    }

private:
    F first_;
    S second_;
};

template <typename F, typename S>
class CompressedPairType<F, S, NO_EMPTY> {
public:
    CompressedPairType() : first_(), second_() {
    }

    template <typename First, typename Second>
    CompressedPairType(First&& first, Second&& second)
        : first_(std::forward<First>(first)), second_(std::forward<Second>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return second_;
    }

    const S& GetSecond() const {
        return second_;
    }

private:
    F first_;
    S second_;
};

template <typename F, typename S>
class CompressedPairType<F, S, FIRST_EMPTY> : public F {
public:
    CompressedPairType() : F(), second_() {
    }

    template <typename First, typename Second>
    CompressedPairType(First&& first, Second&& second)
        : F(std::forward<First>(first)), second_(std::forward<Second>(second)) {
    }

    F& GetFirst() {
        return *this;
    }

    const F& GetFirst() const {
        return *this;
    }

    S& GetSecond() {
        return second_;
    }

    const S& GetSecond() const {
        return second_;
    }

private:
    S second_;
};

template <typename F, typename S>
class CompressedPairType<F, S, SECOND_EMPTY> : public S {
public:
    CompressedPairType() : first_(), S() {
    }

    template <typename First, typename Second>
    CompressedPairType(First&& first, Second&& second)
        : first_(std::forward<First>(first)), S(std::forward<Second>(second)) {
    }

    F& GetFirst() {
        return first_;
    }

    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return *this;
    }

    const S& GetSecond() const {
        return *this;
    }

private:
    F first_;
};

template <typename F, typename S>
class CompressedPair
    : public CompressedPairType<F, S,
                                CompressedPairDeclaration<F, S, IsCompressed<F>::kValue,
                                                          IsCompressed<S>::kValue, false>::kValue> {
public:
    using EmptyBase =
        CompressedPairType<F, S,
                           CompressedPairDeclaration<F, S, IsCompressed<F>::kValue,
                                                     IsCompressed<S>::kValue, false>::kValue>;
    using EmptyBase::EmptyBase;
};

template <typename F>
class CompressedPair<F, F>
    : public CompressedPairType<F, F,
                                CompressedPairDeclaration<F, F, IsCompressed<F>::kValue,
                                                          IsCompressed<F>::kValue, true>::kValue> {
public:
    using EmptyBase =
        CompressedPairType<F, F,
                           CompressedPairDeclaration<F, F, IsCompressed<F>::kValue,
                                                     IsCompressed<F>::kValue, true>::kValue>;
    using EmptyBase::EmptyBase;
};