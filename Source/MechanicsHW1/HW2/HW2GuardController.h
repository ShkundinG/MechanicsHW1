#pragma once

#include "AIController.h"
#include "HW2GuardController.generated.h"

namespace HW2GuardKeys
{
    inline const FName HomeLocation(TEXT("HomeLocation"));
    inline const FName TargetActor(TEXT("TargetActor"));
    inline const FName SearchOrigin(TEXT("SearchOrigin"));
    inline const FName MoveLocation(TEXT("MoveLocation"));
    inline const FName Suspicion(TEXT("Suspicion"));
}

UCLASS()
class MECHANICSHW1_API AHW2GuardController : public AAIController
{
    GENERATED_BODY()

protected:
    virtual void OnPossess(APawn* InPawn) override;
};
