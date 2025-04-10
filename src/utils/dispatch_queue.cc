#include "utils/dispatch_queue.h"

DispatchQueue::DispatchQueue() : running(true) {
  worker = std::thread([this]() {
    workerId = std::this_thread::get_id();
    while (running) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this]() { return !taskQueue.empty() || !running; });

        if (!running && taskQueue.empty()) {
          return;
        }

        task = taskQueue.front();
        taskQueue.pop();
      }
      task();
    }
  });
}

DispatchQueue::~DispatchQueue() {
  stop();
}

void DispatchQueue::stop() {
  {
    std::unique_lock<std::mutex> lk(m);
    running = false;
  }
  cv.notify_one();
  if (worker.joinable()) {
    worker.join();
  }
}

bool DispatchQueue::isWorkerThread() const {
  return std::this_thread::get_id() == workerId;
}

void DispatchQueue::runTask(std::function<void()> task) {
  // If current thread is the worker thread, execute the task directly to avoid
  // deadlock
  if (isWorkerThread()) {
    task();
    return;
  }

  // Create a promise and future for synchronization
  std::promise<void> promise;
  std::future<void> future = promise.get_future();

  // Wrap the original task to set the promise when completed
  auto wrappedTask = [task, &promise]() {
    try {
      task();
      promise.set_value();
    } catch (...) {
      try {
        promise.set_exception(std::current_exception());
      } catch (...) {
        // Ignore exceptions when the promise is already set
      }
    }
  };

  // Add the wrapped task to the queue
  {
    std::unique_lock<std::mutex> lk(m);
    taskQueue.push(wrappedTask);
  }
  cv.notify_one();

  // Wait for the task to complete
  future.wait();
}
