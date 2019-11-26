// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VirtualViewportComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALCAM_API UVirtualViewportComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVirtualViewportComponent();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
                                                                                                                                                                                                                                                                                                                                                                                                                                   