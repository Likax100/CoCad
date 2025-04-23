#pragma once

#include "CoCad_Common.h"

// note: most of this is essentially just wrapping regular deque (double ended queue) functions to ensure thread safety
// AKA, only one piece of code can access and work with the queue at a time
namespace CoCadNet
{
	template<typename T>
	class SafeQueue {
	public:
		SafeQueue() = default; // set default constructor
		SafeQueue(const SafeQueue<T>&) = delete; // ensures we cant copy it so its more thread safe
		virtual ~SafeQueue() { clear(); } 

		const T& front() { std::scoped_lock lock(mutex_q); return deque_q.front(); }

		const T& back() { std::scoped_lock lock(mutex_q); return deque_q.back(); }
		
		bool empty() { std::scoped_lock lock(mutex_q); return deque_q.empty(); }

		unsigned int size() { std::scoped_lock lock(mutex_q); return deque_q.size(); }

		void clear() { std::scoped_lock lock(mutex_q); deque_q.clear(); }

		void wait() {
			while (empty()) { std::unique_lock<std::mutex> ul(m_blocking); cv_blocking.wait(ul); }
		}

		T pop_front() {
			std::scoped_lock lock(mutex_q);
			auto t = std::move(deque_q.front());
			deque_q.pop_front();
			return t;
		}

		T pop_back() {
			std::scoped_lock lock(mutex_q);
			auto t = std::move(deque_q.back());
			deque_q.pop_back();
			return t;
		}

		void push_back(const T& item) {
			std::scoped_lock lock(mutex_q);
			deque_q.emplace_back(std::move(item));
			std::unique_lock<std::mutex> ul(m_blocking);
			cv_blocking.notify_one();
		}

		void push_front(const T& item) {
			std::scoped_lock lock(mutex_q);
			deque_q.emplace_front(std::move(item));
			std::unique_lock<std::mutex> ul(m_blocking);
			cv_blocking.notify_one();
		}

	protected:
		std::mutex mutex_q; // mutex = mutually exclusive queue
		std::deque<T> deque_q; 
		std::condition_variable cv_blocking;
		std::mutex m_blocking;
	};
}
