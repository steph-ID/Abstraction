// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DealDamageComponent.generated.h"

class UCapsuleComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABSTRACTION_API UDealDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDealDamageComponent();

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OverBodyIndex, bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OverBodyIndex);

	UCapsuleComponent* GetTriggerCapsule() const { return TriggerCapsule; }

	bool IsActive() const { return bActive; }
	void SetActive(bool IsActive) { bActive = IsActive; }

protected:
	UPROPERTY(EditAnywhere)
	float BaseDamage = 50.0f;

	UPROPERTY(EditAnywhere, NoClear)
	UCapsuleComponent* TriggerCapsule;

	bool bActive = true;
};
