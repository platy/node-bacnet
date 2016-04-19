#pragma once

#include <uv.h>
#include <queue>
#include <iostream>

using std::cout;
using std::endl;

template <typename T>
class NonBlockingQueue
{
private:
    uv_mutex_t              mutex_handle; // TODO create C++ class
    std::queue<T>           d_queue;
public:
    NonBlockingQueue() {
        uv_mutex_init(&mutex_handle);
    }
    void push(T const& value) {
        uv_mutex_lock(&mutex_handle);
        d_queue.push(value);
        uv_mutex_unlock(&mutex_handle);
    }
    // Threadsafe, return false if the queue is empty
    bool pop(T* out) {
        uv_mutex_lock(&mutex_handle);
        if (this->d_queue.empty()) {
            uv_mutex_unlock(&mutex_handle);
            return false;
        }
        *out = this->d_queue.front();
        this->d_queue.pop();
        uv_mutex_unlock(&mutex_handle);
        return true;
    }
    int size() {
        return d_queue.size();
    }
};
