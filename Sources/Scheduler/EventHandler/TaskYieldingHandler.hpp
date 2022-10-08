//
//  TaskYieldingHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Scheduler_TaskYieldingHandler_hpp
#define Scheduler_TaskYieldingHandler_hpp

#include <Scheduler/Misc/Traits.hpp>

/// Defines the common task termination handler
namespace Scheduler::EventHandlers::TaskYielding::Common
{
    ///
    /// A handler that puts the current task into the ready queue and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    ///
    template <typename ConcreteScheduler>
    struct RunNext
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that the current running task has voluntarily relinquished the CPU
        ///
        /// @param current The current running task that just yielded
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskYielded(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Enqueue the yielded task
            self->ready(current);

            // Dequeue the next ready task
            return self->next();
        }
    };
}

#endif /* Scheduler_TaskYieldingHandler_hpp */
