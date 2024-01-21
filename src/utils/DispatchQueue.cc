#include "DispatchQueue.h"

void LocalDispatchQueue::add(std::function<void()> task) {
    std::unique_lock lk(m);
    taskQueue.push(task);
}

void LocalDispatchQueue::processOne() {
    std::function<void()> task;
    {
        std::unique_lock lk(m);
        if (taskQueue.empty())
            return;

        task = taskQueue.front();
        taskQueue.pop();
    }
    task();
}

void LocalDispatchQueue::processAll() {
    for (;;) {
        std::function<void()> task;
        {
            std::unique_lock lk(m);
            if (taskQueue.empty())
                return;

            task = taskQueue.front();
            taskQueue.pop();
        }
        task();
    }
}

void DispatchQueue::worker(size_t id) {
    std::function<void()> task;
    while (running) {
        {
            std::unique_lock lk(m);
            cv.wait(lk, [&]() {
                return !running || !taskQueue.empty();
            });

            if (!running)
                break;

            task = taskQueue.front();
            taskQueue.pop();
            nWorking++;
        }
        task();
        {
            std::unique_lock lk(m);
            nWorking--;
        }
    }
}

DispatchQueue::DispatchQueue(QueueType type) : running(true), nWorking(0) {
    workers.emplace_back(&DispatchQueue::worker, this, 0);
    if (type == Concurrent) {
        size_t n = std::thread::hardware_concurrency();
        for (size_t i = 1; i < n; i++) {
            workers.emplace_back(&DispatchQueue::worker, this, i);
        }
    }
}

bool DispatchQueue::busy() {
    bool queueBusy;
    {
        std::unique_lock lk(m);
        queueBusy = !taskQueue.empty() || nWorking > 0;
    }
    return queueBusy;
}

void DispatchQueue::stop() {
    {
        std::unique_lock lk(m);
        running = false;
    }
    cv.notify_all();
    for (auto & worker : workers) {
        worker.join();
    }
    workers.clear();
}

void DispatchQueue::wait() {
    while (busy()) std::this_thread::yield();
}

void DispatchQueue::join() {
    wait();
    stop();
}

void DispatchQueue::add(const std::function<void()> & task) {
    {
        std::unique_lock lk(m);
        taskQueue.push(task);
    }
    cv.notify_one();
}
