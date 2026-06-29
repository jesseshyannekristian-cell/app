#include "ArchiveSubsystem.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

void UArchiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadFromDisk();
}

FString UArchiveSubsystem::FilePath()
{
	return FPaths::ProjectSavedDir() / TEXT("Archives/Archives.json");
}

void UArchiveSubsystem::LoadFromDisk()
{
	FString Json;
	if (FFileHelper::LoadFileToString(Json, *FilePath()))
	{
		FArchiveDatabaseData Loaded;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(Json, &Loaded, 0, 0))
		{
			Db = Loaded;
		}
	}
}

void UArchiveSubsystem::SaveToDisk()
{
	const FString Path = FilePath();
	IFileManager::Get().MakeDirectory(*FPaths::GetPath(Path), true);

	FString Json;
	if (FJsonObjectConverter::UStructToJsonObjectString(Db, Json))
	{
		FFileHelper::SaveStringToFile(Json, *Path);
	}
}

void UArchiveSubsystem::AddEntry(const FArchiveEntry& Entry)
{
	Db.Entries.Add(Entry);
	SaveToDisk();
}

bool UArchiveSubsystem::RemoveEntry(int32 Index)
{
	if (Db.Entries.IsValidIndex(Index))
	{
		Db.Entries.RemoveAt(Index);
		SaveToDisk();
		return true;
	}
	return false;
}
