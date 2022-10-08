//
//  Policy.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-13.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Policy_hpp
#define Policy_hpp

#include <Scheduler/Constraint/Schedulable.hpp>

/// The root namespace for the scheduler module where core components are defined
namespace Scheduler
{
    /// Defines a set of methods to be implemented by a concrete scheduling policy
    template <typename Task>
    requires TaskConstraints::Schedulable<Task>
    struct Policy
    {
    public:
        /// Virtual destructor
        virtual ~Policy() = default;

        // MARK:- Scheduling Primitives

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        virtual Task* next() = 0;

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        virtual void ready(Task* task) = 0;
    };
}

/// Defines concepts related to scheduler components
namespace Scheduler::Concepts
{
    /// A non-virtual scheduling policy component must provide the scheduling primitives
    template <typename P>
    concept Policy = requires(P& policy, typename P::SchedulableTask* task)
    {
        /// Must define the associated schedulable task type
        typename P::SchedulableTask;

        /// Must provide the dequeue primitive
        { policy.next() } -> std::same_as<typename P::SchedulableTask*>;

        /// Must provide the enqueue primitive
        { policy.ready(task) } -> std::same_as<void>;
    };
}

#endif /* Policy_hpp */
