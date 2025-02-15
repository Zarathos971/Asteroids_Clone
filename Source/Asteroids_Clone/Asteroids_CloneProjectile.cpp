// Copyright 1998-2019 Epic Games, Inc. All Rights Reserve

#include "Asteroids_CloneProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Classes/Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"

AAsteroids_CloneProjectile::AAsteroids_CloneProjectile() 
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Asteroids_Clone/Meshes/ShipProjectile.ShipProjectile"));
			
	static ConstructorHelpers::FObjectFinder<USoundCue> iTeleport(TEXT("/Game/Audio/Explosion_Cue"));
	MySoundCue = iTeleport.Object;


	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AAsteroids_CloneProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

}

void AAsteroids_CloneProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) /*&& (OtherComp != NULL) && OtherComp->IsSimulatingPhysics()*/)
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		//OtherComp->DestroyComponent();
		
		AAsteroids_ClonePawn* MyPawn = Cast<AAsteroids_ClonePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		MyPawn->Score += 10;

		AMyAsteroid* MyAsteroid = Cast<AMyAsteroid>(OtherActor);

		if (MyAsteroid->Level_Asteroid == 1)
		{
			MyAsteroid->Destroy();
			UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, MySoundCue, MyAsteroid->GetTransform().GetLocation(), FRotator::ZeroRotator, 0.75f, 1.0f, 0.0f, nullptr, nullptr, true);
		}
		else
		{
			MyAsteroid->Level_Asteroid -= 1;
			UAudioComponent* AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, MySoundCue, MyAsteroid->GetTransform().GetLocation(), FRotator::ZeroRotator, 0.75f, 1.0f, 0.0f, nullptr, nullptr, true);
			MyAsteroid->SpawnAgain(MyAsteroid->Level_Asteroid, MyAsteroid->GetActorLocation());
			MyAsteroid->Destroy();
		}
	}

	Destroy();
}
