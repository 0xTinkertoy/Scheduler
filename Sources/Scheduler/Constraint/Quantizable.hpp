//
//  Quantizable.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-12.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Quantizable_hpp
#define Quantizable_hpp

#include <concepts>

/// A namespace where task constraints related to the scheduler are defined
namespace TaskConstraints
{
    /// A type that has time ticks allocated
    template <typename Task>
    concept Quantizable = requires(Task& task, typename Task::Tick ticks)
    {
        /// The task must explicitly define its tick type
        typename Task::Tick;

        /// The tick type must be convertible to an unsigned integer
        std::unsigned_integral<typename Task::Tick>;

        /// The task should adjust its remaining ticks properly on a timer tick
        { task.tick() } -> std::same_as<void>;

        /// The task should report whether it has used up its time allotment
        { task.hasUsedUpTimeAllotment() } -> std::same_as<bool>;

        /// Other entity should be able to allocate a certain number of ticks to the task
        { task.allocateTicks(ticks) } -> std::same_as<void>;
    };
}

#endif /* Quantizable_hpp */
