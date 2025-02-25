#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/*
 * Base for DispatchQueue where
 */

/**
 * @brief Task queue that is executed on the current thread.
 *
 * All tasks must be processed by calls to processOne or processAll on the
 * thread that will execute those tasks.
 */
class LocalDispatchQueue {
 protected:
  std::queue<std::function<void()>> taskQueue;
  std::mutex m;

 public:
  /**
   * Add a task to the queue.
   *
   * @param task the lambda to execute for the task
   */
  void add(std::function<void()> task);

  /**
   * Deque a single task and execute it. This method blocks until the task
   * is complete.
   */
  void processOne();

  /**
   * Dequeue tasks one at a time and execute them sequentially. This method
   * blocks until the task queue is empty.
   */
  void processAll();
};

/**
 * @brief Task queue that is executed on a single or multiple threads.
 *
 * The execution thread(s) are separate from the current thread (unlike
 * LocalDispatchQueue).
 */
class DispatchQueue {
 public:
  /**
   * Dispatch queue type. Serial executes tasks sequentially in a single
   * thread while concurrent executes up to n tasks at once where n is
   * the number of cpu cores as returned from
   * `std::thread::hardware_concurrency()`.
   */
  enum QueueType { Serial, Concurrent };

 private:
  bool running;
  int nWorking;
  std::mutex m;
  std::condition_variable cv;
  std::queue<std::function<void()>> taskQueue;
  std::vector<std::thread> workers;

  /**
   * Worker thread method. This method takes a task from the queue an executes
   * it.
   *
   * @param id the worker id
   */
  void worker(size_t id);

 public:
  /**
   * Create a new `DispatchQueue` with queue `type`.
   * @param type the queue type
   */
  DispatchQueue(QueueType type);

  /**
   * Check if there are tasks waiting for execution.
   */
  bool busy();

  /**
   * Wait for all currently executing tasks to complete and skip any tasks
   * that have not yet been started.
   */
  void stop();

  /**
   * Wait for all tasks to start execution.
   */
  void wait();

  /**
   * Wait for all tasks to start execution and then wait for their completion.
   */
  void join();

  /**
   * Add a task to the queue.
   *
   * @param task the lambda to execute for the task
   */
  void add(const std::function<void()>& task);
};
