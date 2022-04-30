#ifdef EP_BUILD_DYNAMIC
#include "Enterprise/GameEntryPoint.h"

void (*GameSysInit_Ptr)() = nullptr;
void (*GameSysCleanup_Ptr)() = nullptr;
void (*GameInit_Ptr)() = nullptr;
void (*GameCleanup_Ptr)() = nullptr;
void (*PieInit_Ptr)() = nullptr;
void (*PieCleanup_Ptr)() = nullptr;

void GameSysInit()
{
    if (::GameSysInit_Ptr)
        ::GameSysInit_Ptr();
}
void GameSysCleanup()
{
    if (::GameSysCleanup_Ptr)
        ::GameSysCleanup_Ptr();
}
void GameInit()
{
    if (::GameInit_Ptr)
        ::GameInit_Ptr();
}
void GameCleanup()
{
    if (::GameCleanup_Ptr)
        ::GameCleanup_Ptr();
}
void PieInit()
{
    if (::PieInit_Ptr)
        ::PieInit_Ptr();
}
void PieCleanup()
{
    if (::PieCleanup_Ptr)
        ::PieCleanup_Ptr();
}

#endif
