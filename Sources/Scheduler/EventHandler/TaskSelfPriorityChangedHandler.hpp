//
//  TaskSelfPriorityChangedHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Scheduler_TaskSelfPriorityChangedHandler_hpp
#define Scheduler_TaskSelfPriorityChangedHandler_hpp

#include <Scheduler/Misc/Traits.hpp>

/// Defines the handlers that deal with the task who has requested to change its priority level
namespace Scheduler::EventHandlers::TaskSelfPriorityChanged::Preemptive
{
    ///
    /// A handler that deals with tasks who have requested to change their priority level
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    ///
    template <typename ConcreteScheduler>
    struct Balance
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that the priority level of a task has been changed
        ///
        /// @param current The current running task that has requested to change its priority level
        /// @return The task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskPriorityChanged(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            self->ready(current);

            return self->next();
        }
    };
}

#endif /* Scheduler_TaskSelfPriorityChangedHandler_hpp */
