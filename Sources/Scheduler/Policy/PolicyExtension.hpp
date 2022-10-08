//
//  PolicyExtension.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-14.
//  Revised by FireWolf on 2021-9-24.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef PolicyExtension_hpp
#define PolicyExtension_hpp

#include <Scheduler/Constraint/Prioritizable.hpp>
#include <Scheduler/Constraint/Quantizable.hpp>
#include <Scheduler/Constraint/QuantumSpecifier.hpp>

/// The root namespace for the scheduler module where core components are defined
namespace Scheduler
{
    /// Defines a set of methods implemented by the delegate to receive notifications when a task will be enqueued or has been dequeued
    template <typename Task>
    struct PolicyDelegate
    {
    public:
        /// Virtual destructor
        virtual ~PolicyDelegate() = default;

        ///
        /// Notify the delegate that a task will be enqueued
        ///
        /// @param task The task passed to `ready()`
        /// @note This delegate function is called before `ready()`.
        ///
        virtual void taskWillEnqueue(Task* task) = 0;

        ///
        /// Notify the delegate that a task has been dequeued
        ///
        /// @param task The task returned by `next()`
        /// @note This delegate function is called after `next()`.
        ///
        virtual void taskHasDequeued(Task* task) = 0;
    };

    /// Defines the interface of a scheduling policy that supports code injection or event notification via delegate
    template <typename Task>
    struct PolicyWithDelegateSupport: public Policy<Task>
    {
    public:
        /// The super class
        using super = Policy<Task>;

        /// The delegate
        PolicyDelegate<Task>* delegate;

        /// Default constructor
        explicit PolicyWithDelegateSupport(PolicyDelegate<Task>* delegate)
        {
            this->delegate = delegate;
        }

        // MARK:- Scheduling Primitives

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next() override
        {
            Task* task = super::next();

            this->delegate->taskHasDequeued(task);

            return task;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task) override
        {
            this->delegate->taskWillEnqueue(task);

            super::ready(task);
        }
    };
}

/// Defines concepts related to scheduler components
namespace Scheduler::Concepts
{
    ///
    /// Defines the interface of installing code extensions before a task is enqueued or after a task has been dequeued
    ///
    /// @note i.e. Before the task is passed to `ready()` or after the task is returned by `next()`.
    ///
    template <typename Extension, typename Task>
    concept PolicyCodeExtension = requires(Task* task)
    {
        /// The extension can be initialized with zero arguments
        requires std::default_initializable<Extension>;

        /// The extension must implement the operator () that consumes a task to be enqueued or having been dequeued and returns void
        { Extension{}(task) } -> std::same_as<void>;
    };
}

/// The root namespace for the scheduler module where core components are defined
namespace Scheduler
{
    /// Defines the interface of a scheduling policy that supports installing code extensions for the `ready()` primitive
    template <typename BasePolicy, Concepts::PolicyCodeExtension<Traits::PolicyTask<BasePolicy>>... Extension>
    requires Concepts::Policy<BasePolicy>
    struct PolicyWithEnqueueExtensions: public BasePolicy
    {
    public:
        /// Type of the task managed by the policy component
        using Task = Traits::PolicyTask<BasePolicy>;

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task)
        {
            // Run each code extension in order
            ((Extension{}(task)), ...);

            // Enqueue the task
            BasePolicy::ready(task);
        }
    };

    /// Defines the interface of a scheduling policy that supports installing code extensions for the `next()` primitives
    template <typename BasePolicy, Concepts::PolicyCodeExtension<Traits::PolicyTask<BasePolicy>>... Extension>
    requires Concepts::Policy<BasePolicy>
    struct PolicyWithDequeueExtensions: public BasePolicy
    {
    public:
        /// Type of the task managed by the policy component
        using Task = Traits::PolicyTask<BasePolicy>;

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next()
        {
            // Dequeue the next task
            Task* task = BasePolicy::next();

            // Run each code extension in order
            ((Extension{}(task)), ...);

            // All done
            return task;
        }
    };
}

/// Defines some common code injectors
namespace Scheduler::Policies::Extensions
{
    ///
    /// A code injector that allocates quantum for the given task based on the task priority level
    ///
    template <typename Task, typename CustomQuantumSpecifier>
    requires TaskConstraints::Quantizable<Task> &&
             TaskConstraints::PrioritizableByPriority<Task> &&
             Concepts::QuantumSpecifier<CustomQuantumSpecifier, Task>
    struct PriorityBasedTaskQuantumAllocator
    {
        void operator()(Task* task)
        {
            task->allocateTicks(CustomQuantumSpecifier{}(task->getPriority()));
        }
    };
}

#endif /* PolicyExtension_hpp */
