//
// Created by brian on 12/2/23.
//

#ifndef HOUND_LOCK_FREE_QUEUE_H_HPP
#define HOUND_LOCK_FREE_QUEUE_H_HPP
#include <atomic>
#include <memory>

namespace hd::entity {

template <typename T>
class LockFreeQueue {
private:
  struct Node {
    T data;
    std::unique_ptr<Node> next;

    explicit Node(T value) : data(std::move(value)), next(nullptr) {}
  };

  std::atomic<Node*> head;
  std::atomic<Node*> tail;
  std::atomic<size_t> element_count;

public:
  LockFreeQueue() : head(new Node(T())), tail(head.load(std::memory_order_relaxed)), element_count(0) {}

  ~LockFreeQueue() {
    while (Node* old_head = head.load(std::memory_order_relaxed)) {
      head.store(old_head->next.get(), std::memory_order_relaxed);
      delete old_head;
    }
  }

  void push(T value) {
    std::unique_ptr<Node> new_node = std::make_unique<Node>(std::move(value));
    Node* old_tail = tail.exchange(new_node.get(), std::memory_order_acq_rel);
    old_tail->next = std::move(new_node);

    element_count.fetch_add(1, std::memory_order_relaxed);
  }

  std::shared_ptr<T> pop() {
    Node* old_head = head.load(std::memory_order_acquire);
    if (old_head == tail.load(std::memory_order_acquire)) {
      return nullptr; // 队列为空
    }

    head.store(old_head->next.get(), std::memory_order_release);
    std::shared_ptr<T> result = std::make_shared<T>(std::move(old_head->data));
    delete old_head;

    element_count.fetch_sub(1, std::memory_order_relaxed);

    return result;
  }

  size_t count() const {
    return element_count.load(std::memory_order_relaxed);
  }

  bool empty() const {
    return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
  }
};

} // namespace hd::entity



#endif //HOUND_LOCK_FREE_QUEUE_H_HPP
