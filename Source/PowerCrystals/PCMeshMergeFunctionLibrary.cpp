// Copyright 2019-2020 Alberto & co. All Rights Reserved.


#include "PCMeshMergeFunctionLibrary.h"
#include "SkeletalMeshMerge.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "PowerCrystals.h"

static void ToMergeParams(const TArray<FPCSkelMeshMergeSectionMapping_BP>& InSectionMappings, TArray<FSkelMeshMergeSectionMapping>& OutSectionMappings)
{
	if (InSectionMappings.Num() > 0)
	{
		OutSectionMappings.AddUninitialized(InSectionMappings.Num());
		for (int32 i = 0; i < InSectionMappings.Num(); ++i)
		{
			OutSectionMappings[i].SectionIDs = InSectionMappings[i].SectionIDs;
		}
	}
};

static void ToMergeParams(const TArray<FPCSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh, TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh)
{
	if (InUVTransformsPerMesh.Num() > 0)
	{
		OutUVTransformsPerMesh.Empty();
		OutUVTransformsPerMesh.AddUninitialized(InUVTransformsPerMesh.Num());
		for (int32 i = 0; i < InUVTransformsPerMesh.Num(); ++i)
		{
			TArray<TArray<FTransform>>& OutUVTransforms = OutUVTransformsPerMesh[i].UVTransformsPerMesh;
			const TArray<FPCSkelMeshMergeUVTransform>& InUVTransforms = InUVTransformsPerMesh[i].UVTransformsPerMesh;
			if (InUVTransforms.Num() > 0)
			{
				OutUVTransforms.Empty();
				OutUVTransforms.AddUninitialized(InUVTransforms.Num());
				for (int32 j = 0; j < InUVTransforms.Num(); j++)
				{
					OutUVTransforms[i] = InUVTransforms[i].UVTransforms;
				}
			}
		}
	}
};

USkeletalMesh* UPCMeshMergeFunctionLibrary::MergeMeshes(const FPCSkeletalMeshMergeParams& Params)
{
	TArray<USkeletalMesh*> MeshesToMergeCopy = Params.MeshesToMerge;
	MeshesToMergeCopy.RemoveAll([](USkeletalMesh* InMesh)
		{
			return InMesh == nullptr;
		});
	if (MeshesToMergeCopy.Num() <= 1)
	{
		UE_LOG(LogPowerCrystals, Warning, TEXT("Must provide multiple valid Skeletal Meshes in order to perform a merge."));
		return nullptr;
	}

	EMeshBufferAccess BufferAccess = Params.bNeedsCpuAccess ? EMeshBufferAccess::ForceCPUAndGPU : EMeshBufferAccess::Default;
	TArray<FSkelMeshMergeSectionMapping> SectionMappings;
	TArray<FSkelMeshMergeUVTransforms> UvTransforms;
	ToMergeParams(Params.MeshSectionMappings, SectionMappings);
	ToMergeParams(Params.UVTransformsPerMesh, UvTransforms);
	bool bRunDuplicateCheck = false;
	USkeletalMesh* BaseMesh = NewObject<USkeletalMesh>();

	if (Params.Skeleton && Params.bSkeletonBefore)
	{
		BaseMesh->SetSkeleton(Params.Skeleton);
		bRunDuplicateCheck = true;
		for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
		{
			if (Socket)
			{
				UE_LOG(LogPowerCrystals, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
			}
		}
		for (USkeletalMeshSocket* Socket : BaseMesh->GetSkeleton()->Sockets)
		{
			if (Socket)
			{
				UE_LOG(LogPowerCrystals, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
			}
		}
	}

	FSkeletalMeshMerge Merger(BaseMesh, MeshesToMergeCopy, SectionMappings, Params.StripTopLODS, BufferAccess, UvTransforms.GetData());
	if (!Merger.DoMerge())
	{
		UE_LOG(LogPowerCrystals, Warning, TEXT("Merge failed!"));
		return nullptr;
	}

	if (Params.Skeleton && !Params.bSkeletonBefore)
	{
		BaseMesh->SetSkeleton(Params.Skeleton);
	}

	if (Params.PhysicsAsset)
	{
		BaseMesh->SetPhysicsAsset(Params.PhysicsAsset);
	}

	if (bRunDuplicateCheck)
	{
		TArray<FName> SkelMeshSockets;
		TArray<FName> SkelSockets;
		for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
		{
			if (Socket)
			{
				SkelMeshSockets.Add(Socket->GetFName());
				UE_LOG(LogPowerCrystals, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
			}
		}

		for (USkeletalMeshSocket* Socket : BaseMesh->GetSkeleton()->Sockets)
		{
			if (Socket)
			{
				SkelSockets.Add(Socket->GetFName());
				UE_LOG(LogPowerCrystals, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
			}
		}

		TSet<FName> UniqueSkelMeshSockets;
		TSet<FName> UniqueSkelSockets;
		UniqueSkelMeshSockets.Append(SkelMeshSockets);
		UniqueSkelSockets.Append(SkelSockets);
		int32 Total = SkelSockets.Num() + SkelMeshSockets.Num();
		int32 UniqueTotal = UniqueSkelMeshSockets.Num() + UniqueSkelSockets.Num();

		UE_LOG(LogPowerCrystals, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), SkelMeshSockets.Num(), SkelSockets.Num(), Total);
		UE_LOG(LogPowerCrystals, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), UniqueSkelMeshSockets.Num(), UniqueSkelSockets.Num(), UniqueTotal);
		UE_LOG(LogPowerCrystals, Warning, TEXT("Found Duplicates: %s"), *((Total != UniqueTotal) ? FString("True") : FString("False")));
	}

	return BaseMesh;
}
