#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <optional>
// Thread-safe queue
// https://www.geeksforgeeks.org/implement-thread-safe-queue-in-c/
// Definitely do go into the rabbit hole of concurrent queues including lock-free, if time permits, did not for me.
template <typename T>
class SafeQueue {
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool                       closed = false;
public:
    void push(T item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_cond.notify_one();
    }

    std::optional<T> pop() {
        std::unique_lock lock(m_mutex);
        m_cond.wait(lock, [&]{ return !m_queue.empty() || closed; });

        if (m_queue.empty()) {
            return std::nullopt;
        }

        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }

    void close() { {
            std::lock_guard lock(m_mutex);
            closed = true;
        }
        m_cond.notify_all();
    }

};
#endif //SAFEQUEUE_H
