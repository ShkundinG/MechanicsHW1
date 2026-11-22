#include "BTTask_HW2GuardPoints.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "HW2GuardController.h"
#include "NavigationSystem.h"

namespace
{
    bool FindReachablePoint(const UNavigationSystemV1& Navigation, ANavigationData& NavData, const APawn& Pawn,
        const FVector& Center, float Radius, FVector& Location)
    {
        const FVector Start = Pawn.GetNavAgentLocation();
        for (int32 Attempt = 0; Attempt < 6; ++Attempt)
        {
            FNavLocation Candidate;
            if (Navigation.GetRandomReachablePointInRadius(Center, Radius, Candidate, &NavData) &&
                FVector::DistSquared2D(Start, Candidate.Location) >= FMath::Square(120.0) &&
                Navigation.TestPathSync(FPathFindingQuery(&Pawn, NavData, Start, Candidate.Location)))
            {
                Location = Candidate.Location;
                return true;
            }
        }
        return false;
    }
}

UBTTask_HW2PatrolPoint::UBTTask_HW2PatrolPoint()
{
    NodeName = TEXT("Pick reachable patrol point near home");
}

EBTNodeResult::Type UBTTask_HW2PatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const AAIController* Controller = OwnerComp.GetAIOwner();
    const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    UNavigationSystemV1* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(OwnerComp.GetWorld());
    ANavigationData* NavData = Navigation ? Navigation->GetDefaultNavDataInstance(FNavigationSystem::DontCreate) : nullptr;
    if (!Pawn || !Blackboard || !NavData || !Blackboard->IsVectorValueSet(HW2GuardKeys::HomeLocation))
    {
        return EBTNodeResult::Failed;
    }

    const FVector Home = Blackboard->GetValueAsVector(HW2GuardKeys::HomeLocation);
    FNavLocation ProjectedHome;
    FVector Destination;
    if (!Navigation->ProjectPointToNavigation(Home, ProjectedHome, FVector(150.0, 150.0, 200.0), NavData) ||
        !FindReachablePoint(*Navigation, *NavData, *Pawn, ProjectedHome.Location, PatrolRadius, Destination))
    {
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsVector(HW2GuardKeys::MoveLocation, Destination);
    return EBTNodeResult::Succeeded;
}

UBTTask_HW2SearchPoint::UBTTask_HW2SearchPoint()
{
    NodeName = TEXT("Visit stimulus, then widen search");
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_HW2SearchPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const AAIController* Controller = OwnerComp.GetAIOwner();
    const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    UNavigationSystemV1* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(OwnerComp.GetWorld());
    ANavigationData* NavData = Navigation ? Navigation->GetDefaultNavDataInstance(FNavigationSystem::DontCreate) : nullptr;
    if (!Pawn || !Blackboard || !NavData || !Blackboard->IsVectorValueSet(HW2GuardKeys::SearchOrigin))
    {
        return EBTNodeResult::Failed;
    }

    const FVector Origin = Blackboard->GetValueAsVector(HW2GuardKeys::SearchOrigin);
    const double Now = OwnerComp.GetWorld()->GetTimeSeconds();
    if (Now - LastVisitTime > 6.0 || FVector::DistSquared(Origin, PreviousOrigin) >= FMath::Square(150.0))
    {
        VisitCount = 0;
        PreviousOrigin = Origin;
    }

    FNavLocation ProjectedOrigin;
    if (!Navigation->ProjectPointToNavigation(Origin, ProjectedOrigin, FVector(300.0, 300.0, 200.0), NavData))
    {
        return EBTNodeResult::Failed;
    }

    FVector Destination = ProjectedOrigin.Location;
    const bool bVisitOrigin = VisitCount == 0 &&
        Navigation->TestPathSync(FPathFindingQuery(Pawn, *NavData, Pawn->GetNavAgentLocation(), Destination));
    const float Radius = 280.0f + 150.0f * FMath::Min(VisitCount, 3);
    if (!bVisitOrigin && !FindReachablePoint(*Navigation, *NavData, *Pawn, ProjectedOrigin.Location, Radius, Destination))
    {
        return EBTNodeResult::Failed;
    }

    ++VisitCount;
    LastVisitTime = Now;
    Blackboard->SetValueAsVector(HW2GuardKeys::MoveLocation, Destination);
    return EBTNodeResult::Succeeded;
}
