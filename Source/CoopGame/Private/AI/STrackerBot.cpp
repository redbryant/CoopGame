// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/STrackerBot.h"
#include "Components/StaticMeshComponent.h"
//#include "..\..\Public\AI\STrackerBot.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "NavigationSystem/Public/NavigationPath.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	bUseVelocityChange = false;
	MovementForce = 1000.0f;
	RequiredDistanceToTarget = 100;

}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// Find
	NextPathPoint = GetNextPathPoint();
	
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn =  UGameplayStatics::GetPlayerCharacter(this, 0);
	AActor* actor = Cast<AActor>(PlayerPawn);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), actor);

	if (NavPath->PathPoints.Num() > 1)
	{
		// Return next point
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if(DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();		

		DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached");
	}
	else
	{
		FVector FroceDirection = NextPathPoint - GetActorLocation();
		FroceDirection.Normalize();

		FroceDirection *= MovementForce;

		MeshComp->AddForce(FroceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + FroceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 4.0F, 1.0f);
}

// Called to bind functionality to input
void ASTrackerBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

