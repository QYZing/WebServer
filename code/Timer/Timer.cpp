//
// Created by dying on 10/9/23.
//

#include "Timer.h"

bool dying::HeapTimerInstance::add(int id, int timeout, const dying::TimeoutCallBack &callback) {
    if(id < 0){
        throw std::exception();
    }
    size_t i;
    if(!m_ref.count(id)){ // 不存在结点
        i =m_heap.size();
        m_ref[id] = i;
        m_heap.emplace_back(std::make_shared<TimerNode>(id , callback , Clock::now() + MS(timeout)));
        adjustUp(i);
        return true;
    }
    i = m_ref[id];
    m_heap[i]->m_expires = Clock ::now() + MS(timeout);
    m_heap[i]->m_callback = callback;
    if(!adjustDown(i , m_heap.size())){
        adjustUp(i);
    }
    return true;
}

bool dying::HeapTimerInstance::del(int id) {
    if(!m_ref.count(id)){ // 不存在
        return true;
    }
    size_t index = m_ref[id];
    size_t n = m_heap.size() - 1;
    if(index < n){
        swapTimerNode(index , n);
        if(!adjustDown(index , n)){
            adjustUp(index);
        }
    }
    m_ref.erase(id);
    m_heap.pop_back();
    return true;
}

void dying::HeapTimerInstance::tick() {
    if(m_heap.empty()){
        return ;
    }
    while(!m_heap.empty()){
        auto node = m_heap.front();
        if(std::chrono::duration_cast<MS>(node->m_expires - Clock::now()).count() > 0){
            break;
        }
        LOG_DEBUG_DEFAULT << "-------------------tick----------------------"<<"pop fd = "<<node->m_id;
        pop();
        node->m_callback();
    }
}

void dying::HeapTimerInstance::clear() {
    m_heap.clear();
    m_ref.clear();
}

bool dying::HeapTimerInstance::adjust(int id, int timeout) {
    if(m_heap.empty() || !m_ref.count(id)){
        return false;
    }
    m_heap[m_ref[id]]->m_expires = Clock::now() + MS(timeout);
    adjustDown(m_ref[id] , m_heap.size());
    return true;
}

void dying::HeapTimerInstance::adjustUp(size_t i) {
    if(i < 0 || i >= m_heap.size()){
        throw std::exception();
    }
    if(i == 0) return ;
    size_t j = (i - 1) / 2;
    while(j >= 0 && i != 0){
        if(LessTimerNode(m_heap[j] , m_heap[i])){
            break;
        }
        swapTimerNode(i , j);
        i = j;
        j = (i - 1) / 2;
    }
}

bool dying::HeapTimerInstance::adjustDown(size_t index , size_t n) {
    if(index < 0 || index >= m_heap.size()){
        throw std::exception();
    }
    if(n < 0 || n > m_heap.size()){
        throw std::exception();
    }
    size_t i = index;
    size_t j = i * 2 + 1;
    while(j < n){
        if(j + 1 < n && LessTimerNode(m_heap[j + 1] , m_heap[j])){
            j++;
        }
        if(LessTimerNode(m_heap[i] , m_heap[j])){
            break;
        }
        swapTimerNode(i , j);
        i = j;
        j = i * 2 + 1;
    }
    return i > index;
}

void dying::HeapTimerInstance::swapTimerNode(size_t i, size_t j) {
    if(i < 0 || i >= m_heap.size()){
        throw std::exception();
    }
    if(j < 0 || j >= m_heap.size()){
        throw std::exception();
    }
    std::swap(m_heap[i] , m_heap[j]);
    m_ref[m_heap[i]->m_id] = i;
    m_ref[m_heap[j]->m_id] = j;
}

bool dying::HeapTimerInstance::LessTimerNode(const std::shared_ptr<TimerNode> & lhs, const std::shared_ptr<TimerNode> &rhs) {
    return lhs->operator<(*rhs);
}

void dying::HeapTimerInstance::pop() {
    if(m_heap.empty()){
        return ;
    }
    del(m_heap[0]->m_id);
}

bool dying::TimeWheel::add(int id, int timeout, const dying::TimeoutCallBack &callback) {
    if( timeout < 0 ){
        throw std::exception();
    }
    if(m_ref.count(id)){
        del(id);
    }

    int ticks = 0;//有多少基本单位 （时间轮的每一阁）
    if( timeout < SI ){
        ticks = 1;
    }
    else{
        ticks = timeout / SI;
    }
    int ts = ( cur_slot + ( ticks % N ) ) % N; // 应该放在哪个槽
    auto timerPtr = std::make_shared<TimeWheelNode>(id , callback ,
                                                    Clock::now() + MS(timeout)  , ts);
    // insert slot header
    m_slots[ts].push_front(timerPtr);
    m_ref[id] = m_slots[ts].begin();
    return true;
}

bool dying::TimeWheel::adjust(int id, int timeout) {
    if(!m_ref.count(id)){
        return false;
    }
    auto iterator = m_ref[id];
    auto ptr = *iterator;
    del(id);
    add(ptr->m_id , timeout , ptr->m_callback);
    return false;
}

bool dying::TimeWheel::del(int id) {
    if(!m_ref.count(id)){
        return true;
    }
    auto iterator = m_ref[id];
    int slot = (*iterator)->m_timeSlot;
    m_slots[slot].erase(iterator);
    m_ref.erase(id);
    return true;
}

void dying::TimeWheel::tick() {
    auto & list = m_slots[cur_slot];
    for(auto iterator = list.begin(); iterator != list.end();){
        auto ptr = *iterator++;
        if(ptr->m_expires <= Clock::now()){
            LOG_DEBUG_DEFAULT << "-------------------tick----------------------"<<"pop fd = "<<ptr->m_id;
            del(ptr->m_id);
            ptr->m_callback();
        }
    }
    cur_slot = ++cur_slot % N;
}

void dying::TimeWheel::clear() {
    m_ref.clear();
    for(auto & i : m_slots){
        i.clear();
    }
}
void dying::TimeWheel::setInfo(void * arg) {
    int * si = reinterpret_cast<int *>(arg);
    if(si != nullptr){
        auto & SI = const_cast<int &>(dying::TimeWheel::SI);
        SI = *si;
    }
}

dying::TimeWheel::TimeWheelNode::TimeWheelNode(int id, const dying::TimeoutCallBack &callBack,
                                               const dying::TimeStamp &expires , int time_slot)
                                               : TimerNode(id, callBack, expires)
                                               , m_timeSlot(time_slot){
}
