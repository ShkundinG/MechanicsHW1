#pragma once

#include "BehaviorTree/BTService.h"
#include "BTService_HW2Awareness.generated.h"

class UBlackboardComponent;
enum class EHW2GuardState : uint8;

UCLASS()
class MECHANICSHW1_API UBTService_HW2Awareness : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_HW2Awareness();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    void RefreshSearch(UBlackboardComponent& Blackboard, const FVector& StimulusLocation,
        double Until, EHW2GuardState State);

    UPROPERTY(EditAnywhere, Category = "Sight", meta = (ClampMin = "1.0", Units = "cm"))
    float SightRadius = 1500.0f;

    UPROPERTY(EditAnywhere, Category = "Sight", meta = (ClampMin = "1.0", Units = "cm"))
    float LoseSightRadius = 2050.0f;

    UPROPERTY(EditAnywhere, Category = "Sight", meta = (ClampMin = "0.0", ClampMax = "180.0", Units = "deg"))
    float PeripheralAngle = 75.0f;

    UPROPERTY(EditAnywhere, Category = "Hearing", meta = (ClampMin = "1.0", Units = "cm"))
    float SprintHearingRadius = 1850.0f;

    float Suspicion = 0.0f;
    double SearchUntil = 0.0;
    double NextFootstepSample = 0.0;
    FVector LastSeenLocation = FVector::ZeroVector;
    EHW2GuardState SearchState;
};
