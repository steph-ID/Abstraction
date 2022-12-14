// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveWorldSubsystem.h"
#include "ObjectiveComponent.h"
#include "Kismet/GameplayStatics.h"

void UObjectiveWorldSubsystem::CreateObjectiveWidget(TSubclassOf <UUserWidget> ObjectiveWidgetClass)
{
	if (ObjectiveWidget == nullptr)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ObjectiveWidget = CreateWidget<UUserWidget>(PlayerController, ObjectiveWidgetClass);
	}
}

void UObjectiveWorldSubsystem::DisplayObjectiveWidget()
{
	ensureMsgf(ObjectiveWidget, TEXT("UObjectiveWorldSubsystem::DisplayObjectiveWidget ObjectiveWidget is nullptr"));
	ObjectiveWidget->AddToViewport();
}

FString UObjectiveWorldSubsystem::GetCurrentObjectiveDescription()
{
	if (!Objectives.IsValidIndex(0) || Objectives[0]->GetState() == EObjectiveState::OS_Inactive)
	{
		return TEXT("N/A");
	}

	// Display Objective(s)
	FString RetObjective = Objectives[0]->GetDescription();
	if (Objectives[0]->GetState() == EObjectiveState::OS_Completed)
	{
		RetObjective += TEXT(" Completed!");
	}

	return RetObjective;
}

void UObjectiveWorldSubsystem::OnObjectiveCompleted()
{
	DisplayObjectiveWidget();
}

void UObjectiveWorldSubsystem::AddObjective(UObjectiveComponent* ObjectiveComponent)
{
	check(ObjectiveComponent);
	size_t PrevSize = Objectives.Num();
	// Make sure there are no duplicate components
	Objectives.AddUnique(ObjectiveComponent);

	// Check is something was added to the array
	if (Objectives.Num() > PrevSize)
	{
		ObjectiveComponent->OnStateChanged().AddUObject(this, &UObjectiveWorldSubsystem::OnObjectiveStateChanged);
	}
}

void UObjectiveWorldSubsystem::RemoveObjective(UObjectiveComponent* ObjectiveComponent)
{
	Objectives.Remove(ObjectiveComponent);
}

void UObjectiveWorldSubsystem::OnObjectiveStateChanged(UObjectiveComponent* ObjectiveComponent, EObjectiveState ObjectiveState)
{
	DisplayObjectiveWidget();
}