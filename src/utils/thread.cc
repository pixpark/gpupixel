#include "utils/thread.h"

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

namespace Easy {

int my_pthread_cond_timedwait_relative_np(pthread_cond_t* cond,
                                          pthread_mutex_t* mutex,
                                          const timespec* tmspec) {
  timespec timeout;
  timeval time;

  gettimeofday(&time, nullptr);

  timeout.tv_sec = time.tv_sec;
  timeout.tv_nsec = time.tv_usec * 1000;

  clock_gettime(CLOCK_MONOTONIC, &timeout);

  timeout.tv_sec += tmspec->tv_sec;
  timeout.tv_nsec += tmspec->tv_nsec;

  if (timeout.tv_nsec >= 1000000000LL) {
    timeout.tv_sec++;
    timeout.tv_nsec -= 1000000000LL;
  }

  return pthread_cond_timedwait(cond, mutex, &timeout);
}

const char* Threading::anonymousQueueName = "anonymous";

Threading::Lock::Lock() {
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mutex, &attr);
}

Threading::Lock::~Lock() {
  pthread_mutex_destroy(&mutex);
}

bool Threading::Lock::lock() {
  return 0 == pthread_mutex_lock(&mutex);
}

bool Threading::Lock::tryLock() {
  return 0 == pthread_mutex_trylock(&mutex);
}

bool Threading::Lock::unlock() {
  return 0 == pthread_mutex_unlock(&mutex);
}

Threading::Event::Event() : Lock() {
  pthread_condattr_t attr;
  pthread_condattr_init(&attr);
  pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
  pthread_cond_init(&cond, &attr);
  pthread_condattr_destroy(&attr);
}

Threading::Event::~Event() {
  pthread_cond_destroy(&cond);
}

bool Threading::Event::signal() {
  return 0 == pthread_cond_signal(&cond);
}

bool Threading::Event::broadcast() {
  return 0 == pthread_cond_broadcast(&cond);
}

bool Threading::Event::wait(double seconds) {
  if (seconds == Threading::noTimeout) {
    return 0 == pthread_cond_wait(&cond, &mutex);
  }
  timespec tm;
  tm.tv_sec = (int)seconds;
  tm.tv_nsec = (int)((seconds * 1000 - tm.tv_sec * 1000) * 1000000);

  return 0 == my_pthread_cond_timedwait_relative_np(&cond, &mutex, &tm);
}

Threading::RWLock::RWLock() {
  pthread_rwlock_init(&lock, nullptr);
}

Threading::RWLock::~RWLock() {
  pthread_rwlock_destroy(&lock);
}

bool Threading::RWLock::readLock() {
  return 0 == pthread_rwlock_rdlock(&lock);
}

bool Threading::RWLock::tryReadLock() {
  return 0 == pthread_rwlock_tryrdlock(&lock);
}

bool Threading::RWLock::writeLock() {
  return 0 == pthread_rwlock_wrlock(&lock);
}

bool Threading::RWLock::tryWriteLock() {
  return 0 == pthread_rwlock_trywrlock(&lock);
}

bool Threading::RWLock::unlock() {
  return 0 == pthread_rwlock_unlock(&lock);
}

void Threading::sleep(double seconds) {
  if (seconds < 0.0) {
    return;
  }
  int milliseconds = (int)(seconds * 1000.0);
  timeval tv;
  tv.tv_sec = milliseconds / 1000;
  tv.tv_usec = (milliseconds % 1000) * 1000;
  select(0, nullptr, nullptr, nullptr, &tv);
}

class IsThisRequired {
 public:
  IsThisRequired(const ThreadClosure& closure) : closure(closure) {}

  void run() {
    if (closure) {
      closure();
    }
    delete this;
  }

 private:
  ThreadClosure closure;
};

typedef void* (*ThreadFunction)(void*);

void Threading::dispatchOnNewThread(const ThreadClosure& closure) {
  pthread_t thrSlf;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  auto holdObject = new IsThisRequired(closure);

  pthread_create(&thrSlf, &attr, (ThreadFunction)&IsThisRequired::run,
                 holdObject);
}
}  // namespace Easy
