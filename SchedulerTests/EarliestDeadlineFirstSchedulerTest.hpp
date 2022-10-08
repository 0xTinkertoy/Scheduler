//
//  EarliestDeadlineFirstSchedulerTest.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-18.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef EarliestDeadlineFirstSchedulerTest_hpp
#define EarliestDeadlineFirstSchedulerTest_hpp

#include "SchedulerTest.hpp"

class EarliestDeadlineFirstSchedulerTest: public SchedulerTest
{
public:
    EarliestDeadlineFirstSchedulerTest() : SchedulerTest("Earliest Deadline First") {}

private:
    void runPrimitivesTest() override;

    void runTaskManagerDelegateTest() override;

    void runTimerInterruptDelegateTest() override;

    void runGroupOperationsTest() override;
};

#endif /* EarliestDeadlineFirstSchedulerTest_hpp */
