// Created by Paul Baudy

#pragma once

#include "ASBoneData.generated.h"

class UASBoneConstraint;

/** Helper struct used to pass on data to the solver */
USTRUCT(BlueprintType, Blueprintable)
struct FASBoneData
{
	GENERATED_BODY()

public:
	/** Initial bone transform before the solver deforms it */
	UPROPERTY(BlueprintReadOnly)
	FTransform BoneTransform;

	/** Bone Length with its parent */
	UPROPERTY(BlueprintReadOnly)
	float Length = 0.f;

	/** Cached constraint */
	TWeakObjectPtr<const UASBoneConstraint> Constraint = nullptr;
};