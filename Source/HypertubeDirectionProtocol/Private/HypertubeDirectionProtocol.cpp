#include "HypertubeDirectionProtocol.h"
#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"
#include "FGPipeConnectionComponent.h"
#include "FGCharacterMovementComponent.h"
#include "NativeHookManager.h"
#include "GameFramework/PlayerController.h"
#include "FGCharacterPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "Tickable.h"
#include "FGHUD.h"
#include "WidgetBlueprintLibrary.h"
#include "Components/Widget.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

#define LOCTEXT_NAMESPACE "FHypertubeDirectionProtocolModule"

class FHypertubeDirectionTickObject : public FTickableGameObject
{
public:
	FHypertubeDirectionTickObject()
		: FrameCounter(0)
	{}

	virtual void Tick(float DeltaTime) override
	{
		constexpr int32 FramesToSkip = 5;
		if (++FrameCounter % FramesToSkip != 0)
			return;

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
					Index = 0;
				}
				else if (bDownD && !bDownA)
				{
					Index = Outputs.Num() - 1;
				}

				if (Outputs.IsValidIndex(Index))
				{
					Player->Server_UpdateHyperJunctionOutputConnection(Pending.mConnectionEnteredThrough, Outputs[Index].Connection);
				}
			}

			APlayerController* PC = Cast<APlayerController>(Player->GetController());
			if (PC)
			{
				TArray<UUserWidget*> Widgets;
				UWidgetBlueprintLibrary::GetAllWidgetsOfClass(Player->GetWorld(), Widgets, UUserWidget::StaticClass(), false);

				for (UUserWidget* Widget : Widgets)
				{
					if (!IsValid(Widget)) continue;
					FString Name = Widget->GetName();

					if (Name.Contains("Widget_Hint"))
					{
						if (Widget->WidgetTree)
						{
							TArray<UWidget*> Children;
							Widget->WidgetTree->GetAllWidgets(Children);
							for (UWidget* Child : Children)
							{
								if (Child->GetName().Contains("mHintDescription"))
								{
									UTextBlock* TextBlock = Cast<UTextBlock>(Child);
									if (IsValid(TextBlock) && TextBlock->GetText().ToString().Contains("Cycle Travel Direction"))
									{
										TextBlock->SetText(FText::FromString("Pick Left/Right Direction"));
									}
								}
								else if (Child->GetName().Contains("mHintKey"))
								{
									UTextBlock* TextBlock = Cast<UTextBlock>(Child);
									if (IsValid(TextBlock))
									{
										TextBlock->SetText(FText::FromString("A/D"));
									}
								}
							}
						}
					}
				}
			}
		}
	}

	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FHypertubeDirectionTickObject, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return true; }

private:
	int32 FrameCounter;
};




static TSharedPtr<FHypertubeDirectionTickObject> HypertubeDirectionTicker;

void FHypertubeDirectionProtocolModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] Hooking Active"));

	HypertubeDirectionTicker = MakeShared<FHypertubeDirectionTickObject>();

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHypertubeDirectionProtocolModule, HypertubeDirectionProtocol);
