// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GamePlayStatics.h"
#include "DrawDebugHelpers.h"

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

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Gun OwnerPawn is nullptr"));
        return;
    }

    AController* OwnerPawnController = OwnerPawn->GetController();
    if (OwnerPawnController == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Gun OwnerPawnController is nullptr"));
        return;
    }

    FVector Location; //총 위치
    FRotator Rotation; 

    OwnerPawnController->GetPlayerViewPoint(Location , Rotation);

    FVector end = Location + (Rotation.Vector() * MaxRange);

    //DrawDebugPoint(GetWorld() , Location, 20 , FColor::Red , true );
    FHitResult Hit;
    bool bScuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location , end , ECollisionChannel::ECC_GameTraceChannel1);
    if(bScuccess)
    {
        FVector ShotDirection = - Rotation.Vector(); //총알이 날아온 방향
        // DrawDebugPoint(GetWorld() , Hit.Location , 20 , FColor::Red , true );
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(), 
            ImpactEffect, 
            Hit.ImpactPoint, 
            ShotDirection.Rotation()
        );

    }
}
