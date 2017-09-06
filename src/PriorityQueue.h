#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

template <typename T, typename Compare = std::less<T>>
class PriorityQueue {
public:
	PriorityQueue(Compare compare = Compare()) : m_compare(std::move(compare)) {} // ?? Compare()

	const T& top() const { // Const function?
		return m_heap.front(); 
	}
	bool empty() const { return m_heap.empty(); }
	auto size() const { return m_heap.size(); }

	void push(const T &value) {
		m_heap.push_back(value);
		std::push_heap(m_heap.begin(), m_heap.end(), m_compare);
	}

	template <typename... Args>
	void emplace(Args &&... args) {
		m_heap.emplace_back(std::forward<Args>(args)...);
		std::push_heap(m_heap.begin(), m_heap.end(), m_compare);
	}

	void pop() {
		std::pop_heap(m_heap.begin(), m_heap.end(), m_compare);
		m_heap.pop_back();
	}

	// Extra functionality that is not in std::priority_queue
	const T &operator[](std::size_t index) const {
		assert(index >= 0 && index < m_heap.size()); // Is this slow?
		return m_heap[index];
	}

	// Return index of a value for which predicate p returns true.
	template <typename UnaryPredicate>
	std::size_t find_if(UnaryPredicate p) const { // Faster alternative?
		// This a basicly a breadth-first search, although not quite optimal. We could cancel the
		// search earlier if we'd give up a bit of generality here.
		auto it = std::find_if(m_heap.begin(), m_heap.end(), p);

		return std::distance(m_heap.begin(), it);
	}

	void update(std::size_t index, T newValue) {
		assert(index >= 0 && index < m_heap.size());

		// Only moving up the heap is supported!
		// FIXME: assert(???)

		m_heap[index] = std::move(newValue);
		std::push_heap(m_heap.begin(), std::next(m_heap.begin(), index + 1), m_compare);

		assert(std::is_heap(m_heap.begin(), m_heap.end(), m_compare)); // Necessary?
	}

private:
	std::vector<T> m_heap;
	Compare m_compare;
};

