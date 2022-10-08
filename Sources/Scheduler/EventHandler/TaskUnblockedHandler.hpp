//
//  TaskUnblockedHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Scheduler_TaskUnblockedHandler_hpp
#define Scheduler_TaskUnblockedHandler_hpp

#include <Scheduler/Misc/Traits.hpp>

/// Defines all preemptive task unblocked handler
namespace Scheduler::EventHandlers::TaskUnblocked::Preemptive
{
    ///
    /// A handler that preempts the current running task and selects the next ready task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `RunNextWithIdleTaskSupport` to deal with the idle task properly.
    /// @note This handler is useful when the scheduler prioritizes ready tasks.
    ///
    template <typename ConcreteScheduler>
    struct RunNext
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a task has been unblocked
        ///
        /// @param current The current running task
        /// @param task The task that just got unblocked
        /// @returns The non-null task that is selected to run if requested.
        /// @note This method supports group operations.
        ///       1) Pass `nullptr` to `current` to enqueue `task` only.
        ///          In this case, this method returns `nullptr` back to the caller.
        ///       2) Pass `nullptr` to `task` to fetch the next task.
        ///       3) Pass a non-null task to both parameters to enqueue the unblocked task and fetch the next task.
        ///          In the above two cases, this method returns a non-null task that is ready to run,
        ///          indicating that group operations are completed. The caller should not have any subsequent calls.
        ///
        Task* onTaskUnblocked(Task* current, Task* task)
        {
            // Retrieve the current scheduling policy
            auto self = static_cast<ConcreteScheduler*>(this);

            // Guard: [Special] Check whether the caller performs an intermediate call
            if (current == nullptr)
            {
                // Intermediate call
                self->ready(task);

                return nullptr;
            }

            // Guard: [Special] Check whether the caller only wants to fetch the next task
            if (task != nullptr)
            {
                self->ready(task);
            }

            // Default: Get the next ready task
            return self->next();
        }
    };

    ///
    /// A handler that preempts the current running task and selects the next ready task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `RunNextWithIdleTaskSupport` to deal with the idle task properly.
    /// @note This handler is useful when the scheduler prioritizes ready tasks.
    ///
    template <typename ConcreteScheduler>
    struct RunNextWithIdleTaskSupport
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a task has been unblocked
        ///
        /// @param current The current running task
        /// @param task The task that just got unblocked
        /// @returns The non-null task that is selected to run if requested.
        /// @note This method supports group operations.
        ///       1) Pass `nullptr` to `current` to enqueue `task` only.
        ///          In this case, this method returns `nullptr` back to the caller.
        ///       2) Pass `nullptr` to `task` to fetch the next task.
        ///       3) Pass a non-null task to both parameters to enqueue the unblocked task and fetch the next task.
        ///          In the above two cases, this method returns a non-null task that is ready to run,
        ///          indicating that group operations are completed. The caller should not have any subsequent calls.
        ///
        Task* onTaskUnblocked(Task* current, Task* task)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Guard: [Special] Check whether the caller performs an intermediate call
            if (current == nullptr)
            {
                // Intermediate call
                self->ready(task);

                return nullptr;
            }

            // Guard: [Special] Check whether the caller only wants to fetch the next task
            if (task != nullptr)
            {
                self->ready(task);
            }

            // Default: Get the next ready task
            // Guard: Check whether the current running task is the idle task
            if (current != self->getIdleTask())
            {
                self->ready(current);
            }

            Task* next = self->next();

            return next == nullptr ? self->getIdleTask() : next;
        }
    };
}

/// Defines all cooperative task unblocked handler
namespace Scheduler::EventHandlers::TaskUnblocked::Cooperative
{
    ///
    /// A handler that lets the current task keep running while putting the unblocked task into the ready queue
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
        /// Notify the delegate that a task has been unblocked
        ///
        /// @param current The current running task
        /// @param task The task that just got unblocked
        /// @returns The non-null task that is selected to run if requested.
        /// @note This method supports group operations.
        ///       1) Pass `nullptr` to `current` to enqueue `task` only.
        ///          In this case, this method returns `nullptr` back to the caller.
        ///       2) Pass `nullptr` to `task` to fetch the next task.
        ///       3) Pass a non-null task to both parameters to enqueue the unblocked task and fetch the next task.
        ///          In the above two cases, this method returns a non-null task that is ready to run,
        ///          indicating that group operations are completed. The caller should not have any subsequent calls.
        ///
        Task* onTaskUnblocked(Task* current, Task* task)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Guard: [Special] Check whether the caller performs an intermediate call
            if (current == nullptr)
            {
                // Intermediate call
                self->ready(task);

                return nullptr;
            }

            // Guard: [Special] Check whether the caller only wants to fetch the next task
            if (task != nullptr)
            {
                // Enqueue the unblocked task
                self->ready(task);
            }

            // The current running task keeps running
            return current;
        }
    };

    ///
    /// A handler that lets the current task keep running while putting the unblocked task into the ready queue
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
        /// Notify the delegate that a task has been unblocked
        ///
        /// @param current The current running task
        /// @param task The task that just got unblocked
        /// @returns The non-null task that is selected to run if requested.
        /// @note This method supports group operations.
        ///       1) Pass `nullptr` to `current` to enqueue `task` only.
        ///          In this case, this method returns `nullptr` back to the caller.
        ///       2) Pass `nullptr` to `task` to fetch the next task.
        ///       3) Pass a non-null task to both parameters to enqueue the unblocked task and fetch the next task.
        ///          In the above two cases, this method returns a non-null task that is ready to run,
        ///          indicating that group operations are completed. The caller should not have any subsequent calls.
        ///
        Task* onTaskUnblocked(Task* current, Task* task)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Guard: [Special] Check whether the caller performs an intermediate call
            if (current == nullptr)
            {
                // Intermediate call
                self->ready(task);

                return nullptr;
            }

            // Guard: [Special] Check whether the caller only wants to fetch the next task
            if (task == nullptr)
            {
                // The current running task keeps running
                return current;
            }

            // Default: Ready queue might be modified before this method is called
            // Enqueue the unblocked task
            self->ready(task);

            // Guard: Check whether the current task is the idle task
            if (current == self->getIdleTask())
            {
                // Get the next ready task from the queue
                return self->next();
            }
            else
            {
                // The current running task keeps running
                return current;
            }
        }
    };
}

#endif /* Scheduler_TaskUnblockedHandler_hpp */
