#pragma once

#include <uv.h>

// An acquired lock from a Mutex, the Mutex can't be locked again until this is destroyed
class ScopeGuard
{
private:
    uv_mutex_t*            acquired_lock;
public:
    ScopeGuard(uv_mutex_t  *mutex_handle) {
        uv_mutex_lock(mutex_handle);
        acquired_lock = mutex_handle;
    }
    ~ScopeGuard() {
        uv_mutex_unlock(acquired_lock);
    }
};

// Wraps uv_mutex_t, guard_scope() takes a lock and releases it when the returned ScopeGuard is destroyed
class Mutex
{
private:
    uv_mutex_t              mutex_handle;
public:
    Mutex() {
        uv_mutex_init(&mutex_handle);
    }
    ~Mutex() {
        uv_mutex_destroy(&mutex_handle);
    }
    ScopeGuard guard_scope() {
        return ScopeGuard(&mutex_handle);
    }
};
