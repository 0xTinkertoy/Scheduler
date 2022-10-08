//
//  FIFOSchedulerTest.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef FIFOSchedulerTest_hpp
#define FIFOSchedulerTest_hpp

#include "SchedulerTest.hpp"

class FIFOSchedulerTest: public SchedulerTest
{
public:
    FIFOSchedulerTest() : SchedulerTest("FIFO") {}

private:
    void runPrimitivesTest() override;

    void runTaskManagerDelegateTest() override;

    void runTimerInterruptDelegateTest() override;

    void runGroupOperationsTest() override;
};

#endif /* FIFOSchedulerTest_hpp */
