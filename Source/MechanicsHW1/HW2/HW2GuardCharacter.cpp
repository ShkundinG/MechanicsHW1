#include "HW2GuardCharacter.h"

#include "Animation/AnimInstance.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HW2GuardController.h"
#include "UObject/ConstructorHelpers.h"

AHW2GuardCharacter::AHW2GuardCharacter()
    : BehaviorTree(FSoftObjectPath(TEXT("/Game/ThirdPerson/AI/BT_HW2Guard.BT_HW2Guard")))
{
    AIControllerClass = AHW2GuardController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
    BaseEyeHeight = 70.0f;
    bUseControllerRotationYaw = false;

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->bOrientRotationToMovement = true;
    Movement->RotationRate = FRotator(0.0f, 520.0f, 0.0f);
    Movement->MaxWalkSpeed = PatrolSpeed;

    USkeletalMeshComponent* GuardMesh = GetMesh();
    GuardMesh->SetRelativeLocation(FVector(0.0, 0.0, -96.0));
    GuardMesh->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> Manny(
        TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
    static ConstructorHelpers::FClassFinder<UAnimInstance> UnarmedAnimation(
        TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"));
    if (Manny.Succeeded())
    {
        GuardMesh->SetSkeletalMeshAsset(Manny.Object);
    }
    if (UnarmedAnimation.Succeeded())
    {
        GuardMesh->SetAnimInstanceClass(UnarmedAnimation.Class);
    }
}

void AHW2GuardCharacter::SetGuardState(EHW2GuardState NewState)
{
    GuardState = NewState;
    switch (GuardState)
    {
    case EHW2GuardState::Patrol:
        GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
        break;
    case EHW2GuardState::Chase:
        GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
        break;
    default:
        GetCharacterMovement()->MaxWalkSpeed = SearchSpeed;
        break;
    }
}

UBehaviorTree* AHW2GuardCharacter::LoadBehaviorTree() const
{
    return BehaviorTree.LoadSynchronous();
}
