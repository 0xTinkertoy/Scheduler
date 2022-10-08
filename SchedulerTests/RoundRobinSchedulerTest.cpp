//
//  RoundRobinSchedulerTest.cpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#include "RoundRobinSchedulerTest.hpp"
#include "SimpleTask.hpp"
#include "SampleSchedulers.hpp"
#include <Debug.hpp>

namespace Schedulers = SampleSchedulers;

void RoundRobinSchedulerTest::runPrimitivesTest()
{
    pinfo("Same as FIFO scheduler.");
}

void RoundRobinSchedulerTest::runTaskManagerDelegateTest()
{
    pinfo("Same as FIFO scheduler.");
}

void RoundRobinSchedulerTest::runTimerInterruptDelegateTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::RoundRobin<SimpleTask> scheduler(&idleTask);

    // Assume that Task 1 is running
    passert(scheduler.onTimerInterrupt(&t1)->getIdentifier() == 1,
            "Task 1 resumes after a timer interrupt.");

    // Task 2 and Task 3 are ready
    scheduler.ready(&t2);

    scheduler.ready(&t3);

    // Timer interrupt
    passert(scheduler.onTimerInterrupt(&t1)->getIdentifier() == 2,
            "Task 2 preempts Task 1 on a timer interrupt.");

    passert(scheduler.onTimerInterrupt(&t2)->getIdentifier() == 3,
            "Task 3 preempts Task 2 on a timer interrupt.");

    passert(scheduler.onTimerInterrupt(&t3)->getIdentifier() == 1,
            "Task 1 preempts Task 3 on a timer interrupt.");
}

void RoundRobinSchedulerTest::runGroupOperationsTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 4);

    SimpleTask t3(3, 9);

    Schedulers::RoundRobin<SimpleTask> scheduler(&idleTask);

    // Task 1 is running
    // Timer interrupt occurs
    // Task 2 and Task 3 are unblocked
    passert(scheduler.onTaskUnblocked(nullptr, &t2) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskUnblocked(nullptr, &t3) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTimerInterrupt(&t1)->getIdentifier() == 2, "Task 2 runs on a timer interrupt to preempt Task 1.");

    passert(scheduler.onTimerInterrupt(&t2)->getIdentifier() == 3, "Task 3 runs on a timer interrupt to preempt Task 2.");
}
