// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BallPlayer.generated.h"

UCLASS()
class SCORYBALL_API ABallPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABallPlayer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetCurrentHealthPoints() const { return m_CurrentHealthPoints; }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetMaxHealthPoints() const { return m_MaxHealthPoints; }

	UFUNCTION(BlueprintPure, Category = Stats)
	FORCEINLINE float GetEnergyCounter() const { return m_EnergyCounter; }
private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraSpringArm;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BallMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereTrigger;

	UPROPERTY(EditDefaultsOnly, Category = Stats)
	float m_MaxHealthPoints;

	float m_EnergyCounter;
	float m_CurrentHealthPoints;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Jump Impulse Strength*/
	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float m_JumpPower;

	/** Forward and Right Force */
	UPROPERTY(EditDefaultsOnly, Category=Movement)
	float m_TorquePower;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Jump();

	float m_MoveForwardAxisValue;
	float m_MoveRightAxisValue;

	void UpdateBallMovement();

	bool IsGrounded();
};
