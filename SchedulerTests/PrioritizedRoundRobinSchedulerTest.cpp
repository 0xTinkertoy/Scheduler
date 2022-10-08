//
//  PrioritizedRoundRobinSchedulerTest.cpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#include "PrioritizedRoundRobinSchedulerTest.hpp"
#include "SimpleTask.hpp"
#include "SampleSchedulers.hpp"
#include <Debug.hpp>

namespace Schedulers = SampleSchedulers;

void PrioritizedRoundRobinSchedulerTest::runPrimitivesTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::PrioritizedRoundRobin<SimpleTask, 9> scheduler(&idleTask);

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

        uint32_t identifier = 4 - index;

        passert(task->getIdentifier() == identifier, "Task%u Identifier", identifier);

        passert(task->getPriority() == identifier * identifier, "Task%u Priority", identifier);
    }

    // Empty ready queue
    passert(scheduler.next() == nullptr, "Empty ready queue");
}

void PrioritizedRoundRobinSchedulerTest::runTaskManagerDelegateTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::PrioritizedRoundRobin<SimpleTask, 9> scheduler(&idleTask);

    // Assume that Task 2 is running
    // Task 1 is created
    passert(scheduler.onTaskCreated(&t2, &t1)->getIdentifier() == 2,
            "Task 1 cannot preempt Task 2 due to a lower priority.");

    // Task 3 is created
    passert(scheduler.onTaskCreated(&t2, &t3)->getIdentifier() == 3,
            "Task 3 can preempt Task 2 due to a higher priority.");

    // Task 3 has finished
    passert(scheduler.onTaskFinished(&t3)->getIdentifier() == 2,
            "Task 2 resumes after Task 3 has finished.");

    // Task 2 has finished
    passert(scheduler.onTaskFinished(&t2)->getIdentifier() == 1,
            "Task 1 resumes after Task 2 has finished.");

    // Task 1 has finished
    passert(scheduler.onTaskFinished(&t1)->getIdentifier() == 0,
            "Idle task runs after Task 1 has finished.");

    // Task 3 is created
    passert(scheduler.onTaskCreated(&idleTask, &t3)->getIdentifier() == 3,
            "Task 3 preempts the idle task.");

    // Task 2 is created
    passert(scheduler.onTaskCreated(&t3, &t2)->getIdentifier() == 3,
            "Task 2 cannot preempt Task 3 due to a lower priority.");

    // Task 3 has been blocked
    passert(scheduler.onTaskBlocked(&t3)->getIdentifier() == 2,
            "Task 2 resumes after Task 3 has been blocked.");

    // Task 3 has been unblocked
    passert(scheduler.onTaskUnblocked(&t2, &t3)->getIdentifier() == 3,
            "Task 3 preempts Task 2 after it has been unblocked.");

    // Task 3 yielded
    passert(scheduler.onTaskYielded(&t3)->getIdentifier() == 3,
            "Task 3 resumes after it yields.");
}

void PrioritizedRoundRobinSchedulerTest::runTimerInterruptDelegateTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::PrioritizedRoundRobin<SimpleTask, 9> scheduler(&idleTask);

    // Idle task is running
    passert(scheduler.onTimerInterrupt(&idleTask)->getIdentifier() == 0,
            "Idle task keeps running.");

    // Task 2 has arrived
    passert(scheduler.onTaskCreated(&idleTask, &t2)->getIdentifier() == 2,
            "Task 2 has arrived.");

    // Task 3 has arrived
    passert(scheduler.onTaskCreated(&t2, &t3)->getIdentifier() == 3,
            "Task 3 has arrived.");

    // Timer interrupt
    passert(scheduler.onTimerInterrupt(&t3)->getIdentifier() == 3,
            "Task 3 resumes after the timer interrupt.");

    // Task 3 has finished
    passert(scheduler.onTaskFinished(&t3)->getIdentifier() == 2,
            "Task 2 resumes after Task 3 has finished.");

    // Timer interrupt
    passert(scheduler.onTimerInterrupt(&t2)->getIdentifier() == 2,
            "Task 2 resumes after the timer interrupt.");

    // Task 2 has finished
    passert(scheduler.onTaskFinished(&t2)->getIdentifier() == 0,
            "Idle task runs after Task 2 has finished.");

    // Timer interrupt
    passert(scheduler.onTimerInterrupt(&idleTask)->getIdentifier() == 0,
            "Idle task keeps running on a timer interrupt.");
}

void PrioritizedRoundRobinSchedulerTest::runGroupOperationsTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::PrioritizedRoundRobin<SimpleTask, 25> scheduler(&idleTask);

    // Task 1 is running
    // A timer interrupt occurs
    // Task 2 and Task 3 are unblocked
    passert(scheduler.onTaskUnblocked(nullptr, &t2) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskUnblocked(nullptr, &t3) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTimerInterrupt(&t1)->getIdentifier() == 3, "Task 3 starts to run on a timer interrupt.");

    passert(scheduler.onTimerInterrupt(&t3)->getIdentifier() == 3, "Task 3 keeps running on a timer interrupt.");

    SimpleTask t4(4, 16);

    SimpleTask t5(5, 25);

    // Task 2 has finished
    // Task 4 and Task 5 are unblocked
    passert(scheduler.onTaskUnblocked(nullptr, &t4) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskUnblocked(nullptr, &t5) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskFinished(&t2)->getIdentifier() == 5, "Task 5 starts to run after Task 2 has finished.");
}
