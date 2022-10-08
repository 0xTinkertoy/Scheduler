//
//  SampleSchedulers.hpp
//  SchedulerTests
//
//  Created by FireWolf on 2021-9-24.
//

#ifndef SampleSchedulers_hpp
#define SampleSchedulers_hpp

#include <Scheduler/Scheduler.hpp>

/// Defines some common schedulers as samples
namespace SampleSchedulers
{
    using namespace Scheduler;

    ///
    /// A simple scheduler that manages tasks on a first-come, first-served basis
    ///
    template<typename Task>
    class FIFO : public Assembler<
            Policies::FIFO::Normal::LinkedListImp<Task>,
            EventHandlers::TaskCreation::Cooperative::KeepRunningCurrentWithIdleTaskSupport<FIFO<Task>>,
            EventHandlers::TaskTermination::Common::RunNextWithIdleTaskSupport<FIFO<Task>>,
            EventHandlers::TaskBlocked::Common::RunNextWithIdleTaskSupport<FIFO<Task>>,
            EventHandlers::TaskUnblocked::Cooperative::KeepRunningCurrentWithIdleTaskSupport<FIFO<Task>>,
            EventHandlers::TaskYielding::Common::RunNext<FIFO<Task>>,
            EventHandlers::TimerInterrupt::Cooperative::KeepRunningCurrent<FIFO<Task>>>,
            public IdleTaskSupport<Task>
    {
        using IdleTaskSupport<Task>::IdleTaskSupport;
    };

    ///
    /// A simple preemptive scheduler that manages tasks in a round-robin fashion
    ///
    template<typename Task>
    class RoundRobin : public Assembler<
            Policies::FIFO::Normal::LinkedListImp<Task>,
            EventHandlers::TaskCreation::Cooperative::KeepRunningCurrentWithIdleTaskSupport<RoundRobin<Task>>,
            EventHandlers::TaskTermination::Common::RunNextWithIdleTaskSupport<RoundRobin<Task>>,
            EventHandlers::TaskBlocked::Common::RunNextWithIdleTaskSupport<RoundRobin<Task>>,
            EventHandlers::TaskUnblocked::Cooperative::KeepRunningCurrentWithIdleTaskSupport<RoundRobin<Task>>,
            EventHandlers::TaskYielding::Common::RunNext<RoundRobin<Task>>,
            EventHandlers::TimerInterrupt::Preemptive::RunNextWithIdleTaskSupport<RoundRobin<Task>>>,
                       public IdleTaskSupport<Task>
    {
        using IdleTaskSupport<Task>::IdleTaskSupport;
    };

    ///
    /// A fixed priority preemptive scheduler where tasks are prioritized
    /// by their defined priority and executed in a round-robin fashion
    ///
    template<typename Task, size_t MaxPriorityLevel>
    class PrioritizedRoundRobin : public Assembler<
            Policies::PrioritizedMultiQueue::Normal::ArrayMapImp<Task, PolicyMakers::DynamicFIFO<Task>, MaxPriorityLevel>,
            EventHandlers::TaskCreation::Preemptive::RunHigherPriorityWithIdleTaskSupport<PrioritizedRoundRobin<Task, MaxPriorityLevel>>,
            EventHandlers::TaskTermination::Common::RunNextWithIdleTaskSupport<PrioritizedRoundRobin<Task, MaxPriorityLevel>>,
            EventHandlers::TaskBlocked::Common::RunNextWithIdleTaskSupport<PrioritizedRoundRobin<Task, MaxPriorityLevel>>,
            EventHandlers::TaskUnblocked::Preemptive::RunNextWithIdleTaskSupport<PrioritizedRoundRobin<Task, MaxPriorityLevel>>,
            EventHandlers::TaskYielding::Common::RunNext<PrioritizedRoundRobin<Task, MaxPriorityLevel>>,
            EventHandlers::TimerInterrupt::Preemptive::RunNextWithIdleTaskSupport<PrioritizedRoundRobin<Task, MaxPriorityLevel>>>,
                                  public IdleTaskSupport<Task>
    {
        using IdleTaskSupport<Task>::IdleTaskSupport;
    };

    ///
    /// Represents a multilevel feedback queue scheduler
    ///
    template<typename Task, typename QuantumSpecifier, size_t MaxPriorityLevel>
    class MultilevelFeedbackQueue : public Assembler<
            PolicyWithEnqueueExtensions<
                    Policies::PrioritizedMultiQueue::Normal::ArrayMapImp<Task, PolicyMakers::DynamicFIFO<Task>, MaxPriorityLevel>,
                    Policies::Extensions::PriorityBasedTaskQuantumAllocator<Task, QuantumSpecifier>
                    >,
            EventHandlers::TaskCreation::Preemptive::RunHigherPriorityWithIdleTaskSupport<MultilevelFeedbackQueue<Task, QuantumSpecifier, MaxPriorityLevel>>,
            EventHandlers::TaskTermination::Common::RunNextWithIdleTaskSupport<MultilevelFeedbackQueue<Task, QuantumSpecifier, MaxPriorityLevel>>,
            EventHandlers::TaskBlocked::Common::RunNextWithIdleTaskSupport<MultilevelFeedbackQueue<Task, QuantumSpecifier, MaxPriorityLevel>>,
            EventHandlers::TaskUnblocked::Preemptive::RunNextWithIdleTaskSupport<MultilevelFeedbackQueue<Task, QuantumSpecifier, MaxPriorityLevel>>,
            EventHandlers::TaskYielding::Common::RunNext<MultilevelFeedbackQueue<Task, QuantumSpecifier, MaxPriorityLevel>>,
            EventHandlers::TimerInterrupt::Preemptive::KeepRunningCurrentWithAutoDemotionOnQuantumUsedUpAndIdleTaskSupport<MultilevelFeedbackQueue<Task, QuantumSpecifier, MaxPriorityLevel>>>,
                                    public IdleTaskSupport<Task>
    {
        using IdleTaskSupport<Task>::IdleTaskSupport;
    };

    ///
    /// A scheduler that arranges real-time tasks based on their deadline,
    /// where a task that has the earliest deadline has the highest priority
    ///
    template<typename Task>
    class EarliestDeadlineFirst: public Assembler<
            Policies::PrioritizedSingleQueue::Normal::LinkedListImp<Task>,
            EventHandlers::TaskCreation::Preemptive::RunHigherPriorityWithIdleTaskSupport<EarliestDeadlineFirst<Task>>,
            EventHandlers::TaskTermination::Common::RunNextWithIdleTaskSupport<EarliestDeadlineFirst<Task>>,
            EventHandlers::TimerInterrupt::Cooperative::KeepRunningCurrent<EarliestDeadlineFirst<Task>>>,
                                 public IdleTaskSupport<Task>
    {
        using IdleTaskSupport<Task>::IdleTaskSupport;
    };

    /// A scheduler that arranges periodic real-time tasks based on their periods,
    /// where a task that has the lowest period has the highest priority
    // `RateMonotonicScheduler`
}

namespace Scheduler::Traits
{
    template <typename T>
    struct SchedulerTraits<SampleSchedulers::FIFO<T>>
    {
        using Task = T;
    };

    template <typename T>
    struct SchedulerTraits<SampleSchedulers::RoundRobin<T>>
    {
        using Task = T;
    };

    template<typename T, size_t MaxPriorityLevel>
    struct SchedulerTraits<SampleSchedulers::PrioritizedRoundRobin<T, MaxPriorityLevel>>
    {
        using Task = T;
    };

    template<typename T, typename QuantumSpecifier, size_t MaxPriorityLevel>
    struct SchedulerTraits<SampleSchedulers::MultilevelFeedbackQueue<T, QuantumSpecifier, MaxPriorityLevel>>
    {
        using Task = T;
    };

    template <typename T>
    struct SchedulerTraits<SampleSchedulers::EarliestDeadlineFirst<T>>
    {
        using Task = T;
    };
}

#endif /* SampleSchedulers_hpp */
