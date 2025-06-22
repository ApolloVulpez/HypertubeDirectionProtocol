#pragma once

#include "Modules/ModuleManager.h"

// Forward declaration to avoid include bloat
class UFGCharacterMovementComponent;

class FHypertubeDirectionProtocolModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;

};