#ifndef SE_THREAD_THREAD_H
#define SE_THREAD_THREAD_H

#include <pthread.h>

#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <string>

namespace Easy {

class Defer {
 public:
  Defer(const std::function<void()>& closure) : _closure(closure) {}
  ~Defer() {
    if (_closure) {
      _closure();
    }
  }

 private:
  std::function<void()> _closure;
};

#define defer(x)    \
  Defer __defer__ { \
    x               \
  }

typedef std::function<void()> ThreadClosure;

class ThreadQueue {
 public:
  virtual std::string name() const = 0;
  virtual int type() const = 0;
  virtual void dispatch(const ThreadClosure& closure) = 0;
  virtual ~ThreadQueue() {}
};

class ThreadQueueInternal {
 public:
  virtual bool running() const = 0;
  virtual void setRunning(bool run) = 0;
  virtual void broadcast() = 0;
  virtual ~ThreadQueueInternal() {}
};

class Threading {
 public:
  static constexpr double noTimeout = 0.0;
  static const char* anonymousQueueName;
  Threading();

 public:
  enum QueueType { Serial, Concurrent };

 public:
  class Lock {
   public:
    Lock();

    ~Lock();

    bool lock();

    bool tryLock();

    bool unlock();

    template <typename Result>
    Result doWithLock(const std::function<Result()>& closure) {
      lock();
      defer([this] { unlock(); });
      return closure();
    }

   protected:
    pthread_mutex_t mutex;
  };

  class Event : public Lock {
   public:
    Event();
    ~Event();

    bool signal();

    bool broadcast();

    bool wait(double seconds = Threading::noTimeout);

   private:
    pthread_cond_t cond;
  };

  class RWLock {
   public:
    RWLock();
    ~RWLock();

    bool readLock();
    bool tryReadLock();

    bool writeLock();
    bool tryWriteLock();

    bool unlock();

    template <typename Result>
    Result doWithReadLock(const std::function<Result()>& closure) {
      readLock();
      defer([this] { unlock(); });
      return closure();
    }

    template <typename Result>
    Result doWithWriteLock(const std::function<Result()>& closure) {
      writeLock();
      defer([this] { unlock(); });
      return closure();
    }

   private:
    pthread_rwlock_t lock;
  };

  static void sleep(double seconds);

  static void dispatchOnNewThread(const ThreadClosure& closure);

  static int processorCount();

  static std::shared_ptr<ThreadQueue> getDefaultQueue();
  static std::shared_ptr<ThreadQueue> getQueue(QueueType type);
  static std::shared_ptr<ThreadQueue> getQueue(const std::string& name,
                                               QueueType type);
  static void destroyQueue(std::shared_ptr<ThreadQueue> queue);

 private:
  static std::map<std::string, std::shared_ptr<ThreadQueue>> serialQueues;
  static std::map<std::string, std::shared_ptr<ThreadQueue>> concurrentQueues;
  static std::shared_ptr<ThreadQueue> defaultQueue;
  static Lock queuesLock;

 private:
  class SerialQueue : public ThreadQueue, public ThreadQueueInternal {
   public:
    SerialQueue(const std::string& name);

    bool running() const override;
    void setRunning(bool run) override;
    void broadcast() override;
    std::string name() const override;
    int type() const override;
    void dispatch(const ThreadClosure& closure);

   private:
    void startLoop();

   private:
    bool _running;
    Threading::Event lock;
    std::string _name;
    std::queue<ThreadClosure> q;
  };

  class ConcurrentQueue : public ThreadQueue, public ThreadQueueInternal {
   public:
    ConcurrentQueue(const std::string& name);

    bool running() const override;
    void setRunning(bool run) override;
    void broadcast();
    std::string name() const override;
    int type() const override;
    void dispatch(const ThreadClosure& closure) override;

   private:
    void startLoop();

   private:
    bool _running;
    Threading::Event lock;
    std::string _name;
    std::queue<ThreadClosure> q;
  };
};

}  // namespace Easy

#endif /*SE_THREAD_THREAD_H*/
