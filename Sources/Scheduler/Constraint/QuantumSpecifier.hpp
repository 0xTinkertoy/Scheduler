//
//  QuantumSpecifier.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-13.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef Scheduler_QuantumSpecifier_hpp
#define Scheduler_QuantumSpecifier_hpp

#include <concepts>

/// Defines concepts related to scheduler components
namespace Scheduler::Concepts
{
    /// A callable type that maps a task priority level to a certain amount of time ticks
    template <typename Specifier, typename Task>
    concept QuantumSpecifier = requires(const typename Task::Priority& priority)
    {
        ///
        /// The ticks specifier can be initialized with zero arguments
        ///
        std::default_initializable<Specifier>;

        ///
        /// The ticks specifier must implement the operator () to consume a priority level and return the amount of ticks
        ///
        /// @note Signature: `typename Task::Quantum operator()(const typename Task::Priority& priority)`.
        ///
        { Specifier{}(priority) } -> std::same_as<typename Task::Tick>;
    };
}

#endif /* Scheduler_QuantumSpecifier_hpp */
