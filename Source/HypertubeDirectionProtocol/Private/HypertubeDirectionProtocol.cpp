// Copyright Epic Games, Inc. All Rights Reserved.
#include "HypertubeDirectionProtocol.h"
#include "AHDPTickObj.h"


#define LOCTEXT_NAMESPACE "FHypertubeDirectionProtocolModule"

static TSharedPtr<FHDPTickObj> HyperTicker;

void FHypertubeDirectionProtocolModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] Hooking Active"));
	HyperTicker = MakeShared<FHDPTickObj>();
}

void FHypertubeDirectionProtocolModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHypertubeDirectionProtocolModule, HypertubeDirectionProtocol)
