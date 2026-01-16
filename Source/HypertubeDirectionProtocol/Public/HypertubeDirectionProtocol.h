#pragma once

#include "Modules/ModuleManager.h"
#include "FGInputLibrary.h"

// Forward declaration to avoid include bloat
class UFGCharacterMovementComponent;

class FHDPModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
};

