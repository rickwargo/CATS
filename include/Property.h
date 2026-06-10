#pragma once

#include <iostream>
#include <functional>

template<typename T, typename ClassType>
class Property {
public:
    Property(ClassType* inst, T (ClassType::*getFn)() const, void (ClassType::*setFn)(const T))
        : instance(inst), getter(getFn), setter(setFn) {}

    // Getter: Implicit conversion acts as the getter
    operator T() const {
        return (instance->*getter)();
    }

    // Setter: Overloaded assignment operator (e.g., obj.prop = value)
    Property& operator=(const T value) {
        (instance->*setter)(value);
        return *this;
    }

private:
    const ClassType* instance;
    T (ClassType::*getter)() const;
    void (ClassType::*setter)(const T);

};
