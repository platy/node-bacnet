#pragma once

#include <queue>
#include <iostream>
#include "Mutex.h"

template <typename T>
class NonBlockingQueue
{
private:
    Mutex                   mutex;
    std::queue<T>           d_queue;
public:
    void push(const T& value) {
        ScopeGuard guard = mutex.guard_scope();
        d_queue.push(value);
    }
    // Threadsafe, return false if the queue is empty
    bool pop(T* out) {
        ScopeGuard guard = mutex.guard_scope();
        if (this->d_queue.empty()) {
            return false;
        }
        *out = this->d_queue.front();
        this->d_queue.pop();
        return true;
    }
    int size() {
        ScopeGuard guard = mutex.guard_scope();
        return d_queue.size();
    }
};
