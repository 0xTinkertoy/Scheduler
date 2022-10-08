//
//  PrioritizedMultiQueue.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-11.
//  Revised by FireWolf on 2021-9-24.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef PrioritizedMultiQueue_hpp
#define PrioritizedMultiQueue_hpp

#include <Scheduler/Policy/Policy.hpp>
#include <Scheduler/Policy/PolicyMaker.hpp>
#include <Hashable.hpp>
#include <Debug.hpp>
#include <array>
#include <map>

///
/// Defines scheduling policies that prioritizes schedulable tasks in multiple queues
///
/// @note All structs do not define any virtual functions thus are suitable for schedulers that have a fixed policy.
///
namespace Scheduler::Policies::PrioritizedMultiQueue::Normal
{
    ///
    /// Implements the policy using a static array to map each priority level to a queue
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    /// @tparam PolicyMaker A callable type that maps a priority level to a scheduling policy
    /// @tparam MaxPriorityLevel Defines the value of the largest priority level
    /// @note The length of the array map will be `max + 1`, so it is recommended to define a contiguous sequence of priority levels.
    /// @note The priority level must be convertible to an unsigned integer.
    /// @note The policy maker is called once for each priority level,
    ///       and the returned policy instance should be unique for each level.
    ///       The memory of returned policy instance is managed by this class.
    ///       The policy maker must provide a deleter to avoid any memory leaks.
    /// @seealso `SchedulingPolicyMakers` to see predefined policy makers.
    ///
    template <typename Task, typename PolicyMaker, size_t MaxPriorityLevel>
    requires TaskConstraints::PrioritizableByPriority<Task> &&
             Concepts::PolicyMaker<PolicyMaker, Task> &&
             std::unsigned_integral<Traits::TaskPriority<Task>>
    struct ArrayMapImp
    {
    private:
        /// The priority level type
        using Priority = Traits::TaskPriority<Task>;

        /// A private map that maps priority levels to their scheduling policies
        /// Non-existent priority levels are mapped to null pointers
        std::array<Scheduler::Policy<Task>*, MaxPriorityLevel + 1> queues = {};

    public:
        /// Define the schedulable task type
        using SchedulableTask = Task;

        /// Default Destructor
        ~ArrayMapImp()
        {
            for (auto iterator = this->queues.begin(); iterator != this->queues.end(); iterator++)
            {
                if (*iterator != nullptr)
                {
                    PolicyMaker::destroy(*iterator);
                }
            }
        }

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next()
        {
            // From the highest priority level to the lowest
            for (auto iterator = this->queues.rbegin(); iterator != this->queues.rend(); iterator++)
            {
                // Guard: Ensure that the current priority level exists
                if (*iterator == nullptr)
                {
                    continue;
                }

                // Guard: Check whether a higher priority task is pending
                Task* next = (*iterator)->next();

                if (next != nullptr)
                {
                    return next;
                }
            }

            // No pending task
            return nullptr;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task)
        {
            const Priority& priority = task->getPriority();

            // Guard: Check whether a scheduler is already available for the priority of the given task
            if (this->queues[priority] == nullptr)
            {
                this->queues[priority] = PolicyMaker::create(priority);
            }

            // Guard: Scheduler should now be available
            passert(this->queues[priority] != nullptr, "Scheduler for priority level should be non-null.");

            this->queues[priority]->ready(task);
        }
    };

    ///
    /// Implements the policy using an array to map each priority level to a queue
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    /// @tparam PolicyMaker A callable type that maps a priority level to a scheduling policy
    /// @note The priority level must be hashable.
    /// @note The policy maker is called once for each priority level,
    ///       and the returned policy instance should be unique for each level.
    ///       The memory of returned policy instance is managed by this class.
    ///       The policy maker must provide a deleter to avoid any memory leaks.
    /// @seealso `SchedulingPolicyMakers` to see predefined policy makers.
    ///
    template <typename Task, typename PolicyMaker>
    requires TaskConstraints::PrioritizableByPriority<Task> &&
             Concepts::PolicyMaker<PolicyMaker, Task> &&
             Hashable<Traits::TaskPriority<Task>>
    struct StlMapImp
    {
    private:
        /// The priority level type
        using Priority = Traits::TaskPriority<Task>;

        /// A private map that maps priority levels to their scheduling policies
        std::map<Priority, Scheduler::Policy<Task>*, std::greater<Priority>> queues;

    public:
        /// Define the schedulable task type
        using SchedulableTask = Task;

        /// Default Destructor
        ~StlMapImp()
        {
            for (auto iterator = this->queues.begin(); iterator != this->queues.end(); iterator++)
            {
                typename PolicyMaker::destroy(iterator->second);
            }
        }

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next()
        {
            for (auto iterator = this->queues.begin(); iterator != this->queues.end(); iterator++)
            {
                Task* next = iterator->second->next();

                // Guard: A higher priority task is pending
                if (next != nullptr)
                {
                    return next;
                }
            }

            // No pending task
            return nullptr;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task)
        {
            const Priority& priority = task->getPriority();

            // Guard: Check whether a scheduler is already available for the priority of the given task
            if (this->queues[priority] == nullptr)
            {
                this->queues[priority] = PolicyMaker::create(priority);
            }

            // Guard: Scheduler should now be available
            passert(this->queues[priority] != nullptr, "Scheduler for priority level should be non-null.");

            this->queues[priority]->ready(task);
        }
    };

    ///
    /// Implements the policy using a static array to map each priority level to a queue of the same type
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    /// @tparam Policy Specify the type of scheduling policies to which the scheduler maps each priority level
    /// @tparam MaxPriorityLevel Defines the value of the largest priority level
    /// @note The length of the array map will be `max + 1`, so it is recommended to define a contiguous sequence of priority levels.
    /// @note The priority level must be convertible to an unsigned integer.
    ///
    template <typename Task, typename Policy, size_t MaxPriorityLevel>
    requires TaskConstraints::PrioritizableByPriority<Task> && std::unsigned_integral<Traits::TaskPriority<Task>>
    struct ArrayMapHomoImp
    {
    private:
        /// The priority level type
        using Priority = Traits::TaskPriority<Task>;

        /// A private map that maps priority levels to their scheduling policies
        /// Non-existent priority levels are mapped to null pointers
        std::array<Policy, MaxPriorityLevel + 1> queues = {};

    public:
        /// Define the schedulable task type
        using SchedulableTask = Task;

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next()
        {
            // From the highest priority level to the lowest
            for (auto iterator = this->queues.rbegin(); iterator != this->queues.rend(); iterator++)
            {
                // Guard: Check whether a higher priority task is pending
                Task* next = (*iterator).next();

                if (next != nullptr)
                {
                    return next;
                }
            }

            // No pending task
            return nullptr;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task)
        {
            this->queues[task->getPriority()].ready(task);
        }
    };

    ///
    /// Implements the policy using an array to map each priority level to a queue of the same type
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    /// @tparam Policy Specify the type of scheduling policies to which the scheduler maps each priority level
    /// @note The priority level must be hashable.
    ///
    template <typename Task, typename Policy>
    requires TaskConstraints::PrioritizableByPriority<Task> && Hashable<Traits::TaskPriority<Task>>
    struct StlMapHomoImp
    {
    private:
        /// The priority level type
        using Priority = Traits::TaskPriority<Task>;

        /// A private map that maps priority levels to their scheduling policies
        std::map<Priority, Policy, std::greater<Priority>> queues;

    public:
        /// Define the schedulable task type
        using SchedulableTask = Task;

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next()
        {
            for (auto iterator = this->queues.begin(); iterator != this->queues.end(); iterator++)
            {
                Task* next = iterator->second.next();

                // Guard: A higher priority task is pending
                if (next != nullptr)
                {
                    return next;
                }
            }

            // No pending task
            return nullptr;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task)
        {
            this->queues[task->getPriority()].ready(task);
        }
    };
}

///
/// Defines scheduling policies that prioritizes schedulable tasks in multiple queues
///
/// @note All structs implement the interface `SchedulingPolicy` thus their instances can be treated as opaque policies.
///
namespace Scheduler::Policies::PrioritizedMultiQueue::Virtual
{
    ///
    /// Implements the policy using a static array to map each priority level to a queue
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    /// @tparam PolicyMaker A callable type that maps a priority level to a scheduling policy
    /// @tparam MaxPriorityLevel Defines the value of the largest priority level
    /// @note The length of the array map will be `max + 1`, so it is recommended to define a contiguous sequence of priority levels.
    /// @note The priority level must be convertible to an unsigned integer.
    /// @note The policy maker is called once for each priority level,
    ///       and the returned policy instance should be unique for each level.
    ///       The memory of returned policy instance is managed by this class.
    ///       The policy maker must provide a deleter to avoid any memory leaks.
    /// @seealso `SchedulingPolicyMakers` to see predefined policy makers.
    ///
    template <typename Task, typename PolicyMaker, size_t MaxPriorityLevel>
    requires TaskConstraints::PrioritizableByPriority<Task> &&
             Concepts::PolicyMaker<PolicyMaker, Task> &&
             std::unsigned_integral<Traits::TaskPriority<Task>>
    struct ArrayMapImp: public Scheduler::Policy<Task>
    {
    private:
        /// The priority level type
        using Priority = Traits::TaskPriority<Task>;

        /// A private map that maps priority levels to their scheduling policies
        /// Non-existent priority levels are mapped to null pointers
        std::array<Scheduler::Policy<Task>*, MaxPriorityLevel + 1> queues = {};

    public:
        /// Define the schedulable task type
        using SchedulableTask = Task;

        /// Default Destructor
        ~ArrayMapImp()
        {
            for (auto iterator = this->queues.begin(); iterator != this->queues.end(); iterator++)
            {
                if (*iterator != nullptr)
                {
                    PolicyMaker::destroy(*iterator);
                }
            }
        }

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next() override
        {
            // From the highest priority level to the lowest
            for (auto iterator = this->queues.rbegin(); iterator != this->queues.rend(); iterator++)
            {
                // Guard: Ensure that the current priority level exists
                if (*iterator == nullptr)
                {
                    continue;
                }

                // Guard: Check whether a higher priority task is pending
                Task* next = (*iterator)->next();

                if (next != nullptr)
                {
                    return next;
                }
            }

            // No pending task
            return nullptr;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task) override
        {
            const Priority& priority = task->getPriority();

            // Guard: Check whether a scheduler is already available for the priority of the given task
            if (this->queues[priority] == nullptr)
            {
                this->queues[priority] = PolicyMaker::create(priority);
            }

            // Guard: Scheduler should now be available
            passert(this->queues[priority] != nullptr, "Scheduler for priority level should be non-null.");

            this->queues[priority]->ready(task);
        }
    };

    ///
    /// Implements the policy using an array to map each priority level to a queue
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    /// @tparam PolicyMaker A callable type that maps a priority level to a scheduling policy
    /// @note The priority level must be hashable.
    /// @note The policy maker is called once for each priority level,
    ///       and the returned policy instance should be unique for each level.
    ///       The memory of returned policy instance is managed by this class.
    ///       The policy maker must provide a deleter to avoid any memory leaks.
    /// @seealso `SchedulingPolicyMakers` to see predefined policy makers.
    ///
    template <typename Task, typename PolicyMaker>
    requires TaskConstraints::PrioritizableByPriority<Task> &&
             Concepts::PolicyMaker<PolicyMaker, Task> &&
             Hashable<Traits::TaskPriority<Task>>
    struct StlMapImp: public Scheduler::Policy<Task>
    {
    private:
        /// The priority level type
        using Priority = Traits::TaskPriority<Task>;

        /// A private map that maps priority levels to their scheduling policies
        std::map<Priority, Scheduler::Policy<Task>*, std::greater<Priority>> queues;

    public:
        /// Define the schedulable task type
        using SchedulableTask = Task;

        /// Default Destructor
        ~StlMapImp()
        {
            for (auto iterator = this->queues.begin(); iterator != this->queues.end(); iterator++)
            {
                PolicyMaker::destroy(iterator->second);
            }
        }

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next() override
        {
            for (auto iterator = this->queues.begin(); iterator != this->queues.end(); iterator++)
            {
                Task* next = iterator->second->next();

                // Guard: A higher priority task is pending
                if (next != nullptr)
                {
                    return next;
                }
            }

            // No pending task
            return nullptr;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task) override
        {
            const Priority& priority = task->getPriority();

            // Guard: Check whether a scheduler is already available for the priority of the given task
            if (this->queues[priority] == nullptr)
            {
                this->queues[priority] = PolicyMaker::create(priority);
            }

            // Guard: Scheduler should now be available
            passert(this->queues[priority] != nullptr, "Scheduler for priority level should be non-null.");

            this->queues[priority]->ready(task);
        }
    };

    ///
    /// Implements the policy using a static array to map each priority level to a queue of the same type
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    /// @tparam Policy Specify the type of scheduling policies to which the scheduler maps each priority level
    /// @tparam MaxPriorityLevel Defines the value of the largest priority level
    /// @note The length of the array map will be `max + 1`, so it is recommended to define a contiguous sequence of priority levels.
    /// @note The priority level must be convertible to an unsigned integer.
    /// @seealso `SchedulingPolicyMakers` to see predefined policy makers.
    ///
    template <typename Task, typename Policy, size_t MaxPriorityLevel>
    requires TaskConstraints::PrioritizableByPriority<Task> && std::unsigned_integral<Traits::TaskPriority<Task>>
    struct ArrayMapHomoImp: public Scheduler::Policy<Task>
    {
    private:
        /// The priority level type
        using Priority = Traits::TaskPriority<Task>;

        /// A private map that maps priority levels to their scheduling policies
        /// Non-existent priority levels are mapped to null pointers
        std::array<Policy, MaxPriorityLevel + 1> queues = {};

    public:
        /// Define the schedulable task type
        using SchedulableTask = Task;

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next() override
        {
            // From the highest priority level to the lowest
            for (auto iterator = this->queues.rbegin(); iterator != this->queues.rend(); iterator++)
            {
                // Guard: Check whether a higher priority task is pending
                Task* next = (*iterator).next();

                if (next != nullptr)
                {
                    return next;
                }
            }

            // No pending task
            return nullptr;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task) override
        {
            this->queues[task->getPriority()].ready(task);
        }
    };

    ///
    /// Implements the policy using an array to map each priority level to a queue of the same type
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    /// @tparam Policy Specify the type of scheduling policies to which the scheduler maps each priority level
    /// @note The priority level must be hashable.
    ///
    template <typename Task, typename Policy>
    requires TaskConstraints::PrioritizableByPriority<Task> && Hashable<Traits::TaskPriority<Task>>
    struct StlMapHomoImp: public Scheduler::Policy<Task>
    {
    private:
        /// The priority level type
        using Priority = Traits::TaskPriority<Task>;

        /// A private map that maps priority levels to their scheduling policies
        std::map<Priority, Policy, std::greater<Priority>> queues;

    public:
        /// Define the schedulable task type
        using SchedulableTask = Task;

        ///
        /// Dequeue the next ready schedulable task
        ///
        /// @returns A task that is ready to run, `NULL` if no task is ready.
        ///
        Task* next() override
        {
            for (auto iterator = this->queues.begin(); iterator != this->queues.end(); iterator++)
            {
                Task* next = iterator->second.next();

                // Guard: A higher priority task is pending
                if (next != nullptr)
                {
                    return next;
                }
            }

            // No pending task
            return nullptr;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        ///
        void ready(Task* task) override
        {
            this->queues[task->getPriority()].ready(task);
        }
    };
}

#endif /* PrioritizedMultiQueue_hpp */
