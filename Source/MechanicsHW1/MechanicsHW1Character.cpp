
#include "MechanicsHW1Character.h"

#include "MechanicsHW1.h"
#include "Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"

AMechanicsHW1Character::AMechanicsHW1Character()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;

    GetCharacterMovement()->RotationRate =
        FRotator(0.0f, 500.0f, 0.0f);

    GetCharacterMovement()->JumpZVelocity = 500.0f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;

    GetCharacterMovement()->BrakingDecelerationWalking =
        2000.0f;

    GetCharacterMovement()->BrakingDecelerationFalling =
        1500.0f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(
        TEXT("CameraBoom"));

    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(
        TEXT("FollowCamera"));

    FollowCamera->SetupAttachment(
        CameraBoom,
        USpringArmComponent::SocketName);

    FollowCamera->bUsePawnControlRotation = false;
}

void AMechanicsHW1Character::BeginPlay()
{
    Super::BeginPlay();

    CurrentStamina = FMath::Max(0.0f, MaxStamina);

    bIsSprinting = false;
    bSprintKeyHeld = false;
    bSprintExhausted = false;

    UpdateSprintState();
}

void AMechanicsHW1Character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsSprinting)
    {
        CurrentStamina -= StaminaDrainRate * DeltaTime;

        CurrentStamina = FMath::Max(
            CurrentStamina, 0.0f);

        if (CurrentStamina <= 0.0f)
        {
            CurrentStamina = 0.0f;

            bSprintExhausted = true;
            bIsSprinting = false;

            UpdateSprintState();
        }
    }
    else
    {
        CurrentStamina += StaminaRegenRate * DeltaTime;

        CurrentStamina = FMath::Clamp(
            CurrentStamina,
            0.0f,
            FMath::Max(0.0f, MaxStamina));
    }

    if (IsLocallyControlled() && GEngine)
    {
        FString StaminaText = FString::Printf(
            TEXT("Stamina: %.1f / %.1f"),
            CurrentStamina,
            MaxStamina);

        FColor StaminaColor =
            CurrentStamina > 20.0f
            ? FColor::Green
            : FColor::Red;

        GEngine->AddOnScreenDebugMessage(
            static_cast<uint64>(GetUniqueID()),
            0.0f,
            StaminaColor,
            StaminaText);
    }
}

void AMechanicsHW1Character::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(
        PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerInputComponent);

    if (!EnhancedInputComponent)
    {
        UE_LOG(
            LogMechanicsHW1,
            Error,
            TEXT("Enhanced Input Component not found"));

        return;
    }

    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(
            JumpAction,
            ETriggerEvent::Started,
            this,
            &ACharacter::Jump);

        EnhancedInputComponent->BindAction(
            JumpAction,
            ETriggerEvent::Completed,
            this,
            &ACharacter::StopJumping);
    }

    if (MoveAction)
    {
        EnhancedInputComponent->BindAction(
            MoveAction,
            ETriggerEvent::Triggered,
            this,
            &AMechanicsHW1Character::Move);
    }

    if (LookAction)
    {
        EnhancedInputComponent->BindAction(
            LookAction,
            ETriggerEvent::Triggered,
            this,
            &AMechanicsHW1Character::Look);
    }

    if (MouseLookAction)
    {
        EnhancedInputComponent->BindAction(
            MouseLookAction,
            ETriggerEvent::Triggered,
            this,
            &AMechanicsHW1Character::Look);
    }

    if (SprintAction)
    {
        EnhancedInputComponent->BindAction(
            SprintAction,
            ETriggerEvent::Started,
            this,
            &AMechanicsHW1Character::StartSprint);

        EnhancedInputComponent->BindAction(
            SprintAction,
            ETriggerEvent::Completed,
            this,
            &AMechanicsHW1Character::StopSprint);

        EnhancedInputComponent->BindAction(
            SprintAction,
            ETriggerEvent::Canceled,
            this,
            &AMechanicsHW1Character::StopSprint);
    }

    if (InteractAction)
    {
        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Started,
            this,
            &AMechanicsHW1Character::Interact);
    }
}

void AMechanicsHW1Character::StartSprint()
{
    bSprintKeyHeld = true;

    if (!bSprintExhausted && CurrentStamina > 0.0f)
    {
        bIsSprinting = true;
    }

    UpdateSprintState();
}

void AMechanicsHW1Character::StopSprint()
{
    bSprintKeyHeld = false;
    bIsSprinting = false;
    bSprintExhausted = false;

    UpdateSprintState();
}

void AMechanicsHW1Character::UpdateSprintState()
{
    GetCharacterMovement()->MaxWalkSpeed =
        bIsSprinting ? SprintSpeed : WalkSpeed;
}

void AMechanicsHW1Character::Interact()
{
    OnInteraction.Broadcast();
}

void AMechanicsHW1Character::Move(
    const FInputActionValue& Value)
{
    const FVector2D MovementVector =
        Value.Get<FVector2D>();

    DoMove(MovementVector.X, MovementVector.Y);
}

void AMechanicsHW1Character::Look(
    const FInputActionValue& Value)
{
    const FVector2D LookVector =
        Value.Get<FVector2D>();

    DoLook(LookVector.X, LookVector.Y);
}

void AMechanicsHW1Character::DoMove(
    float Right,
    float Forward)
{
    if (GetController() == nullptr)
    {
        return;
    }

    const FRotator Rotation =
        GetController()->GetControlRotation();

    const FRotator YawRotation(
        0.0f,
        Rotation.Yaw,
        0.0f);

    const FVector ForwardDirection =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    const FVector RightDirection =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, Forward);
    AddMovementInput(RightDirection, Right);
}

void AMechanicsHW1Character::DoLook(
    float Yaw,
    float Pitch)
{
    if (GetController() == nullptr)
    {
        return;
    }

    AddControllerYawInput(Yaw);
    AddControllerPitchInput(Pitch);
}

void AMechanicsHW1Character::DoJumpStart()
{
    Jump();
}

void AMechanicsHW1Character::DoJumpEnd()
{
    StopJumping();
}
