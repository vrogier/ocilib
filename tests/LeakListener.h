#pragma once

#include "ocilib_tests.h"
#include "EnvironmentManager.h"


class LeakListener : public ::testing::EmptyTestEventListener
{
public:

    LeakListener(EnvironmentManager& environmentManager) : _environmentManager(environmentManager)
    {
    }
 
    void OnTestProgramStart(const ::testing::UnitTest&) override
    {
        _environmentManager.ResetStatsDelta();
    }

    void OnTestEnd(const ::testing::TestInfo& info) override
    {
        _environmentManager.TrackStatsDelta(info);
    }

private:
    EnvironmentManager& _environmentManager;
};
