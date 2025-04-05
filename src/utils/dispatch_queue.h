#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * @brief Task queue that is executed on a background thread.
 *
 * Tasks are automatically processed by a background thread.
 * The thread waits when the queue is empty and is notified when new tasks are
 * added.
 */
class DispatchQueue {
 protected:
  std::queue<std::function<void()>> taskQueue;
  std::mutex m;
  std::condition_variable cv;
  std::thread worker;
  bool running;
  std::thread::id workerId;

 public:
  /**
   * Constructor starts the worker thread
   */
  DispatchQueue();

  /**
   * Destructor stops the worker thread
   */
  ~DispatchQueue();

  /**
   * Execute a task synchronously
   * @param task The function to execute
   */
  void runTask(std::function<void()> task);

  /**
   * Stop the worker thread
   */
  void stop();

  /**
   * Check if current thread is the worker thread
   * @return true if current thread is the worker thread
   */
  bool isWorkerThread() const;
};
