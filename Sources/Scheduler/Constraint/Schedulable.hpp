//
//  Schedulable.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-9.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef Schedulable_hpp
#define Schedulable_hpp

#include <concepts>

/// The root namespace for the scheduler module where core components are defined
namespace Scheduler
{
    ///
    /// Mark a type that can be scheduled by an scheduler
    ///
    /// @note Classes inherited from `Schedulable` are schedulable.
    ///
    struct Schedulable {};
}

/// A namespace where task constraints related to the scheduler are defined
namespace TaskConstraints
{
    /// A concept to check whether a task is schedulable
    template <typename T>
    concept Schedulable = std::derived_from<T, Scheduler::Schedulable>;
}

#endif /* Schedulable_hpp */
