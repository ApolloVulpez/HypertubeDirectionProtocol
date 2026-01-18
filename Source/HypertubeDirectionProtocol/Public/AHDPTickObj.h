#pragma once
#include "CoreMinimal.h"
#include "Subsystem/ModSubsystem.h"
#include <string>
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
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "FGInputLibrary.h"


#include "AHDPTickObj.generated.h"


UCLASS()
class HYPERTUBEDIRECTIONPROTOCOL_API AHDPTickObj : public AModSubsystem
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void TurnLeftStart();
	UFUNCTION(BlueprintCallable)
	void TurnRightStart();
	UFUNCTION(BlueprintCallable)
	void TurnLeftEnd();
	UFUNCTION(BlueprintCallable)
	void TurnRightEnd();

	static bool TurnLeftValue;
	static bool TurnRightValue;
	bool TurnLeft;
	bool TurnRight;


protected:
	
};

class FHDPTickObj : public FTickableGameObject
{
public:
	FHDPTickObj()
		: FrameCounter(0)
	{};
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FHDPTickObj, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return true; }

protected:
	int32 FrameCounter;
	FName ChooseLeftKeyCurrent;
	FName ChooseRightKeyCurrent;
	bool hdpLeft;
	bool hdpRight;
	bool hdpIsFlipped;
};

