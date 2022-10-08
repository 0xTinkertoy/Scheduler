//
//  FIFOSchedulerTest.cpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#include "FIFOSchedulerTest.hpp"
#include "SimpleTask.hpp"
#include "SampleSchedulers.hpp"
#include <Debug.hpp>

namespace Schedulers = SampleSchedulers;

void FIFOSchedulerTest::runPrimitivesTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::FIFO<SimpleTask> scheduler(&idleTask);

    // Empty ready queue
    passert(scheduler.next() == nullptr, "Empty ready queue");

    scheduler.ready(&t1);

    scheduler.ready(&t2);

    scheduler.ready(&t3);

    // Get next task
    for (uint32_t index = 1; index < 4; index += 1)
    {
        SimpleTask* task = scheduler.next();

        task->print();

        passert(task->getIdentifier() == index, "Task%u Identifier", index);

        passert(task->getPriority() == index * index, "Task%u Priority", index);
    }

    // Empty ready queue
    passert(scheduler.next() == nullptr, "Empty ready queue");
}

void FIFOSchedulerTest::runTaskManagerDelegateTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::FIFO<SimpleTask> scheduler(&idleTask);

    // Assume that Task 1 is running
    // Task 2 is created
    passert(scheduler.onTaskCreated(&t1, &t2)->getIdentifier() == 1,
            "Task 1 should keep running after Task 2 is created.");

    // Task 1 has finished
    // Task 2 should be the next running task
    passert(scheduler.onTaskFinished(&t1)->getIdentifier() == 2,
            "Task 1 has finished running. Task 2 should be the next one.");

    // Task 2 has finished
    // Idle task should be the next running task
    passert(scheduler.onTaskFinished(&t2)->getIdentifier() == 0,
            "Idle task should be running when no task is ready.");

    // Task 3 has been unblocked
    // Task 3 should be running instead of the idle task
    passert(scheduler.onTaskUnblocked(&idleTask, &t3)->getIdentifier() == 3,
            "Task 3 is now unblocked and is the next one to run.");

    // Task 1 is created while Task 3 is running
    passert(scheduler.onTaskCreated(&t3, &t1)->getIdentifier() == 3,
            "Task 3 should keep running after Task 1 is created.");

    // Task 3 has been blocked
    // Task 1 should be the next running task
    passert(scheduler.onTaskBlocked(&t3)->getIdentifier() == 1,
            "Task 1 should be running after Task 3 has been blocked.");

    // Task 1 yielded
    passert(scheduler.onTaskYielded(&t1)->getIdentifier() == 1,
            "Task 1 yielded but it is the only task ready to run.");
}

void FIFOSchedulerTest::runTimerInterruptDelegateTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::FIFO<SimpleTask> scheduler(&idleTask);

    // Assume that Task 1 is running
    passert(scheduler.onTimerInterrupt(&t1)->getIdentifier() == 1,
            "Task 1 resumes after a timer interrupt.");
}

void FIFOSchedulerTest::runGroupOperationsTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::FIFO<SimpleTask> scheduler(&idleTask);

    // Assume that Task 1 is running
    // Task 2 and Task 3 are unblocked
    passert(scheduler.onTaskUnblocked(nullptr, &t2) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskUnblocked(&t1, &t3)->getIdentifier() == 1, "Terminating unblock call.");

    passert(scheduler.onTaskFinished(&t1)->getIdentifier() == 2, "Task 1 has finished and Task 2 starts to run.");

    passert(scheduler.onTaskFinished(&t2)->getIdentifier() == 3, "Task 2 has finished and Task 3 starts to run.");

    // Task 3 has finished
    // Task 1 and Task 2 are unblocked
    passert(scheduler.onTaskUnblocked(nullptr, &t1) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskUnblocked(nullptr, &t2) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskFinished(&t3)->getIdentifier() == 1, "Task 1 starts to run after Task 3 has finished.");

    passert(scheduler.onTaskUnblocked(&t1, nullptr)->getIdentifier() == 1, "Special version of unblock call.");
}
