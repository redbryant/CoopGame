// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/STrackerBot.h"
#include "Components/StaticMeshComponent.h"
//#include "..\..\Public\AI\STrackerBot.h"
#include "SHealthComponent.h"
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

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnHealthChanged);

	bUseVelocityChange = false;
	MovementForce = 1000.0f;
	RequiredDistanceToTarget = 100;

	ExplosionDamage = 40;
	ExplosionRadius = 200;

}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// Find
	NextPathPoint = GetNextPathPoint();
	
}

void ASTrackerBot::OnHealthChanged(USHealthComponent* HealthComp, float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on hitpoints == 0

	// @TODO: pULSE THE MATERIAL ON HIT

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
		
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	UE_LOG(LogTemp, Log, TEXT("hEALTH %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	if (Health <= 0.0f)
		SelfDestruct();
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

void ASTrackerBot::SelfDestruct()
{
	if (bExplode)
		return;

	bExplode = true;

	UGameplayStatics::InternalSpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	TArray<AActor> IgnoreaActors;
	IgnoreaActors.Add(this);

	//Apply Damage!
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoreaActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

	// dELETE ACTOR
	Destroy();
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
//
//void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ASCharacter, CurrentWeapon);
//	DOREPLIFETIME(ASCharacter, bDied);
//}