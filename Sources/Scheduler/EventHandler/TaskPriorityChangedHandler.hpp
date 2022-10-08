//
//  TaskPriorityChangedHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Scheduler_TaskPriorityChangedHandler_hpp
#define Scheduler_TaskPriorityChangedHandler_hpp

#include <Scheduler/Constraint/Prioritizable.hpp>
#include <Scheduler/Misc/Traits.hpp>

/// Defines the handlers that deal with the task of which priority level has been changed
namespace Scheduler::EventHandlers::TaskPriorityChanged::Preemptive
{
    ///
    /// A handler that adjusts the position of the task whose priority level has been changed and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    ///
    template <typename ConcreteScheduler>
    struct Balance
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        /// Type of the task priority level
        using Priority = Traits::TaskPriority<Task>;

        ///
        /// Notify the delegate that the priority level of a task has been changed
        ///
        /// @param current The current running task
        /// @param task The task whose priority level has been changed
        /// @param oldPriority The previous priority level
        /// @return The task that is selected to run.
        /// @note This method does NOT support group operations.
        /// @warning The task of which priority level has been changed must not be identical to the current running task.
        ///          If a task has requested to change its priority level, the caller should use the "self priority changed handler" instead.
        /// @warning Since the scheduler is only responsible for ready tasks,
        ///          the task of which priority level has been changed must be ready and reside in the ready queue.
        /// @seealso `TaskPrioritySelfChangedHandler` to deal with the task who has changed its priority level.
        ///
        Task* onTaskPriorityChanged(Task* current, Task* task, const Priority& oldPriority)
        {
            // Retrieve the current scheduling policy
            auto self = static_cast<ConcreteScheduler*>(this);

            // Precondition: `task` is already in the ready queue
            // Adjust its position in the queue
            self->adjustPosition(task, oldPriority);

            // Check whether the task now has a higher priority than the current running one
            if (AnyPrioritizableTask(task) > AnyPrioritizableTask(current))
            {
                self->ready(current);

                return self->next();
            }
            else
            {
                // The current task should keep running
                return current;
            }
        }
    };
}

#endif /* Scheduler_TaskPriorityChangedHandler_hpp */
