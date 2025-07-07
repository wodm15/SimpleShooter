// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h"


AGun::AGun()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Root);
}


// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGun::PullTrigger()
{
    UGameplayStatics::SpawnEmitterAttached(MuzzleFlash , Mesh , TEXT("MuzzleFlashSocket"));

    
    FHitResult Hit;
    FVector ShotDirection;
    bool bScuccess = GunTrace(Hit,ShotDirection);
    if(bScuccess)
    {
        // DrawDebugPoint(GetWorld() , Hit.Location , 20 , FColor::Red , true );
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(), 
            ImpactEffect, 
            Hit.ImpactPoint, 
            ShotDirection.Rotation()
        );
        
        AActor* HitActor = Hit.GetActor();
        if(HitActor != nullptr)
        {
            FPointDamageEvent DamageEvent(Damage, Hit , ShotDirection , nullptr ); 
            AController* OwnerController = GetOwnerController();
            HitActor->TakeDamage(Damage, DamageEvent, OwnerController , this);
        }

    }
}


bool AGun::GunTrace(FHitResult &Hit, FVector& ShotDirection)
{
    AController* OwnerController = GetOwnerController();
    if (OwnerController == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Gun OwnerController is nullptr"));
        return false;
    }

    FVector Location; //총 위치
    FRotator Rotation; 

    OwnerController->GetPlayerViewPoint(Location , Rotation);
    ShotDirection = - Rotation.Vector(); //총알이 날아온 방향


    FVector end = Location + (Rotation.Vector() * MaxRange);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());
    return GetWorld()->LineTraceSingleByChannel(Hit, Location , end , ECollisionChannel::ECC_GameTraceChannel1, Params);
}


AController* AGun::GetOwnerController() const
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Gun OwnerPawn is nullptr"));
        return nullptr;
    }

    return OwnerPawn->GetController();
}
