/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "ocilib_tests.h"

struct Statistics
{
    OCI_UInt64 Bytes;
    OCI_UInt64 Handles;
    OCI_UInt64 Descriptors;
    OCI_UInt64 Objects;
};

class EnvironmentManager
{
public:

    EnvironmentManager()
    {   
        _statsc = {};
        _statcpp = {};

        _envc = (OCI_EnvironmentCreate)(NULL, HOME, OCI_ENV_DEFAULT | OCI_ENV_THREADED | OCI_ENV_EVENTS);
        _envcpp = new ocilib::Environment(ocilib::Environment::Default | ocilib::Environment::Threaded | ocilib::Environment::Events, HOME);

        WarmupEnvironment(_envc);
        WarmupEnvironment(_envcpp->GetHandle());

        ResetStatsDelta();

    }
    ~EnvironmentManager()
    {
        delete _envcpp;       
        (OCI_EnvironmentFree)(_envc);
    }

    OCI_Environment* GetEnvironmentCApi() 
    {
        return _envc; 
    }

    ocilib::Environment& GetEnvironmentCppApi()
    {
        return *_envcpp;
    }

    void ResetStatsDelta()
    {
        ResetEnvironment(_envc);
        ResetEnvironment(_envcpp->GetHandle());

        _statsc = GetEnvironmentStatistics(_envc);
        _statcpp = GetEnvironmentStatistics(_envcpp->GetHandle());
    }

    void TrackStatsDelta(const ::testing::TestInfo& info)
    {
        ResetEnvironment(_envc);
        ResetEnvironment(_envcpp->GetHandle());

        auto new_statsc = GetEnvironmentStatistics(_envc);
        auto new_statcpp = GetEnvironmentStatistics(_envcpp->GetHandle());

        auto delta_statsc = GetStatisticsDiff(new_statsc, _statsc);
        auto delta_statcpp = GetStatisticsDiff(new_statcpp, _statcpp);

        CheckStatsDelta(info, delta_statsc);
        CheckStatsDelta(info, delta_statcpp);

        _statsc = new_statsc;
        _statcpp = new_statcpp;
    }

private:

    void WarmupEnvironment(OCI_Environment* env)
    {
        /* pre-populate format strings so that lazy allocations do not show up as per-test memory deltas */

        OCI_EnvironmentGetFormat(env, OCI_FMT_DATE);
        OCI_EnvironmentGetFormat(env, OCI_FMT_TIMESTAMP);
        OCI_EnvironmentGetFormat(env, OCI_FMT_TIMESTAMP_TZ);
        OCI_EnvironmentGetFormat(env, OCI_FMT_NUMERIC);
        OCI_EnvironmentGetFormat(env, OCI_FMT_BINARY_DOUBLE);
        OCI_EnvironmentGetFormat(env, OCI_FMT_BINARY_FLOAT);

        /* pre-populate common locale strings */

        static const OCI_UInt32 locale_codes[] =
        {
            OCI_LSI_DAYNAME1, OCI_LSI_DAYNAME2, OCI_LSI_DAYNAME3, OCI_LSI_DAYNAME4,
            OCI_LSI_DAYNAME5, OCI_LSI_DAYNAME6, OCI_LSI_DAYNAME7,
            OCI_LSI_ABDAYNAME1, OCI_LSI_ABDAYNAME2, OCI_LSI_ABDAYNAME3, OCI_LSI_ABDAYNAME4,
            OCI_LSI_ABDAYNAME5, OCI_LSI_ABDAYNAME6, OCI_LSI_ABDAYNAME7,
            OCI_LSI_MONTHNAME1, OCI_LSI_MONTHNAME2, OCI_LSI_MONTHNAME3, OCI_LSI_MONTHNAME4,
            OCI_LSI_MONTHNAME5, OCI_LSI_MONTHNAME6, OCI_LSI_MONTHNAME7, OCI_LSI_MONTHNAME8,
            OCI_LSI_MONTHNAME9, OCI_LSI_MONTHNAME10, OCI_LSI_MONTHNAME11, OCI_LSI_MONTHNAME12,
            OCI_LSI_ABMONTHNAME1, OCI_LSI_ABMONTHNAME2, OCI_LSI_ABMONTHNAME3, OCI_LSI_ABMONTHNAME4,
            OCI_LSI_ABMONTHNAME5, OCI_LSI_ABMONTHNAME6, OCI_LSI_ABMONTHNAME7, OCI_LSI_ABMONTHNAME8,
            OCI_LSI_ABMONTHNAME9, OCI_LSI_ABMONTHNAME10, OCI_LSI_ABMONTHNAME11, OCI_LSI_ABMONTHNAME12,
            OCI_LSI_YES, OCI_LSI_NO,
            OCI_LSI_AM, OCI_LSI_PM,
            OCI_LSI_DECIMAL, OCI_LSI_GROUP,
            OCI_LSI_DATEFORMAT,
            OCI_LSI_LANGUAGE, OCI_LSI_TERRITORY, OCI_LSI_CHARACTER_SET
        };

        for (OCI_Size i = 0; i < sizeof(locale_codes) / sizeof(locale_codes[0]); i++)
        {
            (void)OCI_EnvironmentGetLocaleString(env, locale_codes[i]);
        }
    }

    void ResetEnvironment(OCI_Environment* env)
    {
        OCI_EnvironmentEnableWarnings(env, FALSE);

        OCI_EnvironmentSetFormat(env, OCI_FMT_DATE, NULL);
        OCI_EnvironmentSetFormat(env, OCI_FMT_TIMESTAMP, NULL);
        OCI_EnvironmentSetFormat(env, OCI_FMT_NUMERIC, NULL);
        OCI_EnvironmentSetFormat(env, OCI_FMT_BINARY_DOUBLE, NULL);
        OCI_EnvironmentSetFormat(env, OCI_FMT_BINARY_FLOAT, NULL);
        OCI_EnvironmentSetFormat(env, OCI_FMT_TIMESTAMP_TZ, NULL);
    }

    Statistics GetEnvironmentStatistics(OCI_Environment* env)
    {
        return Statistics
        {
            OCI_EnvironmentGetStats(env, OCI_UST_BYTES),
            OCI_EnvironmentGetStats(env, OCI_UST_HANDLES),
            OCI_EnvironmentGetStats(env, OCI_UST_DESCRIPTORS),
            OCI_EnvironmentGetStats(env, OCI_UST_OBJECTS)
        };
    }

    Statistics GetStatisticsDiff(Statistics newStats, Statistics prevStats)
    {
        auto diff = [](OCI_UInt64 n, OCI_UInt64 p) { return n > p ? n - p : 0; };
        return Statistics
        {
            diff(newStats.Bytes, prevStats.Bytes),
            diff(newStats.Handles, prevStats.Handles),
            diff(newStats.Descriptors, prevStats.Descriptors),
            diff(newStats.Objects, prevStats.Objects)
        };
    }

    void CheckStatsDelta(const ::testing::TestInfo& info, Statistics deltaStats)
    {
        if (deltaStats.Bytes || deltaStats.Handles || deltaStats.Descriptors || deltaStats.Objects)
        {
            ADD_FAILURE() << "[LEAKED] Environment memory leak detected: "
                          << info.test_case_name() << "." << info.name()
                          << " delta mem=" << deltaStats.Bytes
                          << " handles=" << deltaStats.Handles
                          << " descriptors=" << deltaStats.Descriptors
                          << " objects=" << deltaStats.Objects;
        }
    }

    OCI_Environment* _envc;
    ocilib::Environment* _envcpp;

    Statistics _statsc;
    Statistics _statcpp;
};
