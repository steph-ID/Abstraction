// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"
#include "ObjectiveWorldSubsystem.h"
#include "ObjectiveComponent.h"

#include "DrawDebugHelpers.h"

constexpr float FLT_METERS(float meters) { return meters * 100.0f; }

static TAutoConsoleVariable<bool> CVarToggleDebugDoor(
	TEXT("Abstraction.DoorInteractionComponent.Debug"),
	false,
	TEXT("Toggle DoorInteractionComponent debug display."),
	ECVF_Default);

// Sets default values for this component's properties
UDoorInteractionComponent::UDoorInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	DoorState = EDoorState::DS_Closed;

	CVarToggleDebugDoor.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UDoorInteractionComponent::OnDebugToggled));
}


// Called when the game starts
void UDoorInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	StartRotation = GetOwner()->GetActorRotation();
	FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;
	// Ensure TimeToRotate is greater than EPSILON
	CurrentRotationTime = 0.0f;
	/*UObjectiveWorldSubsystem* ObjectiveWorldSubsystem = GetWorld()->GetSubsystem<UObjectiveWorldSubsystem>();
	if (ObjectiveWorldSubsystem)
	{
		// How does this work? Is it always called on OpenedEvent?
		OpenedEvent.AddUObject(ObjectiveWorldSubsystem, &UObjectiveWorldSubsystem::OnObjectiveCompleted);
	}*/

	TriggerBox->OnActorBeginOverlap.AddDynamic(this, &UDoorInteractionComponent::OnBeginOverlap);
	TriggerBox->OnActorEndOverlap.AddDynamic(this, &UDoorInteractionComponent::OnEndOverlap);
}


// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (DoorOpeningDirection != 0)
	{	
		if (DoorOpeningDirection > 0)
			DoorState = EDoorState::DS_Opening;
		else if(DoorOpeningDirection < 0)
			DoorState = EDoorState::DS_Closing;

		CurrentRotationTime += DeltaTime * DoorOpeningDirection;
		if (CurrentRotationTime > TimeToRotate)
		{
			DoorOpeningDirection = 0;
			CurrentRotationTime = TimeToRotate;
			OnDoorOpen();
			/*DoorState = EDoorState::DS_Open;
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("DoorOpened"));
			OpenedEvent.Broadcast();*/
		}
		else if (CurrentRotationTime < 0)
		{
			DoorOpeningDirection = 0;
			CurrentRotationTime = 0;
			DoorState = EDoorState::DS_Closed;
		}
		else
		{
			const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
			const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
			const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
			GetOwner()->SetActorRotation(CurrentRotation);
		}
		//if (DoorOpeningDirection > 0) 
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
	//if (TimeRatio >= 1.0f)
	//{
	//	OnDoorOpen();
	//}
		//	}
		//}
	//}
	DebugDraw();
}


void UDoorInteractionComponent::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	DoorOpeningDirection = 1;
}

void UDoorInteractionComponent::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	DoorOpeningDirection = -1;
}

void UDoorInteractionComponent::OnDoorOpen()
{
	DoorState = EDoorState::DS_Open;
	UObjectiveComponent* ObjectiveComponent = GetOwner()->FindComponentByClass<UObjectiveComponent>();
	if (ObjectiveComponent)
	{
		ObjectiveComponent->SetState(EObjectiveState::OS_Completed);
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("DoorOpened"));
}

void UDoorInteractionComponent::OnDebugToggled(IConsoleVariable* Var)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDebugToggled"));
}

void UDoorInteractionComponent::DebugDraw()
{
	if (CVarToggleDebugDoor->GetBool())
	{
		FVector Offset(FLT_METERS(-0.75f), 0.0f, FLT_METERS(2.5f));
		FVector StartLocation = GetOwner()->GetActorLocation() + Offset;
		FString EnumAsString = TEXT("Door State: ") + UEnum::GetDisplayValueAsText(DoorState).ToString();
		DrawDebugString(GetWorld(), Offset, EnumAsString, GetOwner(), FColor::Blue, 0.0f);
	}
}
