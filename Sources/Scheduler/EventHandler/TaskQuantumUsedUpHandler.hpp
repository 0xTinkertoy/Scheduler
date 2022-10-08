//
//  TaskQuantumUsedUpHandler.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Scheduler_TaskQuantumUsedUpHandler_hpp
#define Scheduler_TaskQuantumUsedUpHandler_hpp

#include <Scheduler/Constraint/Prioritizable.hpp>
#include <Scheduler/Constraint/Quantizable.hpp>
#include <Scheduler/Misc/Traits.hpp>

/// Defines handlers that deal with tasks when their allocated ticks is zero
namespace Scheduler::EventHandlers::TaskQuantumUsedUp::Preemptive
{
    ///
    /// A handler that demotes and enqueue the current running task and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    ///
    template <typename ConcreteScheduler>
    requires TaskConstraints::Schedulable<Traits::ScheduledTask<ConcreteScheduler>> &&
             TaskConstraints::PrioritizableByAutoMutablePriority<Traits::ScheduledTask<ConcreteScheduler>>
    struct RunNextWithDemotion
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that the current running task has used up its allocated ticks
        ///
        /// @param current The current running task whose allocated ticks is zero
        /// @return The task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskQuantumUsedUp(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);
            
            // Decrement the priority level of the current running task
            current->demote();

            // Enqueue the current task
            self->ready(current);

            // Select the next one to run
            return self->next();
        }
    };

    ///
    /// A handler that recharges the current running task with a certain amount of time ticks and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @tparam CustomQuantumSpecifier A callable type that maps a priority level to a certain amount of time ticks
    ///
    template <typename ConcreteScheduler, typename CustomQuantumSpecifier>
    requires TaskConstraints::Schedulable<Traits::ScheduledTask<ConcreteScheduler>> &&
             TaskConstraints::Quantizable<Traits::ScheduledTask<ConcreteScheduler>> &&
             TaskConstraints::PrioritizableByPriority<Traits::ScheduledTask<ConcreteScheduler>> &&
             Concepts::QuantumSpecifier<CustomQuantumSpecifier, Traits::ScheduledTask<ConcreteScheduler>>
    struct RunNextWithQuantumRecharged
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that the current running task has used up its allocated ticks
        ///
        /// @param current The current running task whose allocated ticks is zero
        /// @return The task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskQuantumUsedUp(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Reallocate the time ticks based on the priority level of the current task
            current->allocateTicks(CustomQuantumSpecifier{}(current->getPriority()));

            // Enqueue the current task
            self->ready(current);

            // Select the next one to run
            return self->next();
        }
    };

    ///
    /// A handler that demotes the current running task with time ticks recharged and selects the next task to run
    ///
    /// @tparam ConcreteScheduler Specify the type of the concrete scheduler
    /// @tparam CustomQuantumSpecifier A callable type that maps a priority level to a certain amount of time ticks
    ///
    template <typename ConcreteScheduler, typename CustomQuantumSpecifier>
    requires TaskConstraints::Schedulable<Traits::ScheduledTask<ConcreteScheduler>> &&
             TaskConstraints::Quantizable<Traits::ScheduledTask<ConcreteScheduler>> &&
             TaskConstraints::PrioritizableByAutoMutablePriority<Traits::ScheduledTask<ConcreteScheduler>> &&
             Concepts::QuantumSpecifier<CustomQuantumSpecifier, Traits::ScheduledTask<ConcreteScheduler>>
    struct RunNextWithDemotionAndQuantumRecharged
    {
        /// Type of the task managed by the scheduler
        using Task = Traits::ScheduledTask<ConcreteScheduler>;

        ///
        /// Notify the delegate that the current running task has used up its allocated ticks
        ///
        /// @param current The current running task whose allocated ticks is zero
        /// @return The task that is selected to run.
        /// @note This method does NOT support group operations.
        ///
        Task* onTaskQuantumUsedUp(Task* current)
        {
            auto self = static_cast<ConcreteScheduler*>(this);

            // Decrement the priority level of the current running task
            current->demote();

            // Reallocate the time ticks based on the new priority level of the current task
            current->allocateTicks(CustomQuantumSpecifier{}(current->getPriority()));

            // Enqueue the current task
            self->ready(current);

            // Select the next one to run
            return self->next();
        }
    };
}

#endif /* Scheduler_TaskQuantumUsedUpHandler_hpp */
