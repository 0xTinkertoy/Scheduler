//
//  SchedulerTestDriver.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef SchedulerTestDriver_hpp
#define SchedulerTestDriver_hpp

#include "FIFOSchedulerTest.hpp"
#include "RoundRobinSchedulerTest.hpp"
#include "PrioritizedRoundRobinSchedulerTest.hpp"
#include "MultilevelFeedbackQueueSchedulerTest.hpp"
#include "EarliestDeadlineFirstSchedulerTest.hpp"
#include <Debug.hpp>

class SchedulerTestDriver
{
private:
    FIFOSchedulerTest fifoSchedulerTest;

    RoundRobinSchedulerTest roundRobinSchedulerTest;

    PrioritizedRoundRobinSchedulerTest prioritizedRoundRobinSchedulerTest;

    MultilevelFeedbackQueueSchedulerTest multilevelFeedbackQueueSchedulerTest;

    EarliestDeadlineFirstSchedulerTest earliestDeadlineFirstSchedulerTest;
    
    SchedulerTest* tests[5] =
    {
        &fifoSchedulerTest,
        &roundRobinSchedulerTest,
        &prioritizedRoundRobinSchedulerTest,
        &multilevelFeedbackQueueSchedulerTest,
        &earliestDeadlineFirstSchedulerTest
    };
    
public:
    void run()
    {
        for (auto test : this->tests)
        {
            pinfo(">> %s Scheduler Test: === Started ===", test->name);

            test->run();

            pinfo(">> %s Scheduler Test: === Finished ===\n\n", test->name);
        }
    }
};


#endif /* SchedulerTestDriver_hpp */
