#include "VMCondition.h"

#include <chrono>

const long VMCondition::VMConditionNumberOfFields = 0;

void VMCondition::SignalOne() {
    cond_var->notify_one();
}

void VMCondition::SignalAll() {
    cond_var->notify_all();
}


void VMCondition::Await() {
    cond_var->wait(*lock);
}

bool VMCondition::Await(int64_t timeoutMilliseconds) {
    return cv_status::no_timeout == cond_var->wait_for(
                    *lock, std::chrono::milliseconds(timeoutMilliseconds));
}

StdString VMCondition::AsDebugString() const {
    return "VMCondition";
}

VMCondition* VMCondition::Clone() const {
    VMCondition* clone = new (GetHeap<HEAP_CLS>(), 0 ALLOC_MATURE)
                            VMCondition(lock, cond_var);
    clone->clazz = clazz;
    return clone;
}

void VMCondition::MarkObjectAsInvalid() {
    clazz = (GCClass*) INVALID_GC_POINTER;
    std::unique_lock<recursive_mutex>** lock_for_reset = const_cast<std::unique_lock<recursive_mutex>**>(&lock);
    *lock_for_reset = nullptr;
    
    std::condition_variable_any** cv_for_reset = const_cast<std::condition_variable_any**>(&cond_var);
    *cv_for_reset = nullptr;
}