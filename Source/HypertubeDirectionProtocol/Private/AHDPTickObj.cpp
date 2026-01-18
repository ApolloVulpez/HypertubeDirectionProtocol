#include "AHDPTickObj.h"

bool AHDPTickObj::TurnLeftValue = false;
bool AHDPTickObj::TurnRightValue = false;

void AHDPTickObj::TurnLeftStart()
{
	TurnLeftValue = true;
	UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] TurnLeftStart called | Left=%d"), TurnLeftValue);
}
void AHDPTickObj::TurnRightStart()
{
	TurnRightValue = true;
	UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] TurnRightStart called | Right=%d"), TurnRightValue);
}
void AHDPTickObj::TurnLeftEnd()
{
	TurnLeftValue = false;
	UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] TurnLeftEnd called | Left=%d"), TurnLeftValue);
}
void AHDPTickObj::TurnRightEnd()
{
	TurnRightValue = false;
	UE_LOG(LogTemp, Warning, TEXT("[HypertubeDirectionProtocol] TurnRightEnd called | Right=%d"), TurnRightValue);
}





void FHDPTickObj::Tick(float DeltaTime)
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
			hdpIsFlipped = false;
			if (Pending.mConnectionEnteredThrough)
			{
				AActor* JunctionActor = Pending.mConnectionEnteredThrough->GetOwner();
				if (IsValid(JunctionActor))
				{
					FRotator ActorRotation = JunctionActor->GetActorRotation();
					float Roll = FMath::Abs(FMath::UnwindDegrees(ActorRotation.Roll));
					hdpIsFlipped = (Roll > 160.0f && Roll < 200.0f);
				}
			}
			APlayerController* PC = Cast<APlayerController>(Player->GetController());



			FKey ChooseLeftKey;
			FKey ChooseRightKey;
			TArray<FKey> ShiftKeys;

			UFGInputLibrary::GetCurrentMappingForAction(PC, "HDP.ChooseLeft", ChooseLeftKey, ShiftKeys);
			UFGInputLibrary::GetCurrentMappingForAction(PC, "HDP.ChooseRight", ChooseRightKey, ShiftKeys);

			ChooseLeftKeyCurrent = ChooseLeftKey.GetFName();
			ChooseRightKeyCurrent = ChooseRightKey.GetFName();
			if (!hdpIsFlipped)
			{

				hdpLeft = AHDPTickObj::TurnLeftValue;
				hdpRight = AHDPTickObj::TurnRightValue;
			}
			else
			{
				hdpLeft = AHDPTickObj::TurnRightValue;
				hdpRight = AHDPTickObj::TurnLeftValue;
			}

			int32 Index = INDEX_NONE;
			if (hdpLeft && !hdpRight)
			{
				Index = 0;
			}
			else if (hdpRight && !hdpLeft)
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
								if (IsValid(TextBlock))
								{
									FString CurrentText = TextBlock->GetText().ToString();
									if (CurrentText.Contains("Cycle Travel Direction"))
									{
										TextBlock->SetText(FText::FromString("Pick Left/Right Direction"));
									}
								}
							}
							else if (Child->GetName().Contains("mHintKey"))
							{
								UTextBlock* TextBlock = Cast<UTextBlock>(Child);
								UImage* ImageWidget = Cast<UImage>(Child);
								if (IsValid(TextBlock))
								{
									TextBlock->SetText(FText::FromString(ChooseLeftKeyCurrent.ToString() + "/" + ChooseRightKeyCurrent.ToString()));
								}
							}
						}
					}
				}
			}
		}
	}
};

