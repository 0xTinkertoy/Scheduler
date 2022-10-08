//
//  MultilevelFeedbackQueueSchedulerTest.cpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#include "MultilevelFeedbackQueueSchedulerTest.hpp"
#include "SimpleTask.hpp"
#include "SampleSchedulers.hpp"
#include <Debug.hpp>

namespace Schedulers = SampleSchedulers;

void MultilevelFeedbackQueueSchedulerTest::runPrimitivesTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 2);

    SimpleTask t3(3, 3);
    
    Schedulers::MultilevelFeedbackQueue<SimpleTask, SimpleTask::QuantumSpecifier, 3> scheduler(&idleTask);

    // Empty queue
    passert(scheduler.next() == nullptr, "Empty queue");

    // Check quantum allocation: Task 1
    passert(t1.hasUsedUpTimeAllotment(), "Before: Task 1 has 0 quantum.");

    scheduler.ready(&t1);

    passert(!t1.hasUsedUpTimeAllotment(), "After: Task 1 has UINT32_MAX quantum.");

    t1.print();

    // Check quantum allocation: Task 2
    passert(t2.hasUsedUpTimeAllotment(), "Before: Task 2 has 0 quantum.");

    scheduler.ready(&t2);

    passert(!t2.hasUsedUpTimeAllotment(), "After: Task 2 has 2 quantum.");

    t2.print();

    // Check quantum allocation: Task 3
    passert(t3.hasUsedUpTimeAllotment(), "Before: Task 3 has 0 quantum.");

    scheduler.ready(&t3);

    passert(!t3.hasUsedUpTimeAllotment(), "After: Task 3 has 1 quantum.");

    t3.print();

    // Dequeue
    for (uint32_t index = 1; index < 4; index += 1)
    {
        SimpleTask* task = scheduler.next();

        passert(task->getIdentifier() == (4 - index), "Task %u is ready.", 4 - index);

        passert(task->getPriority() == (4 - index), "Task %u should have priority level unchanged.", 4 - index);

        pinfo("Next()");

        task->print();
    }

    // Empty queue
    passert(scheduler.next() == nullptr, "Empty queue");
}

void MultilevelFeedbackQueueSchedulerTest::runTaskManagerDelegateTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 2);

    SimpleTask t3(3, 3);

    Schedulers::MultilevelFeedbackQueue<SimpleTask, SimpleTask::QuantumSpecifier, 3> scheduler(&idleTask);

    // Idle task is running
    // Task 2 arrives
    passert(scheduler.onTaskCreated(&idleTask, &t2)->getIdentifier() == 2,
            "Task 2 arrives.");

    // Task 1 arrives
    passert(scheduler.onTaskCreated(&t2, &t1)->getIdentifier() == 2,
            "Task 1 arrives but cannot preempt Task 2 due to a lower priority.");

    // Task 3 arrives
    passert(scheduler.onTaskCreated(&t2, &t3)->getIdentifier() == 3,
            "Task 3 arrives and preempts Task 2 due to a higher priority.");

    // Task 3 is blocked
    passert(scheduler.onTaskBlocked(&t3)->getIdentifier() == 2,
            "Task 2 runs after Task 3 has been blocked.");

    passert(t3.getPriority() == 3, "Task 3 should have priority unchanged.");

    // Task 2 is blocked
    passert(scheduler.onTaskBlocked(&t2)->getIdentifier() == 1,
            "Task 1 runs after Task 2 has been blocked.");

    passert(t2.getPriority() == 2, "Task 2 should have priority unchanged.");

    // Task 1 is blocked
    passert(scheduler.onTaskBlocked(&t1)->getIdentifier() == 0,
            "Idle task runs after Task 1 has been blocked.");

    passert(t1.getPriority() == 1, "Task 1 should have priority unchanged.");
}

void MultilevelFeedbackQueueSchedulerTest::runTimerInterruptDelegateTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 2);

    SimpleTask t3(3, 3);

    Schedulers::MultilevelFeedbackQueue<SimpleTask, SimpleTask::QuantumSpecifier, 3> scheduler(&idleTask);

    // Task 1, 2 are ready while Task 3 is running
    scheduler.ready(&t1);

    scheduler.ready(&t2);

    scheduler.ready(&t3);

    SimpleTask* running = scheduler.next();

    // Timer interrupt
    // Task 3 has used up all quantum and is demoted to level 2
    running = scheduler.onTimerInterrupt(running);

    passert(t3.getPriority() == 2, "Task 3 is demoted to level 2 since it has used up all quantum.");

    passert(running->getIdentifier() == 2, "Task 2 is selected to run after Task 3 has been demoted.");

    // Timer interrupt
    // Task 2 has used 1 quantum and has 1 quantum left
    running = scheduler.onTimerInterrupt(running);

    passert(running->getIdentifier() == 2, "Task 2 still runs because it has 1 quantum left.");

    passert(running->getPriority() == 2, "Task 2 should have priority level unchanged.");

    // Timer interrupt
    // Task 2 has used up all quantum and is demoted to level 1
    running = scheduler.onTimerInterrupt(running);

    passert(running->getIdentifier() == 3, "Task 3 runs after Task 2 has been demoted.");

    passert(running->getPriority() == 2, "Task 3 should have priority level 2.");

    passert(t2.getPriority() == 1, "Task 2 is demoted to priority level 1.");

    // Timer interrupt
    // Task 3 has used 1 quantum and has 1 quantum left
    running = scheduler.onTimerInterrupt(running);

    passert(running->getIdentifier() == 3, "Task 3 still runs because it has 1 quantum left.");

    passert(running->getPriority() == 2, "Task 3 should have priority level unchanged.");

    // Timer interrupt
    // Task 3 has used up all quantum and is demoted to level 1
    running = scheduler.onTimerInterrupt(running);

    passert(running->getIdentifier() == 1, "Task 1 runs after Task 3 has been demoted.");

    passert(running->getPriority() == 1, "Task 1 should have priority level 1.");

    passert(t3.getPriority() == 1, "Task 3 is demoted to priority level 1.");

    // Tasks who have priority level 1 will run to completion.
}

void MultilevelFeedbackQueueSchedulerTest::runGroupOperationsTest()
{
    // Test Setup
    SimpleTask idleTask(0, 0);

    SimpleTask t1(1, 1);

    SimpleTask t2(2, 2);

    SimpleTask t3(3, 3);

    Schedulers::MultilevelFeedbackQueue<SimpleTask, SimpleTask::QuantumSpecifier, 3> scheduler(&idleTask);

    scheduler.ready(&t1);

    passert(scheduler.next()->getIdentifier() == 1, "Test Preparation.");

    // Task 1 is running
    // A timer interrupt occurs
    // Task 2 and Task 3 are unblocked
    passert(scheduler.onTaskUnblocked(nullptr, &t2) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskUnblocked(nullptr, &t3) == nullptr, "Intermediate unblock call.");

    // Simulate reentrancy
    // The timer interrupt handler keeps the current task running
    passert(scheduler.onTimerInterrupt(&t1)->getIdentifier() == 1, "Task 1 keeps running on a timer interrupt.");

    // The kernel finishes processing the timer interrupt
    // Switch the context back to the previous kernel context
    passert(scheduler.onTaskUnblocked(&t1, nullptr)->getIdentifier() == 3, "Task 3 now preempts task 1.");

    // Task 3 has finished
    // Task 4 and Task 5 are unblocked
    SimpleTask t4(4, 3);

    SimpleTask t5(5, 3);

    passert(scheduler.onTaskUnblocked(nullptr, &t4) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskUnblocked(nullptr, &t5) == nullptr, "Intermediate unblock call.");

    passert(scheduler.onTaskFinished(&t3)->getIdentifier() == 4, "Task 4 starts to run after Task 3 has finished.");

    passert(scheduler.onTaskFinished(&t4)->getIdentifier() == 5, "Task 5 starts to run after Task 4 has finished.");
}
