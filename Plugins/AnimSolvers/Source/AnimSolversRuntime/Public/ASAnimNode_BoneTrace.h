// Created by Paul Baudy

#pragma once

/// UE4
#include "CoreMinimal.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"

#include "ASAnimNode_BoneTrace.generated.h"


USTRUCT(BlueprintInternalUseOnly)
struct ANIMSOLVERSRUNTIME_API FAnimNode_BoneTrace : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bDrawDebug = false;
};