#pragma once

#include <memory>
#include <string>
#include <cstdint>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int64_t other) : num_(other) {
    }
    int GetValue() const {
        return num_;
    };
    int64_t num_;
};

class Symbol : public Object {
public:
    Symbol(const std::string& other) : name_(other) {
    }
    const std::string& GetName() const {
        return name_;
    };
    std::string name_;
};

class Cell : public Object {
public:
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    };
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    };
    std::shared_ptr<Object> first_ = nullptr;
    std::shared_ptr<Object> second_ = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and conversion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
};

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto x = dynamic_cast<T*>(obj.get());
    if (!x) {
        return false;
    }
    return true;
};
