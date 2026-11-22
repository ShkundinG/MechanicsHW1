#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HW2GuardCharacter.generated.h"

class UBehaviorTree;

UENUM(BlueprintType)
enum class EHW2GuardState : uint8
{
    Patrol,
    InvestigatingNoise,
    Suspicious,
    Chase,
    Searching
};

UCLASS()
class MECHANICSHW1_API AHW2GuardCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AHW2GuardCharacter();

    void SetGuardState(EHW2GuardState NewState);
    UBehaviorTree* LoadBehaviorTree() const;

private:
    UPROPERTY(EditAnywhere, Category = "Guard")
    TSoftObjectPtr<UBehaviorTree> BehaviorTree;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Guard",
        meta = (AllowPrivateAccess = "true"))
    EHW2GuardState GuardState = EHW2GuardState::Patrol;

    UPROPERTY(EditAnywhere, Category = "Guard|Movement", meta = (ClampMin = "1.0"))
    float PatrolSpeed = 260.0f;

    UPROPERTY(EditAnywhere, Category = "Guard|Movement", meta = (ClampMin = "1.0"))
    float SearchSpeed = 350.0f;

    UPROPERTY(EditAnywhere, Category = "Guard|Movement", meta = (ClampMin = "1.0"))
    float ChaseSpeed = 650.0f;
};
