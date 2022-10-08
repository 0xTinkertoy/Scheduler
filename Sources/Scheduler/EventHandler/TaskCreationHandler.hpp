//
//  TaskCreationHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef TaskCreationHandler_hpp
#define TaskCreationHandler_hpp

#include <Scheduler/Constraint/Prioritizable.hpp>
#include <Scheduler/Misc/Traits.hpp>
#include <Scheduler/Misc/Utils.hpp>

/// Defines all preemptive task creation handlers
namespace Scheduler::EventHandlers::TaskCreation::Preemptive
{
    ///
    /// A handler that compares the priority of the current task against the newly created one,
    /// selects the one that has the higher priority to run and puts the lower one into the ready queue
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `RunHigherPriorityWithIdleTaskSupport` to deal with the idle task properly.
    ///
    template <typename ConcreteScheduler>
    requires TaskConstraints::AnyPrioritizable<Traits::ScheduledTask<ConcreteScheduler>>
    struct RunHigherPriority
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a new task has been created
        ///
        /// @param current The current running task
        /// @param task The newly created task
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskCreated(Task* current, Task* task)
        {
            // Check whether the new task has a higher priority than the current one
            std::pair<Task*, Task*> result = Utilities::orderByPriority(current, task);

            // Enqueue the task that has a lower priority
            auto self = static_cast<ConcreteScheduler*>(this);

            self->ready(result.second);

            // Run the task that has a higher priority
            return result.first;
        }
    };

    ///
    /// A handler that compares the priority of the current task against the newly created one,
    /// selects the one that has the higher priority to run and puts the lower one into the ready queue
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler takes the idle task into consideration.
    ///
    template <typename ConcreteScheduler>
    requires TaskConstraints::AnyPrioritizable<Traits::ScheduledTask<ConcreteScheduler>>
    struct RunHigherPriorityWithIdleTaskSupport
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a new task has been created
        ///
        /// @param current The current running task
        /// @param task The newly created task
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskCreated(Task* current, Task* task)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Guard: Check whether the current task is the idle task
            if (current == self->getIdleTask())
            {
                // Run the newly created task directly
                return task;
            }

            // Check whether the new task has a higher priority than the current one
            std::pair<Task*, Task*> result = Utilities::orderByPriority(current, task);

            // Enqueue the task that has a lower priority
            self->ready(result.second);

            // Run the task that has a higher priority
            return result.first;
        }
    };
}

/// Defines all cooperative task creation handlers
namespace Scheduler::EventHandlers::TaskCreation::Cooperative
{
    ///
    /// A handler that lets the current task keep running after putting the new task into the ready queue
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `KeepRunningCurrentWithIdleTaskSupport` to deal with the idle task properly.
    ///
    template <typename ConcreteScheduler>
    struct KeepRunningCurrent
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a new task has been created
        ///
        /// @param current The current running task
        /// @param task The newly created task
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskCreated(Task* current, Task* task)
        {
            // Enqueue the newly created task
            auto self = static_cast<ConcreteScheduler*>(this);

            self->ready(task);

            // The current task keeps running
            return current;
        }
    };

    ///
    /// A handler that lets the current task keep running after putting the new task into the ready queue
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler takes the idle task into consideration.
    ///
    template <typename ConcreteScheduler>
    struct KeepRunningCurrentWithIdleTaskSupport
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a new task has been created
        ///
        /// @param current The current running task
        /// @param task The newly created task
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskCreated(Task* current, Task* task)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Guard: Check whether the current task is the idle task
            if (current == self->getIdleTask())
            {
                // Run the newly created task
                return task;
            }

            // Enqueue the newly created task
            self->ready(task);

            // The current task keeps running
            return current;
        }
    };
}

#endif /* TaskCreationHandler_hpp */
