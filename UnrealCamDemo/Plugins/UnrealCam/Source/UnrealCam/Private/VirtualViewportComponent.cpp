// Fill out your copyright notice in the Description page of Project Settings.

#include "VirtualViewportComponent.h"
#include "UnrealCamDLL.h"

UVirtualViewportComponent::UVirtualViewportComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVirtualViewportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GEngine)
	{
		if (UGameViewportClient* gameViewport = GEngine->GameViewport)
		{
			if (FViewport* inViewport = gameViewport->Viewport)
			{
				FIntRect Rect(0, 0, inViewport->GetSizeXY().X, inViewport->GetSizeXY().Y);
				TArray<FColor> ColorBuffer;
				if (inViewport->ReadPixels(ColorBuffer, FReadSurfaceDataFlags(), Rect))
				{
					int size = inViewport->GetSizeXY().X * inViewport->GetSizeXY().Y;
					UCHAR* data = new UCHAR[size * 3];
					for (int32 i = 0; i < size; i++)
					{
						int32 dataIndex = size * 3 - 1 - i * 3;
						data[dataIndex + 2] = ColorBuffer[i].R;
						data[dataIndex + 1] = ColorBuffer[i].G;
						data[dataIndex] = ColorBuffer[i].B;
					}
					UnrealCamDLL::SendTextureFromDLL(data, inViewport->GetSizeXY().X, inViewport->GetSizeXY().Y);
				}
			}
		}
	}
}