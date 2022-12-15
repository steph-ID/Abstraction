// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UDoorInteractionComponent::UDoorInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDoorInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	StartRotation = GetOwner()->GetActorRotation();
	FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;
	// Ensure TimeToRotate is greater than EPSILON
	CurrentRotationTime = 0.0f;

	TriggerBox->OnActorBeginOverlap.AddDynamic(this, &UDoorInteractionComponent::OnBeginOverlap);
	TriggerBox->OnActorEndOverlap.AddDynamic(this, &UDoorInteractionComponent::OnEndOverlap);
}


// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (DoorOpeningDirection != 0)
	{
		CurrentRotationTime += DeltaTime * DoorOpeningDirection;
		if (CurrentRotationTime > TimeToRotate)
		{
			DoorOpeningDirection = 0;
			CurrentRotationTime = TimeToRotate;
		}
		else if (CurrentRotationTime < 0)
		{
			DoorOpeningDirection = 0;
			CurrentRotationTime = 0;
		}
		else
		{
			const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
			const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
			const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
			GetOwner()->SetActorRotation(CurrentRotation);
		}
	}
	//if (CurrentRotationTime < TimeToRotate)
	//{
		//if (TriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
		//{
		//	APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		//	if (PlayerPawn && TriggerBox->IsOverlappingActor(PlayerPawn))
		//	{
		//		//CurrentRotationTime = FMath::Clamp(CurrentRotationTime, 0.0f, TimeToRotate);
		//		if(CurrentRotationTime < TimeToRotate)
		//		{
		//			CurrentRotationTime += DeltaTime;
		//			const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		//			const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		//			const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
		//			GetOwner()->SetActorRotation(CurrentRotation);
		//		}
		//	}
		//	else if (GetOwner()->GetActorRotation() != StartRotation)
		//	{
		//		if (CurrentRotationTime > 0)
		//		{
		//			CurrentRotationTime -= DeltaTime;
		//			const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		//			const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		//			const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
		//			GetOwner()->SetActorRotation(CurrentRotation);
		//		}
		//	}
		//}
	//}
}


void UDoorInteractionComponent::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	DoorOpeningDirection = 1;
}

void UDoorInteractionComponent::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	DoorOpeningDirection = -1;
}