#include "dispatch_queue.h"

void DispatchQueue::add(std::function<void()> task) {
     std::unique_lock<std::mutex> lk(m);
    taskQueue.push(task);
}

void DispatchQueue::processOne() {
    std::function<void()> task;
    {
         std::unique_lock<std::mutex> lk(m);
        if (taskQueue.empty())
            return;

        task = taskQueue.front();
        taskQueue.pop();
    }
    task();
}

void DispatchQueue::processAll() {
    for (;;) {
        std::function<void()> task;
        {
             std::unique_lock<std::mutex> lk(m);
            if (taskQueue.empty())
                return;

            task = taskQueue.front();
            taskQueue.pop();
        }
        task();
    }
}
 
void DispatchQueue::RunSync(std::function<void()> task) {
    add(task);
    processOne();
}