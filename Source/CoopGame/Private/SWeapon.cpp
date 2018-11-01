// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet//GameplayStatics.h"
#include "Particles/Particlesystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzelSocketName = "MuzzleFlashSocket";
	TargetTracerName = "Target";

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
	// Trace the world, from pawn eyes to crosshair location

	AActor* MyOnwer = GetOwner();
	if (MyOnwer)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOnwer->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParms;
		QueryParms.AddIgnoredActor(MyOnwer);
		QueryParms.AddIgnoredActor(this);
		QueryParms.bTraceComplex = true;

		//Particle  "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if ( GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParms))
		{
			// Blocking hit! Process damage

			AActor* HitActor = Hit.GetActor();


			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOnwer->GetInstigatorController(), this, DamageType );

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzelSocketName);
		}

		
		if (TracerEffect)
		{
			FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzelSocketName);

			UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
			if (TracerComp)
			{
				TracerComp->SetVectorParameter(TargetTracerName, TracerEndPoint);
			}
		}
		
		
	}

	
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

