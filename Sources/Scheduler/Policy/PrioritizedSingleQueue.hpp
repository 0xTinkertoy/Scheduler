//
//  PrioritizedSingleQueue.hpp
//  Scheduler
//
//  Created by FireWolf on 2021-1-11.
//  Revised by FireWolf on 2021-9-24.
//  Copyright © 2021 FireWolf. All rights reserved.
//

#ifndef PrioritizedSingleQueue_hpp
#define PrioritizedSingleQueue_hpp

#include <Scheduler/Policy/Policy.hpp>
#include <Scheduler/Constraint/Prioritizable.hpp>
#include <LinkedList.hpp>
#include <queue>

///
/// Defines scheduling policies that prioritizes schedulable tasks in a single queue
///
/// @note All structs do not define any virtual functions thus are suitable for schedulers that have a fixed policy.
///
namespace Scheduler::Policies::PrioritizedSingleQueue::Normal
{
    ///
    /// Implements the policy by maintaining a linked list of schedulable tasks without allocating memory dynamically
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    ///
    template <typename Task>
    requires ListableItem<Task> && TaskConstraints::AnyPrioritizable<Task>
    struct LinkedListImp
    {
    private:
        /// An internal FIFO queue
        LinkedList<Task> queue;

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
            // Guard: Check whether the queue is empty
            return this->queue.isEmpty() ? nullptr : this->queue.dequeue();
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        /// @warning The given task is inserted into the queue regardless of whether it is the idle task or not.
        ///
        void ready(Task* task)
        {
            this->queue.template insert(task, typename AnyPrioritizableTask<Task>::BridgedGreaterComparator{});
        }
    };

    ///
    /// Implements the policy by maintaining a STL priority queue of schedulable tasks
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    ///
    template <typename Task>
    requires TaskConstraints::AnyPrioritizable<Task>
    struct StlPriorityQueueImp
    {
    private:
        /// An internal priority queue
        std::priority_queue<Task*, std::vector<Task*>, typename AnyPrioritizableTask<Task>::BridgedLessComparator> queue;

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
            // Guard: Check whether the queue is empty
            if (this->queue.empty())
            {
                return nullptr;
            }

            Task* task = this->queue.top();

            this->queue.pop();

            return task;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        /// @warning The given task is inserted into the queue regardless of whether it is the idle task or not.
        ///
        void ready(Task* task)
        {
            this->queue.push(task);
        }
    };
}

///
/// Defines scheduling policies that prioritizes schedulable tasks in a single queue
///
/// @note All structs implement the interface `SchedulingPolicy` thus their instances can be treated as opaque policies.
///
namespace Scheduler::Policies::PrioritizedSingleQueue::Virtual
{
    ///
    /// Implements the policy by maintaining a linked list of schedulable tasks without allocating memory dynamically
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    ///
    template <typename Task>
    requires ListableItem<Task> && TaskConstraints::AnyPrioritizable<Task>
    struct LinkedListImp: public Scheduler::Policy<Task>
    {
    private:
        /// An internal FIFO queue
        LinkedList<Task> queue;

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
            // Guard: Check whether the queue is empty
            return this->queue.isEmpty() ? nullptr : this->queue.removeFirst();
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        /// @warning The given task is inserted into the queue regardless of whether it is the idle task or not.
        ///
        void ready(Task* task) override
        {
            this->queue.template insert(task, typename AnyPrioritizableTask<Task>::BridgedGreaterComparator{});
        }
    };

    ///
    /// Implements the policy by maintaining a STL priority queue of schedulable tasks
    ///
    /// @tparam Task Specify the type of schedulable tasks managed by the scheduler
    ///
    template <typename Task>
    requires TaskConstraints::AnyPrioritizable<Task>
    struct StlPriorityQueueImp: public Scheduler::Policy<Task>
    {
    private:
        /// An internal priority queue
        std::priority_queue<Task*, std::vector<Task*>, typename AnyPrioritizableTask<Task>::BridgedLessComparator> queue;

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
            // Guard: Check whether the queue is empty
            if (this->queue.empty())
            {
                return nullptr;
            }

            Task* task = this->queue.top();

            this->queue.pop();

            return task;
        }

        ///
        /// Enqueue a ready schedulable task
        ///
        /// @param task A non-null task that is ready to run
        /// @warning The given task is inserted into the queue regardless of whether it is the idle task or not.
        ///
        void ready(Task* task) override
        {
            this->queue.push(task);
        }
    };
}

#endif /* PrioritizedSingleQueue_hpp */
