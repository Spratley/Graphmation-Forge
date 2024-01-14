#include "Timer.h"

#define NOW() std::chrono::steady_clock::now()

GraphmationTimer* GraphmationTimer::s_instance;

void GraphmationTimer::Init()
{
    s_instance = new GraphmationTimer;
    s_instance->m_startTime = NOW();
}

TimePoint GraphmationTimer::GetCurrentTimeMS()
{
    ChronoTimePoint now = NOW();
    auto duration = now - s_instance->m_startTime;
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}