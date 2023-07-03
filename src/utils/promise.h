#ifndef EASY_THREAD_PROMISE_H
#define EASY_THREAD_PROMISE_H

#include <utils/thread.h>
#include <iostream>

namespace Easy {

class BrokenPromise : public std::exception {};
class NoPromiseValue : public std::runtime_error {
 public:
  NoPromiseValue()
      : std::runtime_error("Promise Object Has No Value Prepared") {}
};

class DestroyerOfQueues {
 public:
  DestroyerOfQueues(const std::shared_ptr<ThreadQueue>& queue);

  void dispatch(const ThreadClosure& closure);

  ~DestroyerOfQueues();

 private:
  std::shared_ptr<ThreadQueue> queue;
};

template <typename T>
using ResultBlock = std::function<T()>;
template <typename T, typename U>
using ResultWithArgBlock = std::function<T(const U&)>;

template <typename ReturnType>
class Promise;

template <typename T>
using PromiseClosure = std::function<void(Promise<T>)>;

template <typename ReturnType>
class Promise {
 public:
  Promise(const PromiseClosure<ReturnType>& closure) {
    queue = std::make_shared<DestroyerOfQueues>(
        Threading::getQueue(Threading::Serial));
    queue->dispatch([=] {
      try {
        closure(*this);
      } catch (std::exception& e) {
        fail(e);
      }
    });
  }

  Promise(const std::function<ReturnType()>& closure) {
    queue = std::make_shared<DestroyerOfQueues>(
        Threading::getQueue(Threading::Serial));
    queue->dispatch([=] {
      try {
        set(closure());
      } catch (std::exception& e) {
        fail(e);
      }
    });
  }

  template <typename LastType>
  Promise(Promise<LastType>& from,
          const ResultWithArgBlock<ReturnType, LastType>& closure) {
    queue = from.queue;
    queue->dispatch([=, &from] {
      try {
        auto last = from.wait();
        auto value = closure(last);
        set(value);
      } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        fail(e);
      }
    });
  }

  ReturnType get() {
    event.lock();
    defer([=] { event.unlock(); });

    if (!error.empty()) {
      throw std::runtime_error(error.c_str());
    }
    if (value) {
      return *value;
    }

    throw NoPromiseValue();
  }

  void set(const ReturnType& v) {
    event.lock();
    defer([=] { event.unlock(); });
    value = std::make_shared<ReturnType>(v);
    event.broadcast();
  }

  void fail(const std::exception& e) {
    event.lock();
    defer([=] { event.unlock(); });
    error = e.what();
    event.broadcast();
  }

  template <typename NextType>
  Promise<NextType> then(
      const ResultWithArgBlock<NextType, ReturnType>& closure) {
    return Promise<NextType>(*this, closure);
  }

  void when(const std::function<void(const std::exception&)>& closure) {
    try {
      wait();
    } catch (std::exception& e) {
      closure(e);
    }
  }

  ReturnType wait(double seconds = Threading::noTimeout) {
    event.lock();
    defer([=] { event.unlock(); });

    if (!error.empty()) {
      throw std::runtime_error(error.c_str());
    }

    if (value) {
      return *value;
    }

    do {
      if (!error.empty()) {
        throw std::runtime_error(error.c_str());
      }

      if (value) {
        return *value;
      }
    } while (event.wait(seconds));

    if (!error.empty()) {
      throw std::runtime_error(error.c_str());
    }

    if (value) {
      return *value;
    }

    throw NoPromiseValue();
  }

  std::shared_ptr<DestroyerOfQueues> queue;

 private:
  Threading::Event event;
  std::shared_ptr<ReturnType> value;
  std::string error;
};

}  // namespace Easy

#endif /*EASY_THREAD_PROMISE_H*/
