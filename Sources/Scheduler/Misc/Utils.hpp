//
//  Utils.hpp
//  SchedulerTests
//
//  Created by FireWolf on 2021-9-24.
//

#ifndef Scheduler_Utils_hpp
#define Scheduler_Utils_hpp

#include <Scheduler/Constraint/Prioritizable.hpp>
#include <utility>

/// Defines some useful helper functions that may be needed by a handler
namespace Scheduler::Utilities
{
    ///
    /// [SPEC] Order two task based on their priority
    ///
    /// @tparam Task Specify the type of prioritizable tasks
    /// @param task1 A prioritizable task
    /// @param task2 A prioritizable task
    /// @return A pair of tasks, where the first task has the highest priority.
    /// @note If both tasks have the same priority, `task1` will be placed at the first slot.
    /// @note Specialized for implicitly prioritizable tasks.
    ///
    template <typename Task>
    requires TaskConstraints::ImplicitlyPrioritizable<Task>
    std::pair<Task*, Task*> orderByPriority(Task* task1, Task* task2)
    {
        if (*task1 >= *task2)
        {
            return std::make_pair(task1, task2);
        }
        else
        {
            return std::make_pair(task2, task1);
        }
    }

    ///
    /// [SPEC] Order two task based on their priority
    ///
    /// @tparam Task Specify the type of prioritizable tasks
    /// @param task1 A prioritizable task
    /// @param task2 A prioritizable task
    /// @return A pair of tasks, where the first task has the highest priority.
    /// @note If both tasks have the same priority, `task1` will be placed at the first slot.
    /// @note Specialized for tasks that are explicitly prioritizable by their priority level.
    ///
    template <typename Task>
    requires TaskConstraints::PrioritizableByPriority<Task>
    std::pair<Task*, Task*> orderByPriority(Task* task1, Task* task2)
    {
        if (task1->getPriority() >= task2->getPriority())
        {
            return std::make_pair(task1, task2);
        }
        else
        {
            return std::make_pair(task2, task1);
        }
    }
}

#endif /* Scheduler_Utils_hpp */
