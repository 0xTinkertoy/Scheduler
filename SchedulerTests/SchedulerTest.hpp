//
//  SchedulerTest.hpp
//  Scheduler
//
//  Created by FireWolf on 2020-6-16.
//  Copyright © 2020 FireWolf. All rights reserved.
//

#ifndef SchedulerTest_hpp
#define SchedulerTest_hpp

#include <TestSuite.hpp>
#include <Debug.hpp>

/// Basic interface of a scheduler test suite
class SchedulerTest: public TestSuite
{
private:
    /// Test scheduler primitives: ready() and next()
    virtual void runPrimitivesTest() = 0;

    /// Test task manager delegate implementation
    virtual void runTaskManagerDelegateTest() = 0;

    /// Test timer interrupt delegate implementation
    virtual void runTimerInterruptDelegateTest() = 0;

    /// Test group operations
    virtual void runGroupOperationsTest() = 0;
    
public:
    /// Test name
    const char* name;

    /// Default constructor
    explicit SchedulerTest(const char* name)
    {
        this->name = name;
    }
    
    /// Run the test
    void run() override
    {
        pinfo("============================================");
        pinfo("Running Scheduler Primitives Test...");
        pinfo("============================================");

        this->runPrimitivesTest();

        pinfo("============================================");
        pinfo("Running Task Manager Delegate IMP Test...");
        pinfo("============================================");

        this->runTaskManagerDelegateTest();

        pinfo("============================================");
        pinfo("Running Timer Interrupt Delegate IMP Test...");
        pinfo("============================================");

        this->runTimerInterruptDelegateTest();

        pinfo("============================================");
        pinfo("Running Group Operations Test...");
        pinfo("============================================");

        this->runGroupOperationsTest();
    }
};

#endif /* SchedulerTest_hpp */
