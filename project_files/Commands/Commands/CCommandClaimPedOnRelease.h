#pragma once
#include "../CCustomCommand.h"
class CCommandClaimPedOnRelease :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

