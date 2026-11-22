#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HW2GuardPoints.generated.h"

UCLASS()
class MECHANICSHW1_API UBTTask_HW2PatrolPoint : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_HW2PatrolPoint();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    UPROPERTY(EditAnywhere, Category = "Patrol", meta = (ClampMin = "100.0", Units = "cm"))
    float PatrolRadius = 1200.0f;
};

UCLASS()
class MECHANICSHW1_API UBTTask_HW2SearchPoint : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_HW2SearchPoint();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    FVector PreviousOrigin = FVector::ZeroVector;
    double LastVisitTime = -10.0;
    int32 VisitCount = 0;
};
