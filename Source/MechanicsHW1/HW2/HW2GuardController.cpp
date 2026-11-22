#include "HW2GuardController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "HW2GuardCharacter.h"
#include "MechanicsHW1.h"

void AHW2GuardController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    AHW2GuardCharacter* Guard = Cast<AHW2GuardCharacter>(GetPawn());
    if (!Guard)
    {
        return;
    }

    UBehaviorTree* Tree = Guard->LoadBehaviorTree();
    UBlackboardComponent* BlackboardComponent = nullptr;
    if (!Tree || !Tree->BlackboardAsset ||
        !UseBlackboard(Tree->BlackboardAsset, BlackboardComponent) || !BlackboardComponent)
    {
        UE_LOG(LogMechanicsHW1, Error, TEXT("Cannot initialize the guard's behavior tree or Blackboard for %s."), *GetNameSafe(Guard));
        return;
    }

    BlackboardComponent->SetValueAsVector(HW2GuardKeys::HomeLocation, Guard->GetNavAgentLocation());
    BlackboardComponent->ClearValue(HW2GuardKeys::TargetActor);
    BlackboardComponent->ClearValue(HW2GuardKeys::SearchOrigin);
    BlackboardComponent->ClearValue(HW2GuardKeys::MoveLocation);
    BlackboardComponent->SetValueAsFloat(HW2GuardKeys::Suspicion, 0.0f);
    Guard->SetGuardState(EHW2GuardState::Patrol);

    if (!RunBehaviorTree(Tree))
    {
        UE_LOG(LogMechanicsHW1, Error, TEXT("Cannot start the guard's behavior tree for %s."), *GetNameSafe(Guard));
    }
}
