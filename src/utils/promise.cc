#include <utils/promise.h>

namespace Easy {

DestroyerOfQueues::DestroyerOfQueues(const std::shared_ptr<ThreadQueue>& queue)
    : queue(queue) {}

void DestroyerOfQueues::dispatch(const ThreadClosure& closure) {
  if (queue) {
    queue->dispatch(closure);
  }
}

DestroyerOfQueues::~DestroyerOfQueues() {
  Threading::destroyQueue(queue);
}
}  // namespace Easy
