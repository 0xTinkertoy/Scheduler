//
//  SimpleRealtimeTask.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-18.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef SimpleRealtimeTask_hpp
#define SimpleRealtimeTask_hpp

#include <Types.hpp>
#include <LinkedList.hpp>
#include <Scheduler/Scheduler.hpp>

/// Task that has the earliest deadline has the highest priority
class SimpleRealtimeTask: public Listable<SimpleRealtimeTask>, public Scheduler::Schedulable
{
private:
    uint32_t identifier;

    uint32_t deadline;

public:
    // MARK: Constructor
    SimpleRealtimeTask(uint32_t identifier, uint32_t deadline) :
        Listable(), identifier(identifier), deadline(deadline) {}

    // MARK: Prioritizable IMP
    friend bool operator<(const SimpleRealtimeTask& lhs, const SimpleRealtimeTask& rhs)
    {
        return lhs.deadline > rhs.deadline;
    }

    friend bool operator>(const SimpleRealtimeTask& lhs, const SimpleRealtimeTask& rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const SimpleRealtimeTask& lhs, const SimpleRealtimeTask& rhs)
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const SimpleRealtimeTask& lhs, const SimpleRealtimeTask& rhs)
    {
        return !(lhs < rhs);
    }

    [[nodiscard]]
    uint32_t getIdentifier() const
    {
        return this->identifier;
    }
};

#endif /* SimpleRealtimeTask_hpp */
