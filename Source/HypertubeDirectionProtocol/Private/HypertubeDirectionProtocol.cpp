#include "HypertubeDirectionProtocol.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"
#include "FGPipeConnectionComponent.h"
#include "FGCharacterMovementComponent.h"
#include "NativeHookManager.h"
#include "ModLoadingLibrary.h"
#include "SatisfactoryModLoader.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/IConsoleManager.h"
#include "HyperFunctions.h"
#include "FGCharacterPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerInput.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "Tickable.h"

#define LOCTEXT_NAMESPACE "FHypertubeDirectionProtocolModule"

class FHypertubeDirectionTickObject : public FTickableGameObject
{
public:
	virtual void Tick(float DeltaTime) override
	{
		for (TObjectIterator<AFGCharacterPlayer> It; It; ++It)
		{
			AFGCharacterPlayer* Player = *It;
			if (!IsValid(Player)) continue;

			UFGCharacterMovementComponent* MoveComp = Cast<UFGCharacterMovementComponent>(Player->GetMovementComponent());
			if (!MoveComp || !MoveComp->IsInHyperPipe()) continue;

			const FFGPendingHyperJunctionInfo& Pending = MoveComp->GetPendingHyperJunction();
			const TArray<FFGHypertubeJunctionOutputConnectionInfo>& Outputs = Pending.mAvailableOutputConnections;

			if (Outputs.Num() > 1 && Pending.mConnectionEnteredThrough)
			{
				APlayerController* PC = Cast<APlayerController>(Player->GetController());
				if (!PC) continue;

				bool bDownA = PC->IsInputKeyDown(EKeys::A);
				bool bDownD = PC->IsInputKeyDown(EKeys::D);

				int32 Index = INDEX_NONE;
				if (bDownA && !bDownD)
				{
					UE_LOG(LogTemp, Log, TEXT("[HypertubeDirectionProtocol] A pressed - Selecting Left Junction Output"));
					Index = 0;
				}
				else if (bDownD && !bDownA)
				{
					UE_LOG(LogTemp, Log, TEXT("[HypertubeDirectionProtocol] D pressed - Selecting Right Junction Output"));
					Index = Outputs.Num() - 1;
				}

				if (Outputs.IsValidIndex(Index))
				{
					Player->Server_UpdateHyperJunctionOutputConnection(Pending.mConnectionEnteredThrough, Outputs[Index].Connection);
				}
			}
		}
	}

	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FHypertubeDirectionTickObject, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return true; }
};

static TSharedPtr<FHypertubeDirectionTickObject> HypertubeDirectionTicker;

void FHypertubeDirectionProtocolModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] StartupModule running"));

	HypertubeDirectionTicker = MakeShared<FHypertubeDirectionTickObject>();

	SUBSCRIBE_METHOD(AFGCharacterPlayer::Server_CycleHyperJunctionOutputConnection,
		[](TCallScope<void(*)(AFGCharacterPlayer*, UFGPipeConnectionComponentBase*)>& Scope,
			AFGCharacterPlayer* self,
			UFGPipeConnectionComponentBase* param)
		{
			UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] Hooked Server_CycleHyperJunctionOutputConnection"));
			UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] Allowing Server_CycleHyperJunctionOutputConnection to run"));
			Scope(self, param);
		});

	SUBSCRIBE_METHOD(AFGCharacterPlayer::Server_UpdateHyperJunctionOutputConnection,
		[](TCallScope<void(*)(AFGCharacterPlayer*, UFGPipeConnectionComponentBase*, UFGPipeConnectionComponentBase*)>& Scope,
			AFGCharacterPlayer* self,
			UFGPipeConnectionComponentBase* entered,
			UFGPipeConnectionComponentBase* output)
		{
			UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] Hooked Server_UpdateHyperJunctionOutputConnection"));
			UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] Allowing Server_UpdateHyperJunctionOutputConnection to run"));
			Scope(self, entered, output);
		});
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHypertubeDirectionProtocolModule, HypertubeDirectionProtocol);
