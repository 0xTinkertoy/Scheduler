//
//  PolicyMaker.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-11.
//  Revised by FireWolf on 2021-9-24.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef PolicyMaker_hpp
#define PolicyMaker_hpp

#include <Scheduler/Constraint/Prioritizable.hpp>
#include <Scheduler/Policy/FIFO.hpp>
#include <Scheduler/Misc/Traits.hpp>
#include <concepts>

/// Defines concepts related to scheduler components
namespace Scheduler::Concepts
{
    /// A callable type that maps a priority level to an opaque scheduling policy
    template <typename Maker, typename Task>
    concept PolicyMaker = requires(const typename Task::Priority& priority, Scheduler::Policy<Task>* policy)
    {
        ///
        /// The policy maker must implement the static member function `create()` that consumes a priority level and returns an opaque scheduling policy
        ///
        /// @note Signature: `static Scheduler::Policy<Task>* create(const typename Task::Priority& priority)`.
        ///
        { Maker::create(priority) } -> std::same_as<Scheduler::Policy<Task>*>;

        ///
        /// The policy maker must implement the static member function `destroy()` so that each policy instance returned by `create()` is properly released
        ///
        /// @note Signature: `static void destroy(Scheduler::Policy<Task>* policy)`.
        ///
        { Maker::destroy(policy) } -> std::same_as<void>;
    };
}

/// Defines some common policy makers as an example
namespace Scheduler::PolicyMakers
{
    ///
    /// A policy maker that maps each priority level to a dynamically allocated FIFO scheduling policy
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    ///
    template <typename Task>
    requires TaskConstraints::PrioritizableByPriority<Task>
    struct DynamicFIFO
    {
        /// Type of the task priority
        using Priority = Scheduler::Traits::TaskPriority<Task>;

        ///
        /// Maps the given priority level to a specific scheduling policies
        ///
        /// @param priority The task priority
        /// @return A non-null scheduling policy.
        ///
        static Scheduler::Policy<Task>* create([[maybe_unused]] const Priority& priority)
        {
            return new Scheduler::Policies::FIFO::Virtual::LinkedListImp<Task>();
        }

        ///
        /// Releases the given policy properly
        ///
        /// @param policy A non-null scheduling policy instance returned by `create()`
        ///
        static void destroy(Scheduler::Policy<Task>* policy)
        {
            delete policy;
        }
    };
}

#endif /* PolicyMaker_hpp */
