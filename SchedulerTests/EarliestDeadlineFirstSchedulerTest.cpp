//
//  EarliestDeadlineFirstSchedulerTest.cpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-18.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#include "EarliestDeadlineFirstSchedulerTest.hpp"
#include "SimpleRealtimeTask.hpp"
#include "SampleSchedulers.hpp"
#include <Debug.hpp>

namespace Schedulers = SampleSchedulers;

void EarliestDeadlineFirstSchedulerTest::runPrimitivesTest()
{

}

void EarliestDeadlineFirstSchedulerTest::runTaskManagerDelegateTest()
{

}

void EarliestDeadlineFirstSchedulerTest::runTimerInterruptDelegateTest()
{
    // Test Setup
    SimpleRealtimeTask idleTask(0, UINT32_MAX);

    Schedulers::EarliestDeadlineFirst<SimpleRealtimeTask> scheduler(&idleTask);

    // Schedulable Task Set with 95% processor utilization
    // ----------------------------------------------------------
    // Task | Release Time | Execution Time | Deadline | Period |
    // ----------------------------------------------------------
    //  T1  |      0       |        1       |     4    |    4   |
    //  T2  |      0       |        2       |     6    |    6   |
    //  T3  |      0       |        3       |     8    |    8   |
    // ----------------------------------------------------------

    // At t = 0, idle task is running
    // Task 1, 2, 3 are released
    SimpleRealtimeTask t1p1(1, 4);

    SimpleRealtimeTask t2p1(2, 6);

    SimpleRealtimeTask t3p1(3, 8);

    passert(scheduler.onTaskCreated(&idleTask, &t1p1)->getIdentifier() == 1, "Task 1 is created.");

    passert(scheduler.onTaskCreated(&t1p1, &t2p1)->getIdentifier() == 1, "Task 1 still runs after Task 2 is created.");

    passert(scheduler.onTaskCreated(&t1p1, &t3p1)->getIdentifier() == 1, "Task 1 still runs after Task 3 is created.");

    passert(scheduler.onTimerInterrupt(&t1p1)->getIdentifier() == 1, "Task 1 will soon finish running.");

    // At t = 1, Task 1 has finished
    // Task 2 starts to run since it has an earlier deadline
    passert(scheduler.onTaskFinished(&t1p1)->getIdentifier() == 2, "Task 2 starts to run after Task 1 has finished.");

    // At t = 2, Task 2 is still running and has not finished
    passert(scheduler.onTimerInterrupt(&t2p1)->getIdentifier() == 2, "Task 2 is still running.");

    // At t = 3, Task 2 has finished
    // Task 3 starts to run since it is the only task in the queue
    passert(scheduler.onTimerInterrupt(&t2p1)->getIdentifier() == 2, "Task 2 will soon finish running.");

    passert(scheduler.onTaskFinished(&t2p1)->getIdentifier() == 3, "Task 3 starts to run after Task 2 has finished.");

    // At t = 4, Task 1 is created due to its period of 4
    // Task 3 still runs, even though it has the same deadline as Task 1
    // i.e. Rule to break the tie: First come, first serve
    SimpleRealtimeTask t1p2(1, 8);

    passert(scheduler.onTimerInterrupt(&t3p1)->getIdentifier() == 3, "Task 3 is still running.");

    passert(scheduler.onTaskCreated(&t3p1, &t1p2)->getIdentifier() == 3,
            "Task 3 keeps running since it has the same deadline as Task 1 but enters the system earlier.");

    // At t = 5, Task 3 is still running and has not finished
    passert(scheduler.onTimerInterrupt(&t3p1)->getIdentifier() == 3, "Task 3 is still running.");

    // At t = 6, Task 3 has finished
    // Meanwhile, Task 2 is created due to its period of 6
    // Task 1 is selected to run, because it has an earlier deadline (8) than Task 2 (12).
    passert(scheduler.onTimerInterrupt(&t3p1)->getIdentifier() == 3, "Task 3 will soon finish running.");

    passert(scheduler.onTaskFinished(&t3p1)->getIdentifier() == 1, "Task 1 starts to run after Task 3 has finished.");

    SimpleRealtimeTask t2p2(2, 12);

    passert(scheduler.onTaskCreated(&t1p2, &t2p2)->getIdentifier() == 1, "Task 1 keeps running after Task 2 is created.");

    // At t = 7, Task 1 has finished
    // Task 2 starts to run
    passert(scheduler.onTimerInterrupt(&t1p2)->getIdentifier() == 1, "Task 1 will soon finish running.");

    passert(scheduler.onTaskFinished(&t1p2)->getIdentifier() == 2, "Task 2 starts to run after Task 1 has finished.");

    // At t = 8, Task 1 and Task 3 are created due to their periods
    // Currently, task 1's deadline = 12, task 2's deadline = 12, task 3's deadline = 16
    // Task 2 still runs, because it has the earlier deadline than Task 3 and arrives before Task 1.
    passert(scheduler.onTimerInterrupt(&t2p2)->getIdentifier() == 2, "Task 2 is still running.");

    SimpleRealtimeTask t1p3(1, 12);

    SimpleRealtimeTask t3p2(3, 16);

    passert(scheduler.onTaskCreated(&t2p2, &t1p3)->getIdentifier() == 2, "Task 2 keeps running after Task 1 is created.");

    passert(scheduler.onTaskCreated(&t2p2, &t3p2)->getIdentifier() == 2, "Task 2 keeps running after Task 3 is created.");

    // At t = 9, Task 2 has finished
    // Task 1 is selected to run, because it has an earlier deadline than Task 3
    passert(scheduler.onTimerInterrupt(&t2p2)->getIdentifier() == 2, "Task 2 will soon finish running.");

    passert(scheduler.onTaskFinished(&t2p2)->getIdentifier() == 1, "Task 1 starts to run after Task 2 has finished.");

    // At t = 10, Task 1 has finished
    // Task 3 is selected to run, because it is the only task in the queue
    passert(scheduler.onTimerInterrupt(&t1p3)->getIdentifier() == 1, "Task 1 will soon finish running.");

    passert(scheduler.onTaskFinished(&t1p3)->getIdentifier() == 3, "Task 3 starts to run after Task 1 has finished.");

    // At t = 11, Task 3 is still running
    passert(scheduler.onTimerInterrupt(&t3p2)->getIdentifier() == 3, "Task 3 is still running.");

    // At t = 12, Task 1 and Task 2 are created due to their periods
    // Currently, task 1's deadline = 16, task 2's deadline = 18, task 3's deadline = 16
    // Task 3 is still running, because it has an earlier deadline than Task 2 and arrives before Task 1
    passert(scheduler.onTimerInterrupt(&t3p2)->getIdentifier() == 3, "Task 3 is still running.");

    SimpleRealtimeTask t1p4(1, 16);

    SimpleRealtimeTask t2p3(2, 18);

    passert(scheduler.onTaskCreated(&t3p2, &t1p4)->getIdentifier() == 3, "Task 3 keeps running after Task 1 is created.");

    passert(scheduler.onTaskCreated(&t3p2, &t2p3)->getIdentifier() == 3, "Task 3 keeps running after Task 2 is created.");

    // At t = 13, Task 3 has finished
    // Task 1 is selected to run, because it has an earlier deadline than Task 2
    passert(scheduler.onTimerInterrupt(&t3p2)->getIdentifier() == 3, "Task 3 will soon finish running.");

    passert(scheduler.onTaskFinished(&t3p2)->getIdentifier() == 1, "Task 1 starts to run after Task 3 has finished.");

    // At t = 14, Task 1 has finished
    // Task 2 is selected to run, because it is the only task in the queue
    passert(scheduler.onTimerInterrupt(&t1p4)->getIdentifier() == 1, "Task 1 will soon finish running.");

    passert(scheduler.onTaskFinished(&t1p4)->getIdentifier() == 2, "Task 2 starts to run after Task 1 has finished.");

    // At t = 15, Task 2 is still running
    passert(scheduler.onTimerInterrupt(&t2p3)->getIdentifier() == 2, "Task 2 is still running.");

    // At t = 16, Task 2 has finished
    // Meanwhile, Task 1 and Task 3 are created due to their periods
    // Task 1 is selected to run because it has an earlier deadline (20) than Task 3 (24)
    passert(scheduler.onTimerInterrupt(&t2p3)->getIdentifier() == 2, "Task 2 will soon finish running.");

    SimpleRealtimeTask t1p5(1, 20);

    SimpleRealtimeTask t3p3(3, 24);

    passert(scheduler.onTaskFinished(&t2p3)->getIdentifier() == 0, "Task 2 has finished.");

    passert(scheduler.onTaskCreated(&idleTask, &t1p5)->getIdentifier() == 1, "Task 1 starts to run after Task 2 has finished.");

    passert(scheduler.onTaskCreated(&t1p5, &t3p3)->getIdentifier() == 1, "Task 1 keeps running after Task 3 is created.");

    // At t = 17, Task 1 has finished
    // Task 3 starts to run
    passert(scheduler.onTimerInterrupt(&t1p5)->getIdentifier() == 1, "Task 1 will soon finish running.");

    passert(scheduler.onTaskFinished(&t1p5)->getIdentifier() == 3, "Task 3 starts to run after Task 1 has finished.");

    // At t = 18, Task 2 is created due to its period
    // Task 3 keeps running because it arrives before Task 2
    passert(scheduler.onTimerInterrupt(&t3p3)->getIdentifier() == 3, "Task 3 is still running.");

    SimpleRealtimeTask t2p4(2, 24);

    passert(scheduler.onTaskCreated(&t3p3, &t2p4)->getIdentifier() == 3, "Task 3 keeps running after Task 2 is created.");

    // At t = 19, Task 3 is still running
    passert(scheduler.onTimerInterrupt(&t3p3)->getIdentifier() == 3, "Task 3 is still running.");

    // At t = 20, Task 3 has finished, and Task 1 is created due to its period
    // Currently, task 1's deadline = 24, task 2's deadline = 24, task 3's deadline = 24
    // Task 2 is selected to run, because it arrives before Task 1
    passert(scheduler.onTimerInterrupt(&t3p3)->getIdentifier() == 3, "Task 3 will soon finish running.");

    SimpleRealtimeTask t1p6(1, 24);

    passert(scheduler.onTaskCreated(&t3p3, &t1p6)->getIdentifier() == 3, "Task 3 keeps running after Task 1 is created.");

    passert(scheduler.onTaskFinished(&t3p3)->getIdentifier() == 2, "Task 2 starts to run after Task 3 has finished.");

    // At t = 21, Task 2 is still running
    passert(scheduler.onTimerInterrupt(&t2p4)->getIdentifier() == 2, "Task 2 is still running.");

    // At t = 22, Task 2 has finished
    // Task 1 is selected to run, because it is the only task in the queue
    passert(scheduler.onTimerInterrupt(&t2p4)->getIdentifier() == 2, "Task 2 will soon finish.");

    passert(scheduler.onTaskFinished(&t2p4)->getIdentifier() == 1, "Task 1 starts to run after Task 2 has finished.");

    // At t = 23, Task 1 has finished
    // No task in the queue, so idle task is selected to run
    passert(scheduler.onTimerInterrupt(&t1p6)->getIdentifier() == 1, "Task 1 will soon finish running.");

    passert(scheduler.onTaskFinished(&t1p6)->getIdentifier() == 0, "Idle task starts to run after Task 1 has finished.");

    // At t = 24, Task 1, 2, 3 are released
    // Start to repeat the process from t = 0.
}

void EarliestDeadlineFirstSchedulerTest::runGroupOperationsTest()
{

}
