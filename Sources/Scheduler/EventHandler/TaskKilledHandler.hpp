//
//  TaskKilledHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef TaskKilledHandler_hpp
#define TaskKilledHandler_hpp

#include <Scheduler/Misc/Traits.hpp>

/// Defines the common task killed handler
namespace Scheduler::EventHandlers::TaskKilled::Common
{
    ///
    /// A handler that removes the task being killed from the ready queue and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    ///
    template <typename ConcreteScheduler>
    struct KeepRunningCurrent
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that a task has been killed
        ///
        /// @param current The current running task
        /// @param task The task that just got killed
        /// @return The task that is selected to run if requested.
        /// @warning The task being killed must not be identical to the current running task.
        ///          If a task kills itself, the caller should use the task termination handler instead.
        /// @seealso `TaskTerminationHandler` to deal with the task who kills itself.
        /// @warning Since the scheduler is only responsible for ready tasks,
        ///          the task being killed must be ready and reside in the ready queue.
        /// @note This method supports group operations.
        ///       1) Pass `nullptr` to `current` to remove `task` from the ready queue only.
        ///          In this case, this method returns `nullptr` back to the caller.
        ///       2) Pass `nullptr` to `task` to fetch the next task.
        ///       3) Pass a non-null task to both parameters to remove the task being killed from the ready queue and fetch the next task.
        ///          In the above two cases, this method returns a task that is ready to run,
        ///          indicating that group operations are completed. The caller should not have any subsequent calls.
        ///
        Task* onTaskKilled(Task* current, Task* task)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Guard: The current running task must not be identical to the task being killed
            passert(current != task, "Usage Error: The current running task cannot be identical to the task being killed.");

            // Guard: [Special] Check whether the caller performs an intermediate call
            if (current == nullptr)
            {
                // Intermediate call
                self->remove(task);

                return nullptr;
            }

            // Guard: [Special] Check whether the caller only wants to fetch the next task
            if (task != nullptr)
            {
                // Remove the task being killed
                self->remove(task);
            }

            // Default: The current running task keeps running
            return current;
        }
    };
}

#endif /* TaskKilledHandler_hpp */
