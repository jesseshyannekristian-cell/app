#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FoundationTypes.h"
#include "ArchiveSubsystem.generated.h"

/**
 * Simple JSON-backed database for player-authored Archives & Dossiers.
 * Loads from <ProjectSaved>/Archives/Archives.json on init and writes back on every change,
 * so custom SCP reports persist between sessions.
 */
UCLASS()
class FOUNDATION_API UArchiveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	const TArray<FArchiveEntry>& GetEntries() const { return Db.Entries; }
	void AddEntry(const FArchiveEntry& Entry);
	bool RemoveEntry(int32 Index);

private:
	FArchiveDatabaseData Db;

	void SaveToDisk();
	void LoadFromDisk();
	static FString FilePath();
};
