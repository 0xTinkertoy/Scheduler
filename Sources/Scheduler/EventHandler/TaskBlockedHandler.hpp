//
//  TaskBlockedHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef TaskBlockedHandler_hpp
#define TaskBlockedHandler_hpp

#include <Scheduler/Misc/Traits.hpp>

/// Defines the common task termination handler
namespace Scheduler::EventHandlers::TaskBlocked::Common
{
    ///
    /// A handler that selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @warning This handler does not take the idle task into consideration.
    /// @seealso `RunNextWithIdleTaskSupport` to deal with the idle task properly.
    ///
    template <typename ConcreteScheduler>
    struct RunNext
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that the current running task has been blocked
        ///
        /// @param current The current running task that is blocked
        /// @returns A task that is selected to run, `nullptr` if no task is ready.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskBlocked([[maybe_unused]] Task* current)
        {
            // Dequeue the next ready task
            auto self = static_cast<ConcreteScheduler*>(this);

            return self->next();
        }
    };

    ///
    /// A handler that selects the next task to run
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
        /// Notify the delegate that the current running task has been blocked
        ///
        /// @param current The current running task that is blocked
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskBlocked([[maybe_unused]] Task* current)
        {
            // Dequeue the next ready task
            auto self = static_cast<ConcreteScheduler*>(this);

            Task* next = self->next();

            return next == nullptr ? self->getIdleTask() : next;
        }
    };
}

#endif /* TaskBlockedHandler_hpp */
