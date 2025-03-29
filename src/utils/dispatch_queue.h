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
class DispatchQueue {
public:
    /**
     * Deque a single task and execute it. This method blocks until the task
     * is complete.
     */
    void processOne();
 
    /**
     * 
     * @return true if the task queue is empty, false otherwise
     */
    void runTask(std::function<void()> task);

private:
    /**
     * Add a task to the queue.
     *
     * @param task the lambda to execute for the task
     */
    void add(std::function<void()> task);
    
    std::queue<std::function<void()>> taskQueue;
    std::mutex m;
};
