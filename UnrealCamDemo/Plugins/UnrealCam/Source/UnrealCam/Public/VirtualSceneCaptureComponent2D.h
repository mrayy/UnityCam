// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "VirtualSceneCaptureComponent2D.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALCAM_API UVirtualSceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()
	
public:
	UVirtualSceneCaptureComponent2D();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
