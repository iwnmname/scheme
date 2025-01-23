#pragma once

#include <memory>
#include <string>
#include <cstdint>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Boolean : public Object {
public:
    bool value_;

    Boolean(bool value) : value_(value) {
    }
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
    Cell() = default;

    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second) {
    }
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    };
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    };
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and conversion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (!dynamic_cast<T*>(obj.get())) {
        return false;
    }
    return true;
}
