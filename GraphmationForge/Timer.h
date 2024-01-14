#pragma once
#include <chrono>

using TimePoint = long long;
using ChronoTimePoint = std::chrono::steady_clock::time_point;

class GraphmationTimer
{
private:
    static GraphmationTimer* s_instance;

public:
    static void Init();

    static TimePoint GetCurrentTimeMS();
    
private:
    ChronoTimePoint m_startTime;
};