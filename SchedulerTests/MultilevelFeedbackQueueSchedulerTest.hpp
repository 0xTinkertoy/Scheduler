//
//  MultilevelFeedbackQueueSchedulerTest.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef MultilevelFeedbackQueueSchedulerTest_hpp
#define MultilevelFeedbackQueueSchedulerTest_hpp

#include "SchedulerTest.hpp"

class MultilevelFeedbackQueueSchedulerTest: public SchedulerTest
{
public:
    MultilevelFeedbackQueueSchedulerTest() : SchedulerTest("Multilevel Feedback Queue") {}

private:
    void runPrimitivesTest() override;

    void runTaskManagerDelegateTest() override;

    void runTimerInterruptDelegateTest() override;

    void runGroupOperationsTest() override;
};

#endif /* MultilevelFeedbackQueueSchedulerTest_hpp */
