//
//  TimerInterruptHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Scheduler_TimerInterruptHandler_hpp
#define Scheduler_TimerInterruptHandler_hpp

#include <Scheduler/Misc/Traits.hpp>

/// Defines all preemptive timer interrupt handlers
namespace Scheduler::EventHandlers::TimerInterrupt::Preemptive
{
    ///
    /// A handler that puts the current task into the ready queue and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of schedulable tasks managed by the scheduler
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `RunNextWithIdleTaskSupport` to deal with the idle task properly.
    ///
    template <typename ConcreteScheduler>
    struct RunNext
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a timer interrupt has occurred
        ///
        /// @param current The current running task
        /// @returns The non-null task that is selected to run.
        ///
        Task* onTimerInterrupt(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            self->ready(current);

            return self->next();
        }
    };

    ///
    /// A handler that puts the current task into the ready queue and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler takes the idle task into consideration.
    ///
    template <typename ConcreteScheduler>
    struct RunNextWithIdleTaskSupport
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a timer interrupt has occurred
        ///
        /// @param current The current running task
        /// @returns The non-null task that is selected to run.
        ///
        Task* onTimerInterrupt(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // The ready queue might be modified before this method is called
            // Guard: Check whether the current task is the idle task
            if (current != self->getIdleTask())
            {
                // Enqueue the current task
                self->ready(current);
            }

            // Get the next ready task from the queue
            Task* nextTask = self->next();

            return nextTask == nullptr ? self->getIdleTask() : nextTask;
        }
    };

    ///
    /// A handler that keeps the current running with a custom ticks used up handler
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `KeepRunningCurrentWithAnyQuantumUsedUpHandlerAndIdleTaskSupport` to deal with the idle task properly.
    ///
    template <typename ConcreteScheduler>
    struct KeepRunningCurrentWithAnyQuantumUsedUpHandler
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a timer interrupt has occurred
        ///
        /// @param current The current running task
        /// @returns The non-null task that is selected to run.
        ///
        Task* onTimerInterrupt(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // The current running task has run for a tick
            current->tick();

            // Guard: Demote the task if it has used up all ticks
            if (current->hasUsedUpTimeAllotment())
            {
                return self->onTaskQuantumUsedUp(current);
            }

            // Keep running the current task
            return current;
        }
    };

    ///
    /// A handler that keeps the current running with a custom ticks used up handler
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler takes the idle task into consideration.
    ///
    template <typename ConcreteScheduler>
    struct KeepRunningCurrentWithAnyQuantumUsedUpHandlerAndIdleTaskSupport
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a timer interrupt has occurred
        ///
        /// @param current The current running task
        /// @returns The non-null task that is selected to run.
        ///
        Task* onTimerInterrupt(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // The ready queue might be modified before this method is called
            // Guard: Check whether the current task is the idle task
            if (current == self->getIdleTask())
            {
                Task* next = self->next();

                return next == nullptr ? self->getIdleTask() : next;
            }

            // The current running task has run for a tick
            current->tick();

            // Guard: Demote the task if it has used up all ticks
            if (current->hasUsedUpTimeAllotment())
            {
                return self->onTaskQuantumUsedUp(current);
            }

            // Keep running the current task
            return current;
        }
    };

    ///
    /// A handler that keeps the current task running unless it has used up its ticks
    /// In this case, the handler demotes the current task and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `KeepRunningCurrentWithAutoDemotionOnQuantumUsedUpAndIdleTaskSupport` to deal with the idle task properly.
    ///
    template <typename ConcreteScheduler>
    struct KeepRunningCurrentWithAutoDemotionOnQuantumUsedUp:
            public KeepRunningCurrentWithAnyQuantumUsedUpHandler<ConcreteScheduler>,
            public TaskQuantumUsedUp::Preemptive::RunNextWithDemotion<ConcreteScheduler> {};

    ///
    /// A handler that keeps the current task running unless it has used up its ticks
    /// In this case, the handler demotes the current task and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler takes the idle task into consideration.
    ///
    template <typename ConcreteScheduler>
    struct KeepRunningCurrentWithAutoDemotionOnQuantumUsedUpAndIdleTaskSupport:
            public KeepRunningCurrentWithAnyQuantumUsedUpHandlerAndIdleTaskSupport<ConcreteScheduler>,
            public TaskQuantumUsedUp::Preemptive::RunNextWithDemotion<ConcreteScheduler> {};

    ///
    /// A handler that keeps the current task running unless it has used up its ticks
    /// In this case, the handler recharges its ticks based on its priority level and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @tparam CustomQuantumSpecifier A callable type that maps a priority level to a certain amount of time ticks
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `KeepRunningCurrentWithAutoRechargeOnQuantumUsedUpAndIdleTaskSupport` to deal with the idle task properly.
    ///
    template <typename ConcreteScheduler, typename CustomQuantumSpecifier>
    struct KeepRunningCurrentWithAutoRechargeOnQuantumUsedUp:
            public KeepRunningCurrentWithAnyQuantumUsedUpHandler<ConcreteScheduler>,
            public TaskQuantumUsedUp::Preemptive::RunNextWithQuantumRecharged<ConcreteScheduler, CustomQuantumSpecifier> {};

    ///
    /// A handler that keeps the current task running unless it has used up its ticks
    /// In this case, the handler recharges its ticks based on its priority level and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @tparam CustomQuantumSpecifier A callable type that maps a priority level to a certain amount of time ticks
    /// @warning This handler takes the idle task into consideration.
    ///
    template <typename ConcreteScheduler, typename CustomQuantumSpecifier>
    struct KeepRunningCurrentWithAutoRechargeOnQuantumUsedUpAndIdleTaskSupport:
            public KeepRunningCurrentWithAnyQuantumUsedUpHandlerAndIdleTaskSupport<ConcreteScheduler>,
            public TaskQuantumUsedUp::Preemptive::RunNextWithQuantumRecharged<ConcreteScheduler, CustomQuantumSpecifier> {};
}

/// Defines all cooperative timer interrupt handlers
namespace Scheduler::EventHandlers::TimerInterrupt::Cooperative
{
    ///
    /// A handler that lets the current task keep running
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    ///
    template <typename ConcreteScheduler>
    struct KeepRunningCurrent
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a timer interrupt has occurred
        ///
        /// @param current The current running task
        /// @returns The non-null task that is selected to run.
        ///
        Task* onTimerInterrupt(Task* current)
        {
            return current;
        }
    };
}

#endif /* Scheduler_TimerInterruptHandler_hpp */
