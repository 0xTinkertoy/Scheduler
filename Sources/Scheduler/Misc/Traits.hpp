//
//  Traits.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-9-24.
//

#ifndef Traits_hpp
#define Traits_hpp

/// A namespace where all scheduler-related traits are defined
namespace Scheduler::Traits
{
    ///
    /// A helper trait class where types associated with a concrete scheduler type are defined
    ///
    /// @tparam Scheduler Specify the type of the concrete scheduler
    /// @note Developers must define a specialization for their concrete scheduler and the type of tasks to be scheduled
    /// @example Supposed that `MyRoundRobinScheduler` can schedule `SimpleTask`s.
    ///          ```
    ///          template <>
    ///          struct BasicTraits<MyRoundRobinScheduler>
    ///          {
    ///              using Task = SimpleTask;
    ///          }
    ///          ```
    ///          Developers define the member type `Task` to be `SimpleTask`.
    ///
    template <typename Scheduler>
    struct SchedulerTraits;

    ///
    /// Compute the type of tasks scheduled by the given scheduler
    ///
    /// @tparam Scheduler Specify the type of the concrete scheduler
    ///
    template <typename Scheduler>
    using ScheduledTask = typename SchedulerTraits<Scheduler>::Task;

    ///
    /// Compute the type of tasks managed by the given scheduling policy
    ///
    /// @tparam Policy Specify the type of the concrete scheduling policy component
    ///
    template <typename Policy>
    using PolicyTask = typename Policy::SchedulableTask;

    ///
    /// Compute the type of the task priority
    ///
    /// @tparam Task Specify the type of a prioritizable-by-priority task
    ///
    template <typename Task>
    using TaskPriority = typename Task::Priority;
}

#endif /* Traits_hpp */
