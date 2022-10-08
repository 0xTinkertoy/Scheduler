//
//  Scheduler.hpp
//  SchedulerTests
//
//  Created by FireWolf on 2021-9-24.
//

#ifndef Scheduler_Scheduler_hpp
#define Scheduler_Scheduler_hpp

//
// This is the umbrella header of the scheduler module
//

// MARK: - Task Control Block Constraints
#include <Scheduler/Constraint/Schedulable.hpp>
#include <Scheduler/Constraint/Prioritizable.hpp>
#include <Scheduler/Constraint/Quantizable.hpp>
#include <Scheduler/Constraint/QuantumSpecifier.hpp>

// MARK: - Scheduling Policy Components
#include <Scheduler/Policy/FIFO.hpp>
#include <Scheduler/Policy/Policy.hpp>
#include <Scheduler/Policy/PrioritizedSingleQueue.hpp>
#include <Scheduler/Policy/PrioritizedMultiQueue.hpp>
#include <Scheduler/Policy/PolicyMaker.hpp>
#include <Scheduler/Policy/PolicyExtension.hpp>

// MARK: - Schedule Event Handlers
#include <Scheduler/EventHandler/TaskCreationHandler.hpp>
#include <Scheduler/EventHandler/TaskTerminationHandler.hpp>
#include <Scheduler/EventHandler/TaskYieldingHandler.hpp>
#include <Scheduler/EventHandler/TaskBlockedHandler.hpp>
#include <Scheduler/EventHandler/TaskUnblockedHandler.hpp>
#include <Scheduler/EventHandler/TaskKilledHandler.hpp>
#include <Scheduler/EventHandler/TaskPriorityChangedHandler.hpp>
#include <Scheduler/EventHandler/TaskSelfPriorityChangedHandler.hpp>
#include <Scheduler/EventHandler/TaskQuantumUsedUpHandler.hpp>
#include <Scheduler/EventHandler/TimerInterruptHandler.hpp>

// MARK: - Helper Type Traits & Functions
#include <Scheduler/Misc/Traits.hpp>
#include <Scheduler/Misc/Utils.hpp>

// MARK: - Assemble Custom Schedulers

/// The root namespace for the scheduler module where core components are defined
namespace Scheduler
{
    ///
    /// A scheduler is composed of a scheduling policy and zero or more event handlers
    ///
    /// @tparam Policy Specify the scheduling policy component
    /// @tparam EventHandler Specify zero or more event handlers to interact with other entities
    /// @note Use this template to assemble a scheduler.
    ///
    template <typename Policy, typename... EventHandler>
    struct Assembler: public Policy, public EventHandler... {};

    ///
    /// A scheduler component that provides the idle task support component
    ///
    /// @tparam Task Specify the type of the task scheduled by the concrete scheduler
    /// @discussion Event handlers with idle task support rely on this component to fetch the idle task.
    ///             Concrete schedulers that provide support for the idle task must inherit from this class.
    ///
    template <typename Task>
    struct IdleTaskSupport
    {
        /// The idle task
        Task* idleTask;

        /// Initialize with the given idle task
        explicit IdleTaskSupport(Task* idleTask) : idleTask(idleTask) {}

        ///
        /// Get the idle task
        ///
        /// @return The non-null idle task.
        ///
        Task* getIdleTask()
        {
            return this->idleTask;
        }
    };
}

// MARK: - Examine Scheduler Components

///
/// Defines the interface of event handlers
/// (A set of concepts to check whether a scheduler provides certain event handlers)
///
/// @note Interfaces defined in this namespace are categorized into two groups:
///       one that supports group operations, termed as intermediate call;
///       one that does not, termed as terminating call.
///       Intermediate call does not return the task that should run next (a nullptr return value),
///       while terminating call guarantees to return a non-null task that is selected to run if it takes the idle task into consideration.
///       By default, all methods invoked with non-null parameters are terminating calls.
///       Some methods also provide an alternative way to invoke as a terminating call to ease the programming.
///       Details on how to invoke a method as an intermediate call can be found in the doc of each method.
/// @note All group operations should be finished by a terminating call to fetch the next task.
///       For example, if task A and task B are unblocked, while task C is running,
///       the sequence of calls should be either
///       1) onTaskUnblocked(nullptr, A) -> nullptr
///          onTaskUnblocked(nullptr, B) -> nullptr
///          onTaskUnblocked(C, nullptr) -> The next ready task
///       or
///       2) onTaskUnblocked(nullptr, A) -> nullptr
///          onTaskUnblocked(C, B) -> The next ready task
/// @note A group operation can also be composed of several different delegate method calls.
///       For example, if task A and task B are unblocked because task C has finished,
///       (e.g. A and B request to wait for C to finish)
///       the sequence of calls should be
///          onTaskUnblocked(nullptr, A) -> nullptr
///          onTaskUnblocked(nullptr, B) -> nullptr
///          onTaskFinished(C) -> The next ready task
///
namespace Scheduler
{
    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTimerInterruptHandler = requires(ConcreteScheduler& scheduler, Task* current)
    {
        ///
        /// Notify the delegate that a timer interrupt has occurred
        ///
        /// @param current The current running task
        /// @returns The non-null task that is selected to run.
        /// @note Signature: `Task* onTimerInterrupt(Task* current)`.
        ///
        { scheduler.onTimerInterrupt(current) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTaskCreationHandler = requires(ConcreteScheduler& scheduler, Task* current, Task* task)
    {
        ///
        /// Notify the delegate that a new task has been created
        ///
        /// @param current The current running task
        /// @param task The newly created task
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        /// @note Signature: `Task* onTaskCreated(Task* current, Task* task)`.
        ///
        { scheduler.onTaskCreated(current, task) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTaskTerminationHandler = requires(ConcreteScheduler& scheduler, Task* current)
    {
        ///
        /// Notify the delegate that the current running task has finished or terminated
        ///
        /// @param current The current running task that just finished
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        /// @note Signature: `Task* onTaskFinished(Task* current)`.
        ///
        { scheduler.onTaskFinished(current) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTaskYieldingHandler = requires(ConcreteScheduler& scheduler, Task* current)
    {
        ///
        /// Notify the delegate that the current running task has voluntarily relinquished the CPU
        ///
        /// @param current The current running task that just yielded
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        /// @note Signature: `Task* onTaskYielded(Task* current)`.
        ///
        { scheduler.onTaskYielded(current) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTaskBlockedHandler = requires(ConcreteScheduler& scheduler, Task* current)
    {
        ///
        /// Notify the delegate that the current running task has been blocked
        ///
        /// @param current The current running task that is blocked
        /// @returns The non-null task that is selected to run.
        /// @note This method does NOT support group operations.
        /// @note Signature: `Task* onTaskBlocked(Task* current)`.
        ///
        { scheduler.onTaskBlocked(current) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTaskUnblockedHandler = requires(ConcreteScheduler& scheduler, Task* current, Task* task)
    {
        ///
        /// Notify the delegate that a task has been unblocked
        ///
        /// @param current The current running task
        /// @param task The task that just got unblocked
        /// @returns The non-null task that is selected to run if requested.
        /// @note This method supports group operations.
        ///       1) Pass `nullptr` to `current` to enqueue `task` only.
        ///          In this case, this method returns `nullptr` back to the caller.
        ///       2) Pass `nullptr` to `task` to fetch the next task.
        ///       3) Pass a non-null task to both parameters to enqueue the unblocked task and fetch the next task.
        ///          In the above two cases, this method returns a non-null task that is ready to run,
        ///          indicating that group operations are completed. The caller should not have any subsequent calls.
        /// @note Signature: `Task* onTaskUnblocked(Task* current, Task* task)`.
        ///
        { scheduler.onTaskUnblocked(current, task) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTaskKilledHandler = requires(ConcreteScheduler& scheduler, Task* current, Task* task)
    {
        ///
        /// Notify the delegate that a task has been killed
        ///
        /// @param current The current running task
        /// @param task The task that just got killed
        /// @return The task that is selected to run if requested.
        /// @warning The task being killed must not be identical to the current running task.
        ///          If a task kills itself, the caller should use the task termination handler instead.
        /// @seealso `TaskTerminationHandler` to deal with the task who kills itself.
        /// @warning Since the scheduler is only responsible for ready tasks,
        ///          the task being killed must be ready and reside in the ready queue.
        /// @note This method supports group operations.
        ///       1) Pass `nullptr` to `current` to remove `task` from the ready queue only.
        ///          In this case, this method returns `nullptr` back to the caller.
        ///       2) Pass `nullptr` to `task` to fetch the next task.
        ///       3) Pass a non-null task to both parameters to remove the task being killed from the ready queue and fetch the next task.
        ///          In the above two cases, this method returns a task that is ready to run,
        ///          indicating that group operations are completed. The caller should not have any subsequent calls.
        /// @note Signature: `Task* onTaskKilled(Task* current, Task* task)`.
        ///
        { scheduler.onTaskKilled(current, task) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>, typename Priority = Traits::TaskPriority<Task>>
    concept ProvidesTaskPriorityChangedHandler = requires(ConcreteScheduler& scheduler, Task* current, Task* task, const Priority& oldPriority)
    {
        ///
        /// Notify the delegate that the priority level of a task has been changed
        ///
        /// @param current The current running task
        /// @param task The task whose priority level has been changed
        /// @param oldPriority The previous priority level
        /// @return The task that is selected to run.
        /// @note This method does NOT support group operations.
        /// @warning The task of which priority level has been changed must not be identical to the current running task.
        ///          If a task has requested to change its priority level, the caller should use the "self priority changed handler" instead.
        /// @warning Since the scheduler is only responsible for ready tasks,
        ///          the task of which priority level has been changed must be ready and reside in the ready queue.
        /// @seealso `TaskPrioritySelfChangedHandler` to deal with the task who has changed its priority level.
        /// @note Signature: `Task* onTaskPriorityChanged(Task* current, Task* task, const typename Task::Priority& oldPriority`.
        ///
        { scheduler.onTaskPriorityChanged(current, task, oldPriority) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTaskSelfPriorityChangedHandler = requires(ConcreteScheduler& scheduler, Task* current)
    {
        ///
        /// Notify the delegate that the priority level of a task has been changed
        ///
        /// @param current The current running task that has requested to change its priority level
        /// @return The task that is selected to run.
        /// @note This method does NOT support group operations.
        /// @note Signature: `Task* onTaskPriorityChanged(Task* current)`.
        ///
        { scheduler.onTaskPriorityChanged(current) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept ProvidesTaskQuantumUsedUpHandler = requires(ConcreteScheduler& scheduler, Task* current)
    {
        ///
        /// Notify the delegate that the current running task has used up its allocated ticks
        ///
        /// @param current The current running task whose allocated ticks is zero
        /// @return The task that is selected to run.
        /// @note This method does NOT support group operations.
        /// @note Signature: `Task* onTaskQuantumUsedUp(Task* current)`.
        ///
        { scheduler.onTaskQuantumUsedUp(current) } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept SupportsIdleTask = requires(ConcreteScheduler& scheduler)
    {
        ///
        /// Get the idle task
        ///
        /// @return The non-null idle task.
        /// @note Signature: `Task* getIdleTask()`.
        ///
        { scheduler.getIdleTask() } -> std::same_as<Task*>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>>
    concept SupportsRemovingTasks = requires(ConcreteScheduler& scheduler, Task* task)
    {
        ///
        /// Remove the given task from the ready queue
        ///
        /// @param task The task to be removed.
        /// @note Signature: `void remove(Task* task)`.
        { scheduler.remove(task) } -> std::same_as<void>;
    };

    template <typename ConcreteScheduler, typename Task = Traits::ScheduledTask<ConcreteScheduler>, typename Priority = Traits::TaskPriority<Task>>
    concept SupportsAdjustingTaskPriority = requires(ConcreteScheduler& scheduler, Task* task, const Priority& oldPriority)
    {
        ///
        /// Adjust the position of the given task in the ready queue
        ///
        /// @param task The task of which priority level has been changed
        /// @param oldPriority The previous priority level
        /// @note Signature: `void adjustPosition(Task* task, const typename Task::Priority& oldPriority)`.
        ///
        { scheduler.adjustPosition(task, oldPriority) } -> std::same_as<void>;
    };
}

#endif /* Scheduler_Scheduler_hpp */
