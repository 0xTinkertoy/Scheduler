//
//  Prioritizable.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-11.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Scheduler_Prioritizable_hpp
#define Scheduler_Prioritizable_hpp

#include <Comparable.hpp>

/// A namespace where task constraints related to the scheduler are defined
namespace TaskConstraints
{
    /// A type that can be prioritized by other entities by overloading operators >, >=, <, <=
    template <typename Task>
    concept ImplicitlyPrioritizable = Comparable<Task>;

    /// A type that can be explicitly prioritized by its defined priority level
    template <typename Task>
    concept PrioritizableByPriority = requires(const std::remove_reference_t<Task>& task)
    {
        ///
        /// The task must explicitly define its priority level type
        ///
        typename Task::Priority;

        ///
        /// The priority level must also be comparable
        ///
        /// @note The larger the priority level, the higher the task priority.
        ///
        requires Comparable<typename Task::Priority>;

        ///
        /// The task must be able to provide its immutable priority level
        ///
        /// @note Signature: `const typename Task::Priority& getPriority() const`.
        ///
        { task.getPriority() } -> std::same_as<const typename Task::Priority&>;
    };

    /// A type that can be explicitly prioritized by its mutable priority level
    template <typename Task>
    concept PrioritizableByMutablePriority = requires(std::remove_reference_t<Task>& task, const typename Task::Priority& priority)
    {
        ///
        /// The task must be prioritizable by priority
        ///
        requires PrioritizableByPriority<Task>;

        ///
        /// The task must be able to accept a new priority level
        ///
        /// @note Signature: `void setPriority(const typename Task::Priority& priority)`.
        ///
        { task.setPriority(priority) } -> std::same_as<void>;
    };

    /// A type that can be explicitly prioritized by its mutable priority level and promoted or demoted automatically
    template <typename Task>
    concept PrioritizableByAutoMutablePriority = requires(std::remove_reference_t<Task>& task)
    {
        ///
        /// The task must be prioritizable by its mutable priority
        ///
        requires PrioritizableByMutablePriority<Task>;

        ///
        /// The task can be promoted to the next priority level
        ///
        /// @note The priority level should remain unchanged if it is the highest one.
        ///
        { task.promote() } -> std::same_as<void>;

        ///
        /// The task can be demoted to the next priority level
        ///
        /// @note The priority level should remain unchanged if it is the lowest one.
        ///
        { task.demote() } -> std::same_as<void>;
    };

    /// A type that can be prioritized by other entity
    template <typename Task>
    concept AnyPrioritizable = ImplicitlyPrioritizable<Task> ||
                               PrioritizableByPriority<Task> ||
                               PrioritizableByMutablePriority<Task> ||
                               PrioritizableByAutoMutablePriority<Task>;
}

/// A comparable wrapper for any prioritizable task type
template <typename Task>
requires TaskConstraints::AnyPrioritizable<Task>
struct AnyPrioritizableTask
{
    /// The wrapped task pointer
    const Task& task;

    /// Wrap a prioritizable task
    explicit AnyPrioritizableTask(const Task& task) : task(task) {}

    /// Convenient comparator
    struct BridgedLessComparator
    {
        bool operator()(Task* const& lhs, Task* const& rhs)
        {
            return AnyPrioritizableTask(*lhs) < AnyPrioritizableTask(*rhs);
        }

        bool operator()(const Task& lhs, const Task& rhs)
        {
            return AnyPrioritizableTask(lhs) < AnyPrioritizableTask(rhs);
        }
    };

    /// Convenient comparator
    struct BridgedGreaterComparator
    {
        bool operator()(Task* const& lhs, Task* const& rhs)
        {
            return AnyPrioritizableTask(*lhs) > AnyPrioritizableTask(*rhs);
        }

        bool operator()(const Task& lhs, const Task& rhs)
        {
            return AnyPrioritizableTask(lhs) > AnyPrioritizableTask(rhs);
        }
    };

    /// Specialized for implicitly prioritizable tasks
    friend bool operator<(const AnyPrioritizableTask& lhs, const AnyPrioritizableTask& rhs) requires TaskConstraints::ImplicitlyPrioritizable<Task>
    {
        return lhs.task < rhs.task;
    }

    /// Specialized for implicitly prioritizable tasks
    friend bool operator>(const AnyPrioritizableTask& lhs, const AnyPrioritizableTask& rhs) requires TaskConstraints::ImplicitlyPrioritizable<Task>
    {
        return lhs.task > rhs.task;
    }

    /// Specialized for implicitly prioritizable tasks
    friend bool operator<=(const AnyPrioritizableTask& lhs, const AnyPrioritizableTask& rhs) requires TaskConstraints::ImplicitlyPrioritizable<Task>
    {
        return lhs.task <= rhs.task;
    }

    /// Specialized for implicitly prioritizable tasks
    friend bool operator>=(const AnyPrioritizableTask& lhs, const AnyPrioritizableTask& rhs) requires TaskConstraints::ImplicitlyPrioritizable<Task>
    {
        return lhs.task >= rhs.task;
    }

    /// Specialized for tasks that are explicitly prioritizable by their priority
    friend bool operator<(const AnyPrioritizableTask& lhs, const AnyPrioritizableTask& rhs) requires TaskConstraints::PrioritizableByPriority<Task>
    {
        return lhs.task.getPriority() < rhs.task.getPriority();
    }

    /// Specialized for tasks that are explicitly prioritizable by their priority
    friend bool operator>(const AnyPrioritizableTask& lhs, const AnyPrioritizableTask& rhs) requires TaskConstraints::PrioritizableByPriority<Task>
    {
        return lhs.task.getPriority() > rhs.task.getPriority();
    }

    /// Specialized for tasks that are explicitly prioritizable by their priority
    friend bool operator<=(const AnyPrioritizableTask& lhs, const AnyPrioritizableTask& rhs) requires TaskConstraints::PrioritizableByPriority<Task>
    {
        return lhs.task.getPriority() <= rhs.task.getPriority();
    }

    /// Specialized for tasks that are explicitly prioritizable by their priority
    friend bool operator>=(const AnyPrioritizableTask& lhs, const AnyPrioritizableTask& rhs) requires TaskConstraints::PrioritizableByPriority<Task>
    {
        return lhs.task.getPriority() >= rhs.task.getPriority();
    }
};

#endif /* Scheduler_Prioritizable_hpp */
