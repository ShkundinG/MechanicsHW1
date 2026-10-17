
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MechanicsHW1Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInteraction);

UCLASS(Abstract)
class AMechanicsHW1Character : public ACharacter
{
    GENERATED_BODY()

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

protected:
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MouseLookAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* InteractAction;

public:
    AMechanicsHW1Character();

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintAssignable, Category = "Interaction")
    FOnCharacterInteraction OnInteraction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Sprint")
    bool bIsSprinting = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Sprint")
    float WalkSpeed = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Sprint")
    float SprintSpeed = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Stamina")
    float MaxStamina = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Stamina")
    float CurrentStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Stamina")
    float StaminaDrainRate = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Stamina")
    float StaminaRegenRate = 15.0f;

protected:
    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(
        UInputComponent* PlayerInputComponent) override;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

    void StartSprint();
    void StopSprint();
    void Interact();

    void UpdateSprintState();

private:
    bool bSprintKeyHeld = false;
    bool bSprintExhausted = false;

public:
    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoMove(float Right, float Forward);

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoLook(float Yaw, float Pitch);

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoJumpStart();

    UFUNCTION(BlueprintCallable, Category = "Input")
    virtual void DoJumpEnd();

    FORCEINLINE USpringArmComponent* GetCameraBoom() const
    {
        return CameraBoom;
    }

    FORCEINLINE UCameraComponent* GetFollowCamera() const
    {
        return FollowCamera;
    }
};
