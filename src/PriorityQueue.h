// From https://github.com/Kruecke/cpu-astar/blob/master/src/PriorityQueue.h

#pragma once

#ifndef _DEBUG
#define NDEBUG
#endif

#include <algorithm>
#include <cassert>
#include <iterator>
#include <list>
#include <map>
#include <vector>

template <typename T, typename ValueCompare = std::less<T>, typename PrioCompare = std::less<T>>
class PriorityQueue {
private:
    template <typename Compare>
    struct Wrapper {
        Wrapper(Compare comp) : m_comp(comp) {}

        template <typename Iterator>
        bool operator()(const Iterator &a, const Iterator &b) {
            return m_comp(*a, *b);
        }

        Compare m_comp;
    };

    using DataIter = typename std::list<T>::iterator;
    using ConstDataIter = typename std::list<T>::const_iterator;

public:
    PriorityQueue(ValueCompare valueComp = ValueCompare(), PrioCompare prioComp = PrioCompare())
        : m_lookup(valueComp), m_prioComp(prioComp) {}

    const T &   top() const { return *m_heap.front(); }
    bool        empty() const { return m_data.empty(); }
    std::size_t size() const { return m_data.size(); }

    void push(const T &value) {
        m_data.push_back(value);
        const auto it = --m_data.end();
        m_lookup.emplace(value, it);
        m_heap.push_back(it);
        std::push_heap(m_heap.begin(), m_heap.end(), m_prioComp);
    }

    template <typename... Args>
    void emplace(Args &&... args) {
        m_data.emplace_back(std::forward<Args>(args)...);
        const auto it = --m_data.end();
        m_lookup.emplace(*it, it);
        m_heap.push_back(it);
        std::push_heap(m_heap.begin(), m_heap.end(), m_prioComp);
    }

    void pop() {
        const auto it = m_heap.front();
        std::pop_heap(m_heap.begin(), m_heap.end(), m_prioComp);
        m_heap.pop_back();

        assert(it != m_data.end());
        m_lookup.erase(*it);
        m_data.erase(it);
    }

    // --- These member functions don't exist in std::priority_queue. ----------

    // Look up value in priority queue.
    ConstDataIter find(const T &value) const {
        const auto it = m_lookup.find(value);
        return it != m_lookup.end() ? it->second : m_data.end();
    }

    // find(myValue) == end() means myValue is not in this priority queue.
    ConstDataIter end() const { return m_data.end(); }

    void update(const ConstDataIter &it, T newValue) {
        assert(it != m_data.end());

        // Only moving up the heap is supported!
        // FIXME: assert(???)

        // TODO: This function is still bad because we have to find the element in the priority
        // heap.
        const auto heapIt = std::find(m_heap.begin(), m_heap.end(), it);
        assert(heapIt != m_heap.end());

        **heapIt = std::move(newValue); // update value
        push_heap(m_heap.begin(), heapIt + 1, m_prioComp);

        assert(std::is_heap(m_heap.begin(), m_heap.end(), m_prioComp));
    }

private:
    std::list<T>                        m_data;
    std::map<T, DataIter, ValueCompare> m_lookup;
    std::vector<DataIter>               m_heap;
    Wrapper<PrioCompare>                m_prioComp;
};