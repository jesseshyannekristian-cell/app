#include "Faction.h"

EAttitude UFactionLibrary::GetAttitude(EFaction A, EFaction B)
{
	if (A == B)
	{
		return EAttitude::Friendly;
	}

	// Foundation and its task forces stand together.
	const bool bAIsFoundation = (A == EFaction::Foundation || A == EFaction::MTF);
	const bool bBIsFoundation = (B == EFaction::Foundation || B == EFaction::MTF);
	if (bAIsFoundation && bBIsFoundation)
	{
		return EAttitude::Friendly;
	}

	// Anomalies are hostile to every human faction (and vice versa).
	if (A == EFaction::Anomaly || B == EFaction::Anomaly)
	{
		const EFaction Other = (A == EFaction::Anomaly) ? B : A;
		return (Other == EFaction::Wondertainment) ? EAttitude::Neutral : EAttitude::Hostile;
	}

	auto HostileTo = [](EFaction Self, EFaction Target) -> bool
	{
		switch (Self)
		{
		case EFaction::Foundation:
		case EFaction::MTF:
			return Target == EFaction::ChaosInsurgency;
		case EFaction::ChaosInsurgency:
			return Target == EFaction::Foundation || Target == EFaction::MTF || Target == EFaction::UIU;
		case EFaction::UIU:
			return Target == EFaction::ChaosInsurgency;
		default:
			return false;
		}
	};

	if (HostileTo(A, B) || HostileTo(B, A))
	{
		return EAttitude::Hostile;
	}

	return EAttitude::Neutral;
}

FLinearColor UFactionLibrary::GetFactionColor(EFaction Faction)
{
	switch (Faction)
	{
	case EFaction::Foundation:		return FLinearColor(0.85f, 0.85f, 0.9f);
	case EFaction::MTF:				return FLinearColor(0.1f, 0.35f, 0.9f);
	case EFaction::ChaosInsurgency:	return FLinearColor(0.5f, 0.04f, 0.04f);
	case EFaction::UIU:				return FLinearColor(0.05f, 0.55f, 0.45f);
	case EFaction::Wondertainment:	return FLinearColor(0.95f, 0.2f, 0.75f);
	case EFaction::Anomaly:			return FLinearColor(0.9f, 0.1f, 0.05f);
	case EFaction::Civilian:		return FLinearColor(0.9f, 0.8f, 0.2f);
	default:						return FLinearColor::White;
	}
}

FString UFactionLibrary::GetFactionName(EFaction Faction)
{
	switch (Faction)
	{
	case EFaction::Foundation:		return TEXT("SCP Foundation");
	case EFaction::MTF:				return TEXT("MTF XI-87 'Binge Watchers'");
	case EFaction::ChaosInsurgency:	return TEXT("Chaos Insurgency");
	case EFaction::UIU:				return TEXT("Unusual Incidents Unit");
	case EFaction::Wondertainment:	return TEXT("Dr. Wondertainment");
	case EFaction::Anomaly:			return TEXT("Anomalous Entity");
	case EFaction::Civilian:		return TEXT("Civilian");
	default:						return TEXT("Unknown");
	}
}
