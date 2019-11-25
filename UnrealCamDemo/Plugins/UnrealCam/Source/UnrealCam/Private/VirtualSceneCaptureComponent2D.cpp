// Fill out your copyright notice in the Description page of Project Settings.


#include "VirtualSceneCaptureComponent2D.h"
#include "UnrealCamDll.h"
#include "Engine/Classes/Kismet/KismetRenderingLibrary.h"

UVirtualSceneCaptureComponent2D::UVirtualSceneCaptureComponent2D()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UVirtualSceneCaptureComponent2D::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(TextureTarget)
	{
		FTextureRenderTarget2DResource* textureResource = (FTextureRenderTarget2DResource*)TextureTarget->Resource;
		TArray<FColor> ColorBuffer;
		if (textureResource->ReadPixels(ColorBuffer))
		{
			int size = TextureTarget->SizeX * TextureTarget->SizeY;
			UCHAR* data = new UCHAR[size * 3];
			for (int32 i = 0; i < size; i++)
			{
				int32 dataIndex = size * 3 - 1 - i * 3;
				data[dataIndex + 2] = ColorBuffer[i].R;
				data[dataIndex + 1] = ColorBuffer[i].G;
				data[dataIndex] = ColorBuffer[i].B;
			}
			UnrealCamDLL::SendTextureFromDLL(data, TextureTarget->SizeX, TextureTarget->SizeY);
		}
	}
}

