//
//  PrioritizedRoundRobinSchedulerTest.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef PrioritizedRoundRobinSchedulerTest_hpp
#define PrioritizedRoundRobinSchedulerTest_hpp

#include "SchedulerTest.hpp"

class PrioritizedRoundRobinSchedulerTest: public SchedulerTest
{
public:
    PrioritizedRoundRobinSchedulerTest() : SchedulerTest("Prioritized Round Robin") {}

private:
    void runPrimitivesTest() override;

    void runTaskManagerDelegateTest() override;

    void runTimerInterruptDelegateTest() override;

    void runGroupOperationsTest() override;
};

#endif /* PrioritizedRoundRobinSchedulerTest_hpp */
