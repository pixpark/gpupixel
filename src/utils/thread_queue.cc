#include <utils/thread.h>

#include <unistd.h>

namespace Easy {

std::map<std::string, std::shared_ptr<ThreadQueue>> Threading::serialQueues;
std::map<std::string, std::shared_ptr<ThreadQueue>> Threading::concurrentQueues;
std::shared_ptr<ThreadQueue> Threading::defaultQueue;
Threading::Lock Threading::queuesLock;

bool Threading::SerialQueue::running() const {
  return _running;
}

void Threading::SerialQueue::setRunning(bool run) {
  _running = run;
}

void Threading::SerialQueue::broadcast() {
  lock.broadcast();
}

std::string Threading::SerialQueue::name() const {
  return _name;
}

int Threading::SerialQueue::type() const {
  return Threading::Serial;
}

Threading::SerialQueue::SerialQueue(const std::string& name)
    : _name(name), _running(true) {
  startLoop();
}

void Threading::SerialQueue::dispatch(const ThreadClosure& closure) {
  lock.lock();
  defer([this] { lock.unlock(); });
  q.push(closure);
  lock.signal();
}

void Threading::SerialQueue::startLoop() {
  Threading::dispatchOnNewThread([this] {
    while (running()) {
      ThreadClosure block;
      {
        lock.lock();
        defer([this] { lock.unlock(); });

        auto count = q.size();

        if (count > 0) {
          block = q.front();
          q.pop();
        } else {
          lock.wait();
          if (q.size() > 0) {
            block = q.front();
            q.pop();
          }
        }
      }
      if (block) {
        block();
      }
    }
  });
}

Threading::ConcurrentQueue::ConcurrentQueue(const std::string& name)
    : _name(name), _running(true) {
  startLoop();
}

bool Threading::ConcurrentQueue::running() const {
  return _running;
}

void Threading::ConcurrentQueue::setRunning(bool run) {
  _running = run;
}

void Threading::ConcurrentQueue::broadcast() {
  lock.broadcast();
}

std::string Threading::ConcurrentQueue::name() const {
  return _name;
}

int Threading::ConcurrentQueue::type() const {
  return Threading::Concurrent;
}

void Threading::ConcurrentQueue::dispatch(const ThreadClosure& closure) {
  lock.lock();
  defer([this] { lock.unlock(); });
  q.push(closure);
  lock.signal();
}

void Threading::ConcurrentQueue::startLoop() {
  for (int i = 0, count = std::max(4, Threading::processorCount()); i < count;
       ++i) {
    Threading::dispatchOnNewThread([this] {
      while (running()) {
        ThreadClosure block;
        {
          lock.lock();
          defer([this] { lock.unlock(); });

          auto count = q.size();

          if (count > 0) {
            block = q.front();
            q.pop();
          } else {
            lock.wait();
            if (q.size() > 0) {
              block = q.front();
              q.pop();
            }
          }
        }
        if (block) {
          block();
        }
      }
    });
  }
}

int Threading::processorCount() {
  return sysconf(_SC_NPROCESSORS_ONLN);
}

std::shared_ptr<ThreadQueue> Threading::getDefaultQueue() {
  if (defaultQueue == nullptr) {
    defaultQueue = getQueue("default", Concurrent);
  }
  return defaultQueue;
}

std::shared_ptr<ThreadQueue> Threading::getQueue(QueueType type) {
  switch (type) {
    case Serial:
      return std::make_shared<SerialQueue>(anonymousQueueName);
    case Concurrent:
      return std::make_shared<ConcurrentQueue>(anonymousQueueName);
    default:
      return nullptr;
  }
}

std::shared_ptr<ThreadQueue> Threading::getQueue(const std::string& name,
                                                 QueueType type) {
  queuesLock.lock();
  defer([] { queuesLock.unlock(); });

  switch (type) {
    case Serial: {
      if (serialQueues.find(name) != serialQueues.end()) {
        return serialQueues[name];
      }

      auto q = std::make_shared<SerialQueue>(name);
      serialQueues.insert(std::make_pair(name, q));
      return q;
    }
    case Concurrent: {
      if (concurrentQueues.find(name) != concurrentQueues.end()) {
        return concurrentQueues[name];
      }

      auto q = std::make_shared<ConcurrentQueue>(name);

      concurrentQueues.insert(std::make_pair(name, q));
      return q;
    }
    default:
      return nullptr;
  }
}

void Threading::destroyQueue(std::shared_ptr<ThreadQueue> queue) {
  if (queue == nullptr) {
    return;
  }
  if (queue->name() != anonymousQueueName) {
    queuesLock.lock();
    defer([] { queuesLock.unlock(); });
    switch (queue->type()) {
      case Serial:
        serialQueues.erase(queue->name());
        break;
      case Concurrent:
        concurrentQueues.erase(queue->name());
      default:
        break;
    }
  }

  auto qi = dynamic_cast<ThreadQueueInternal*>(queue.get());
  if (qi) {
    qi->setRunning(false);
    qi->broadcast();
  }
}
}  // namespace Easy
