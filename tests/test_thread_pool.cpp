#include <gtest/gtest.h>

#include "zerolancom/utils/periodic_task.hpp"
#include "zerolancom/utils/thread_pool.hpp"

namespace zlc
{

// =============================================
// ThreadPool Tests
// =============================================

TEST(ThreadPoolTest, ConstructionWithDefaultThreads)
{
  ThreadPool pool;
  EXPECT_GT(pool.size(), 0);
}

TEST(ThreadPoolTest, ConstructionWithCustomThreads)
{
  ThreadPool pool(4);
  EXPECT_EQ(pool.size(), 4);
}

TEST(ThreadPoolTest, StartStop)
{
  ThreadPool pool(2);
  EXPECT_FALSE(pool.is_running());

  pool.start();
  EXPECT_TRUE(pool.is_running());

  pool.stop();
  EXPECT_FALSE(pool.is_running());
}

TEST(ThreadPoolTest, MultipleStartStop)
{
  ThreadPool pool(2);

  pool.start();
  pool.stop();

  // Should be able to start again
  pool.start();
  pool.stop();
}

TEST(ThreadPoolTest, BasicTaskExecution)
{
  ThreadPool pool(2);
  pool.start();

  int counter = 0;
  std::mutex mutex;

  for (int i = 0; i < 10; ++i)
  {
    pool.enqueue(
        [&counter, &mutex]()
        {
          std::lock_guard<std::mutex> lock(mutex);
          counter++;
        });
  }

  pool.wait();
  pool.stop();

  EXPECT_EQ(counter, 10);
}

TEST(ThreadPoolTest, TaskExecutesOnDifferentThreads)
{
  ThreadPool pool(4);
  pool.start();

  std::set<std::thread::id> thread_ids;
  std::mutex mutex;

  for (int i = 0; i < 10; ++i)
  {
    pool.enqueue(
        [&thread_ids, &mutex]()
        {
          std::lock_guard<std::mutex> lock(mutex);
          thread_ids.insert(std::this_thread::get_id());
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
  }

  pool.wait();
  pool.stop();

  // Should have used multiple threads
  EXPECT_GT(thread_ids.size(), 1);
}

TEST(ThreadPoolTest, ExceptionInTaskIsHandled)
{
  ThreadPool pool(2);
  pool.start();

  int successful = 0;
  std::mutex mutex;

  for (int i = 0; i < 5; ++i)
  {
    pool.enqueue(
        [&successful, &mutex, i]()
        {
          if (i == 2)
          {
            throw std::runtime_error("Test exception");
          }

          std::lock_guard<std::mutex> lock(mutex);
          successful++;
        });
  }

  pool.wait();
  pool.stop();

  // Should have executed 4 successful tasks (1 threw exception)
  EXPECT_EQ(successful, 4);
}

TEST(ThreadPoolTest, WaitBlocksUntilAllTasksComplete)
{
  ThreadPool pool(2);
  pool.start();

  std::atomic<int> counter{0};

  for (int i = 0; i < 10; ++i)
  {
    pool.enqueue(
        [&counter]()
        {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
          counter++;
        });
  }

  pool.wait();

  EXPECT_EQ(counter, 10);

  pool.stop();
}

TEST(ThreadPoolTest, PendingTasksCount)
{
  ThreadPool pool(1);
  pool.start();

  // Enqueue slow tasks to keep them in queue
  for (int i = 0; i < 5; ++i)
  {
    pool.enqueue([]() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
  }

  // Should have pending tasks
  size_t pending = pool.pending_tasks();
  EXPECT_GT(pending, 0);

  pool.wait();
  pool.stop();
}

TEST(ThreadPoolTest, EnqueueOnStoppedPoolIsIgnored)
{
  ThreadPool pool(2);

  // Don't start the pool
  int counter = 0;
  pool.enqueue([&counter]() { counter++; });

  // Task should not execute
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_EQ(counter, 0);
}

TEST(ThreadPoolTest, DestructorStopsPool)
{
  {
    ThreadPool pool(2);
    pool.start();

    std::atomic<int> counter{0};

    for (int i = 0; i < 10; ++i)
    {
      pool.enqueue([&counter]() { counter++; });
    }

    // Destructor should stop the pool and wait
  } // pool destroyed here

  // Should not crash or leak
}

// =============================================
// PeriodicTask Tests
// =============================================

TEST(PeriodicTaskTest, ExecutesPeriodically)
{
  ThreadPool pool(2);
  pool.start();
  int counter = 0;

  PeriodicTask task([&counter]() { counter++; }, 50, pool);
  task.start();

  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  task.stop();
  pool.stop();

  // Should execute approximately 5 times (250ms / 50ms)
  // Allow range due to timing variations
  EXPECT_GE(counter, 3);
  EXPECT_LE(counter, 7);
}

TEST(PeriodicTaskTest, StopPreventsExecution)
{
  ThreadPool pool(2);
  pool.start();
  int counter = 0;

  PeriodicTask task([&counter]() { counter++; }, 50, pool);
  task.start();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  task.stop();

  int count_at_stop = counter;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  pool.stop();

  // Should not have increased
  EXPECT_EQ(counter, count_at_stop);
}

TEST(PeriodicTaskTest, WithThreadPool)
{
  ThreadPool pool(2);
  pool.start();

  int counter = 0;

  PeriodicTask task([&counter]() { counter++; }, 50, pool);
  task.start();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  task.stop();

  pool.stop();

  EXPECT_GE(counter, 2);
}

TEST(PeriodicTaskTest, ExceptionInCallbackIsHandled)
{
  ThreadPool pool(2);
  pool.start();
  int counter = 0;
  int exceptions = 0;

  PeriodicTask task(
      [&counter, &exceptions]()
      {
        if (counter % 2 == 0)
        {
          exceptions++;
          throw std::runtime_error("Test exception");
        }
        counter++;
      },
      50, pool);

  task.start();

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  task.stop();
  pool.stop();

  // Should have handled exceptions and continued
  EXPECT_GT(exceptions, 0);
}

TEST(PeriodicTaskTest, IsRunning)
{
  ThreadPool pool(2);
  pool.start();
  PeriodicTask task([]() {}, 100, pool);

  EXPECT_FALSE(task.is_running());

  task.start();
  EXPECT_TRUE(task.is_running());

  task.stop();
  EXPECT_FALSE(task.is_running());
  pool.stop();
}

TEST(PeriodicTaskTest, DestructorStopsTask)
{
  ThreadPool pool(2);
  pool.start();
  int counter = 0;

  {
    PeriodicTask task([&counter]() { counter++; }, 50, pool);
    task.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  } // task destroyed here

  int count_at_destroy = counter;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  pool.stop();

  // Should not have increased after destruction
  EXPECT_EQ(counter, count_at_destroy);
}

} // namespace zlc
