//
//  RoundRobinSchedulerTest.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef RoundRobinSchedulerTest_hpp
#define RoundRobinSchedulerTest_hpp

#include "SchedulerTest.hpp"

class RoundRobinSchedulerTest: public SchedulerTest
{
public:
    RoundRobinSchedulerTest() : SchedulerTest("Round Robin") {}

private:
    void runPrimitivesTest() override;

    void runTaskManagerDelegateTest() override;

    void runTimerInterruptDelegateTest() override;

    void runGroupOperationsTest() override;
};

#endif /* RoundRobinSchedulerTest_hpp */
