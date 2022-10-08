//
//  SimpleTask.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef SimpleTask_hpp
#define SimpleTask_hpp

#include <Types.hpp>
#include <LinkedList.hpp>
#include <Scheduler/Scheduler.hpp>
#include <Debug.hpp>

class SimpleTask: public Listable<SimpleTask>, public Scheduler::Schedulable
{
private:
    uint32_t identifier;

    uint32_t priority;

    uint32_t ticks;

public:
    // MARK: Constructor
    SimpleTask(uint32_t identifier, uint32_t priority) :
            Listable(), identifier(identifier), priority(priority), ticks(0) {}

    // MARK: Prioritizable By Mutable Priority IMP
    using Priority = uint32_t;

    [[nodiscard]]
    const uint32_t& getPriority() const
    {
        return this->priority;
    }

    void setPriority(const uint32_t& priority)
    {
        this->priority = priority;

        pinfo("SimpleTask%u: Now has a priority of %u.", this->identifier, this->priority);
    }

    void demote()
    {
        if (this->priority > 1)
        {
            this->priority -= 1;
        }
    }

    void promote()
    {
        pinfo("Not supported.");
    }

    // MARK: Quantizable IMP
    using Tick = uint32_t;

    void tick()
    {
        this->ticks -= 1;

        pinfo("SimpleTask%u: Remaining ticks is %u after tick.", this->identifier, this->ticks);
    }

    bool hasUsedUpTimeAllotment()
    {
        return this->ticks == 0;
    }

    void allocateTicks(uint32_t ticks)
    {
        this->ticks = ticks;

        pinfo("SimpleTask%u: Allocated ticks = %u.", this->identifier, this->ticks);
    }

    [[nodiscard]]
    uint32_t getIdentifier() const
    {
        return this->identifier;
    }

    // Print task info
    void print() const
    {
        pinfo("SimpleTask%u: Priority = %u; Quantum = %u.", this->identifier, this->priority, this->ticks);
    }

    // Quantum Specifier
    struct QuantumSpecifier
    {
        uint32_t operator()(const uint32_t& priority)
        {
            switch (priority)
            {
                case 0:
                    pfatal("Should never allocate ticks for idle task.");

                case 1:
                    // Infinite ticks (Run to completion)
                    return UINT32_MAX;

                case 2:
                    return 2;

                case 3:
                    return 1;

                default:
                    pfatal("Invalid priority level. Supported: 0 (Reserved), 1, 2, 3.");
            }
        }
    };
};

#endif /* SimpleTask_hpp */
