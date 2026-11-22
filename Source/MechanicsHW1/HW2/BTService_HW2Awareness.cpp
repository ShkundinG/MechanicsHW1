#include "BTService_HW2Awareness.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HW2GuardCharacter.h"
#include "HW2GuardController.h"
#include "Kismet/GameplayStatics.h"
#include "MechanicsHW1Character.h"

UBTService_HW2Awareness::UBTService_HW2Awareness()
    : SearchState(EHW2GuardState::Patrol)
{
    NodeName = TEXT("Observe sight and sprint footsteps");
    Interval = 0.15f;
    RandomDeviation = 0.0f;
    bCreateNodeInstance = true;
    INIT_SERVICE_NODE_NOTIFY_FLAGS();
}

void UBTService_HW2Awareness::RefreshSearch(UBlackboardComponent& Blackboard, const FVector& StimulusLocation,
    double Until, EHW2GuardState State)
{
    const FVector PreviousOrigin = Blackboard.GetValueAsVector(HW2GuardKeys::SearchOrigin);
    if (!Blackboard.IsVectorValueSet(HW2GuardKeys::SearchOrigin) ||
        FVector::DistSquared(PreviousOrigin, StimulusLocation) >= FMath::Square(150.0))
    {
        Blackboard.SetValueAsVector(HW2GuardKeys::SearchOrigin, StimulusLocation);
    }

    SearchUntil = FMath::Max(SearchUntil, Until);
    SearchState = State;
}

void UBTService_HW2Awareness::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* Controller = OwnerComp.GetAIOwner();
    AHW2GuardCharacter* Guard = Controller ? Cast<AHW2GuardCharacter>(Controller->GetPawn()) : nullptr;
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!Guard || !Blackboard)
    {
        return;
    }

    const double Now = Guard->GetWorld()->GetTimeSeconds();
    APawn* Player = UGameplayStatics::GetPlayerPawn(Guard, 0);
    const bool bWasChasing = Blackboard->GetValueAsObject(HW2GuardKeys::TargetActor) != nullptr;
    const FVector ToPlayer = Player ? Player->GetActorLocation() - Guard->GetActorLocation() : FVector::ZeroVector;
    const double DistanceSquared = ToPlayer.SizeSquared();
    const double SightLimit = bWasChasing ? FMath::Max(SightRadius, LoseSightRadius) : SightRadius;
    const double ForwardDot = FVector::DotProduct(Guard->GetActorForwardVector(), ToPlayer.GetSafeNormal2D());
    const bool bWithinView = bWasChasing || ToPlayer.SizeSquared2D() < FMath::Square(120.0) ||
        ForwardDot >= FMath::Cos(FMath::DegreesToRadians(PeripheralAngle));
    const bool bCanSee = Player && DistanceSquared <= FMath::Square(SightLimit) &&
        bWithinView && Controller->LineOfSightTo(Player);

    if (bCanSee)
    {
        LastSeenLocation = Player->GetNavAgentLocation();
        const float NormalizedDistance = FMath::Clamp(static_cast<float>(FMath::Sqrt(DistanceSquared) / SightRadius), 0.0f, 1.0f);
        const float ConfirmSeconds = FMath::Lerp(0.35f, 1.15f, NormalizedDistance);
        Suspicion = bWasChasing ? 1.0f : FMath::Min(1.0f, Suspicion + DeltaSeconds / ConfirmSeconds);

        if (Suspicion >= 1.0f)
        {
            Guard->SetGuardState(EHW2GuardState::Chase);
            Blackboard->SetValueAsFloat(HW2GuardKeys::Suspicion, Suspicion);
            Blackboard->SetValueAsObject(HW2GuardKeys::TargetActor, Player);
            Blackboard->ClearValue(HW2GuardKeys::SearchOrigin);
            SearchUntil = 0.0;
            return;
        }

        RefreshSearch(*Blackboard, LastSeenLocation, Now + 2.5, EHW2GuardState::Suspicious);
    }
    else if (bWasChasing)
    {
        Suspicion = 0.55f;
        RefreshSearch(*Blackboard, LastSeenLocation, Now + 5.0, EHW2GuardState::Searching);
        Blackboard->ClearValue(HW2GuardKeys::TargetActor);
    }
    else
    {
        Suspicion = FMath::Max(0.0f, Suspicion - DeltaSeconds * 0.3f);
        const AMechanicsHW1Character* Runner = Cast<AMechanicsHW1Character>(Player);
        if (Runner && Now >= NextFootstepSample && DistanceSquared <= FMath::Square(SprintHearingRadius) &&
            Runner->bIsSprinting && Runner->GetCharacterMovement()->IsMovingOnGround() &&
            Runner->GetVelocity().SizeSquared2D() >= FMath::Square(300.0))
        {
            NextFootstepSample = Now + 0.45;
            Suspicion = FMath::Max(Suspicion, 0.30f);
            RefreshSearch(*Blackboard, Runner->GetNavAgentLocation(), Now + 4.0, EHW2GuardState::InvestigatingNoise);
        }
    }

    Blackboard->SetValueAsFloat(HW2GuardKeys::Suspicion, Suspicion);
    if (Now < SearchUntil && Blackboard->IsVectorValueSet(HW2GuardKeys::SearchOrigin))
    {
        Guard->SetGuardState(SearchState);
    }
    else
    {
        Blackboard->ClearValue(HW2GuardKeys::SearchOrigin);
        Guard->SetGuardState(EHW2GuardState::Patrol);
    }
}
