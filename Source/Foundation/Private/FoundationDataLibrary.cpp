#include "FoundationDataLibrary.h"

FString UFoundationDataLibrary::SCPClassToString(ESCPClass Class)
{
	switch (Class)
	{
	case ESCPClass::Safe:        return TEXT("Safe");
	case ESCPClass::Euclid:      return TEXT("Euclid");
	case ESCPClass::Keter:       return TEXT("Keter");
	case ESCPClass::Thaumiel:    return TEXT("Thaumiel");
	case ESCPClass::Apollyon:    return TEXT("Apollyon");
	case ESCPClass::Neutralized: return TEXT("Neutralized");
	}
	return TEXT("Euclid");
}

const TArray<FSCPData>& UFoundationDataLibrary::GetSCPs()
{
	static TArray<FSCPData> Data;
	if (Data.Num() == 0)
	{
		auto Add = [](const FString& Num, const FString& Name, ESCPClass C, int32 Threat, const FString& Proc)
		{
			FSCPData S; S.Number = Num; S.Name = Name; S.Class = C; S.ThreatLevel = Threat;
			S.ContainmentProcedure = Proc; S.bBreachable = true; Data.Add(S);
		};
		Add(TEXT("SCP-173"), TEXT("The Sculpture"), ESCPClass::Euclid, 3, TEXT("Kept in a locked container; personnel must maintain unbroken line of sight and never blink simultaneously."));
		Add(TEXT("SCP-096"), TEXT("The Shy Guy"), ESCPClass::Euclid, 4, TEXT("Contained in a sealed cell with no viewing apparatus; no images of its face may exist on-site."));
		Add(TEXT("SCP-106"), TEXT("The Old Man"), ESCPClass::Keter, 4, TEXT("Contained within a sealed lead-lined cell; recall protocols use a femur-bone lure and EM fields."));
		Add(TEXT("SCP-682"), TEXT("Hard-to-Destroy Reptile"), ESCPClass::Keter, 5, TEXT("Submerged in hydrochloric acid; if it adapts, neutralisation by all available means is authorised."));
		Add(TEXT("SCP-049"), TEXT("The Plague Doctor"), ESCPClass::Euclid, 3, TEXT("Standard humanoid cell; physical contact prohibited due to lethal 'cure' procedure."));
		Add(TEXT("SCP-035"), TEXT("Possessive Mask"), ESCPClass::Keter, 4, TEXT("Sealed airtight container; corrosive secretions drained continuously, no skin contact."));
		Add(TEXT("SCP-939"), TEXT("With Many Voices"), ESCPClass::Keter, 4, TEXT("Cold-environment habitat (<-26C reduces activity); fed live prey, handled in pairs only."));
		Add(TEXT("SCP-966"), TEXT("Sleep Killer"), ESCPClass::Keter, 3, TEXT("Contained in a climate-controlled enclosure; personnel use thermal/IR optics to observe."));
		Add(TEXT("SCP-999"), TEXT("The Tickle Monster"), ESCPClass::Safe, 1, TEXT("Open habitat; freely roams designated wing, beneficial morale effects, no restraint needed."));
		Add(TEXT("SCP-053"), TEXT("Young Girl"), ESCPClass::Euclid, 2, TEXT("Comfortable furnished room; only D-class over 60 may interact, no living thing within 10m."));
		Add(TEXT("SCP-079"), TEXT("Old AI"), ESCPClass::Euclid, 2, TEXT("Power supply isolated; no network connection, no external storage may be attached."));
		Add(TEXT("SCP-087"), TEXT("The Stairwell"), ESCPClass::Euclid, 3, TEXT("Access door welded shut; exploration suspended, no light source exceeds limits inside."));
		Add(TEXT("SCP-457"), TEXT("Burning Man"), ESCPClass::Euclid, 3, TEXT("Fire-suppression vault; deprived of oxygen and combustible material to limit size."));
		Add(TEXT("SCP-1048"), TEXT("Builder Bear"), ESCPClass::Keter, 3, TEXT("Steel cell under 24h surveillance; constructs hostile copies, all materials removed."));
		Add(TEXT("SCP-1471"), TEXT("MalO ver1.0.0"), ESCPClass::Euclid, 2, TEXT("App isolated on a Faraday-caged device; affected personnel monitored for apparitions."));
		Add(TEXT("SCP-2521"), TEXT("...Taken by Shadow People"), ESCPClass::Euclid, 3, TEXT("All documentation pictographic only; never named or described in spoken/written word."));
		Add(TEXT("SCP-3008"), TEXT("Infinite IKEA"), ESCPClass::Euclid, 3, TEXT("Storefront entrance sealed and guarded; no personnel enter the retail interior."));
		Add(TEXT("SCP-4666"), TEXT("The Yule Man"), ESCPClass::Keter, 4, TEXT("Largely uncontained; tracked via abduction patterns, mobile task forces intercept."));
		Add(TEXT("SCP-008"), TEXT("Zombie Plague"), ESCPClass::Euclid, 4, TEXT("Biohazard vault under multi-stage airlock; absolute pathogen isolation enforced."));
		Add(TEXT("SCP-914"), TEXT("The Clockworks"), ESCPClass::Safe, 1, TEXT("Sealed chamber; only approved inputs processed via Rough/Fine/Very Fine settings."));
		Add(TEXT("SCP-055"), TEXT("[unknown]"), ESCPClass::Keter, 3, TEXT("Antimemetic anomaly; containment documented only via indirect, self-keeping records."));
		Add(TEXT("SCP-16829"), TEXT("The TV in Time"), ESCPClass::Euclid, 4, TEXT("Anchored in a Scranton Reality Anchor field with temporal-loop stabilisation; the screen must not be viewed by unsupervised personnel (Dimension Chemosic egress / carcinogenic exposure risk)."));
	}
	return Data;
}

const TArray<FRogueMTFData>& UFoundationDataLibrary::GetRogueMTF()
{
	static TArray<FRogueMTFData> Data;
	if (Data.Num() == 0)
	{
		auto Add = [](const FString& Name, const FString& Origin, int32 Threat, const FString& Notes)
		{ FRogueMTFData R; R.Name = Name; R.Origin = Origin; R.ThreatLevel = Threat; R.Notes = Notes; Data.Add(R); };

		Add(TEXT("Chaos Insurgency Cell 'Black Queen'"), TEXT("Chaos Insurgency"), 4, TEXT("Defected Foundation splinter; steals anomalies for weaponisation."));
		Add(TEXT("Defected MTF Nine-Tailed Fox Squad"), TEXT("Rogue MTF Epsilon-11"), 5, TEXT("Elite operatives gone dark with breach-grade equipment."));
		Add(TEXT("Serpent's Hand Coven"), TEXT("Serpent's Hand"), 3, TEXT("Anomaly-sympathetic insurgents using the Library to infiltrate."));
		Add(TEXT("Rogue MTF Alpha-1 'Red Right Hand'"), TEXT("Rogue O5 Guard"), 5, TEXT("Compromised O5 protection detail; highest clearance threat."));
		Add(TEXT("GOC Strike Team (Hostile)"), TEXT("Global Occult Coalition"), 4, TEXT("Destruction-doctrine rivals seeking to neutralise contained assets."));
		Add(TEXT("Marshall, Carter & Dark Operatives"), TEXT("MC&D"), 3, TEXT("Anomalous black-market agents extracting items for auction."));
		Add(TEXT("Defected MTF Tau-5 'Samsara'"), TEXT("Rogue Cyborg MTF"), 4, TEXT("Augmented unit malfunctioning after anomalous exposure."));
		Add(TEXT("Insurgency Sleeper Agent"), TEXT("Chaos Insurgency"), 2, TEXT("Embedded site staff leaking containment schematics."));
	}
	return Data;
}

const TArray<FEquipmentData>& UFoundationDataLibrary::GetEquipment()
{
	static TArray<FEquipmentData> Data;
	if (Data.Num() == 0)
	{
		auto Add = [](const TCHAR* Id, const FString& Name, EEquipDept Dept, int32 Rank, int32 Cost, bool bResearch, const FString& Desc)
		{
			FEquipmentData E; E.Id = FName(Id); E.Name = Name; E.Dept = Dept; E.RequiredRank = Rank;
			E.PurchaseCost = Cost; E.bRequiresResearch = bResearch; E.Description = Desc; Data.Add(E);
		};

		// --- Field equipment (Containment / MTF gear) ---
		Add(TEXT("flashlight"), TEXT("Reinforced Flashlight"), EEquipDept::Field, 0, 50, false, TEXT("Standard-issue illumination. Available from day one."));
		Add(TEXT("keycard_l1"), TEXT("Level 1 Keycard"), EEquipDept::Field, 0, 75, false, TEXT("Opens low-clearance containment doors."));
		Add(TEXT("kevlar_vest"), TEXT("Tactical Kevlar Vest"), EEquipDept::Field, 1, 200, true, TEXT("Reduces ballistic damage from hostile factions."));
		Add(TEXT("epsilon_rifle"), TEXT("Epsilon-11 Rifle"), EEquipDept::Field, 2, 450, true, TEXT("MTF standard rifle effective against rogue operatives."));
		Add(TEXT("scranton_anchor"), TEXT("Scranton Reality Anchor"), EEquipDept::Field, 3, 1200, true, TEXT("Stabilises local reality; suppresses reality-bending anomalies."));
		Add(TEXT("micro_hid"), TEXT("Micro-H.I.D. Emitter"), EEquipDept::Field, 4, 2000, true, TEXT("High-intensity discharge weapon; lethal to durable Keter-class entities."));
		Add(TEXT("hazmat_suit"), TEXT("Hazmat Containment Suit"), EEquipDept::Field, 1, 300, true, TEXT("Protects against corrosive secretions and airborne hazards."));
		Add(TEXT("thermal_optics"), TEXT("Thermal/IR Optics"), EEquipDept::Field, 2, 380, true, TEXT("Reveals SCP-966 and other invisible thermal-signature entities."));
		Add(TEXT("femur_breaker"), TEXT("Femur-Bone Recall Lure"), EEquipDept::Field, 3, 900, true, TEXT("Recall device used to re-contain SCP-106."));

		// --- Research department equipment ---
		Add(TEXT("memetic_kit"), TEXT("Memetic Analysis Kit"), EEquipDept::Research, 1, 350, true, TEXT("Lab kit for studying cognitohazards safely."));
		Add(TEXT("anomaly_scanner"), TEXT("Anomalous Signature Scanner"), EEquipDept::Research, 2, 600, true, TEXT("Detects Hume fluctuations and classifies anomalies."));
		Add(TEXT("kant_counter"), TEXT("Kant Counter"), EEquipDept::Research, 3, 850, true, TEXT("Measures local reality strength (Humes) for breach prediction."));
		Add(TEXT("memetic_inoculant"), TEXT("Memetic Inoculant Serum"), EEquipDept::Research, 3, 1100, true, TEXT("Grants temporary resistance to memetic/cognitohazardous effects."));
		Add(TEXT("cryo_unit"), TEXT("Cryogenic Containment Unit"), EEquipDept::Research, 4, 1500, true, TEXT("Sub-zero habitat module required to contain SCP-939."));
		Add(TEXT("faraday_rig"), TEXT("Faraday Isolation Rig"), EEquipDept::Research, 2, 500, true, TEXT("Electromagnetically isolates digital anomalies (SCP-079/1471)."));
	}
	return Data;
}

FName UFoundationDataLibrary::ResearchIdForEquipment(FName EquipId)
{
	return FName(*FString::Printf(TEXT("res_%s"), *EquipId.ToString()));
}

const TArray<FResearchProject>& UFoundationDataLibrary::GetResearchProjects()
{
	static TArray<FResearchProject> Data;
	if (Data.Num() == 0)
	{
		// 1) Auto-generate an equipment research project for every level-locked item.
		for (const FEquipmentData& E : GetEquipment())
		{
			if (!E.bRequiresResearch) continue;
			FResearchProject P;
			P.Id = ResearchIdForEquipment(E.Id);
			P.Name = FString::Printf(TEXT("R&D: %s"), *E.Name);
			P.Category = EResearchCategory::Equipment;
			P.Objective = FString::Printf(TEXT("Complete prototype testing to authorise %s for site requisition."), *E.Name);
			P.Description = E.Description;
			P.RequiredRank = E.RequiredRank;
			P.ResearchCost = 100 + E.RequiredRank * 150 + (E.Dept == EEquipDept::Research ? 50 : 0);
			P.UnlocksEquipment = E.Id;
			Data.Add(P);
		}

		// 2) Containment-procedure research projects (lore-driven, no equipment unlock).
		auto AddProc = [](const TCHAR* Id, const FString& Name, const FString& Obj, int32 Rank, int32 Cost, const FString& Desc)
		{
			FResearchProject P; P.Id = FName(Id); P.Name = Name; P.Category = EResearchCategory::ContainmentProcedure;
			P.Objective = Obj; P.RequiredRank = Rank; P.ResearchCost = Cost; P.Description = Desc;
			P.UnlocksEquipment = NAME_None; Data.Add(P);
		};
		AddProc(TEXT("proc_173"), TEXT("Procedure: SCP-173 Observation Rota"), TEXT("Design a fail-safe line-of-sight rotation eliminating simultaneous blinking."), 1, 200, TEXT("Improves SCP-173 containment reliability."));
		AddProc(TEXT("proc_106"), TEXT("Procedure: SCP-106 Recall Protocol"), TEXT("Refine femur-breaker recall and EM-field re-containment sequence."), 3, 500, TEXT("Enables reliable SCP-106 re-containment."));
		AddProc(TEXT("proc_939"), TEXT("Procedure: SCP-939 Cold Habitat"), TEXT("Establish sub-zero feeding and handling protocol for SCP-939."), 4, 700, TEXT("Reduces SCP-939 activity and breach risk."));
		AddProc(TEXT("proc_096"), TEXT("Procedure: SCP-096 Visual Blackout"), TEXT("Implement site-wide image scrubbing to prevent face exposure."), 2, 400, TEXT("Prevents SCP-096 trigger events."));
		AddProc(TEXT("proc_682"), TEXT("Procedure: SCP-682 Acid Submersion"), TEXT("Optimise hydrochloric submersion and adaptation monitoring."), 5, 900, TEXT("Slows SCP-682 adaptation during containment."));
		AddProc(TEXT("proc_035"), TEXT("Procedure: SCP-035 Secretion Drainage"), TEXT("Automate corrosive secretion drainage and no-contact handling."), 4, 650, TEXT("Stabilises SCP-035 containment."));
		AddProc(TEXT("proc_16829"), TEXT("Procedure: SCP-16829 Temporal Loop Stabilization"), TEXT("Develop reality-anchored stabilisation preventing dimensional egress and temporal looping (Dimension Chemosic)."), 5, 900, TEXT("Stabilises SCP-16829 and shields personnel from temporal/carcinogenic exposure."));
	}
	return Data;
}

const TArray<FRankData>& UFoundationDataLibrary::GetRanks()
{
	static TArray<FRankData> Data;
	if (Data.Num() == 0)
	{
		auto Add = [](int32 Lvl, const FString& Title, int32 XP, int32 Cr, int32 RCr)
		{ FRankData R; R.Level = Lvl; R.Title = Title; R.XPRequired = XP; R.CreditReward = Cr; R.ResearchCreditReward = RCr; Data.Add(R); };
		Add(0, TEXT("Junior Researcher"),       0,     0,    0);
		Add(1, TEXT("Researcher"),              500,   300,  150);
		Add(2, TEXT("Senior Researcher"),       1500,  600,  300);
		Add(3, TEXT("Site Supervisor"),         3500,  1000, 500);
		Add(4, TEXT("Site Director"),           7000,  1800, 900);
		Add(5, TEXT("O5 Overseer (OMEGA BLACK)"), 12000, 3000, 1500);
	}
	return Data;
}

const TArray<FContainmentOp>& UFoundationDataLibrary::GetOperations()
{
	static TArray<FContainmentOp> Data;
	if (Data.Num() == 0)
	{
		// Containment ops for every SCP.
		for (const FSCPData& S : GetSCPs())
		{
			FContainmentOp O;
			O.Id = FName(*FString::Printf(TEXT("op_%s"), *S.Number.Replace(TEXT("-"), TEXT("_"))));
			O.Name = FString::Printf(TEXT("Contain %s"), *S.Number);
			O.TargetDesignation = FString::Printf(TEXT("%s — %s"), *S.Number, *S.Name);
			O.Description = S.ContainmentProcedure;
			O.RequiredRank = FMath::Clamp(S.ThreatLevel - 1, 0, 5);
			O.Difficulty = S.ThreatLevel;
			O.RewardCredits = 150 * S.ThreatLevel;
			O.RewardResearchCredits = 80 * S.ThreatLevel;
			O.RewardXP = 200 * S.ThreatLevel;

			// Tie ops to their containment-procedure research where one exists.
			if (S.Number == TEXT("SCP-173")) O.RequiredResearch = FName(TEXT("proc_173"));
			else if (S.Number == TEXT("SCP-106")) O.RequiredResearch = FName(TEXT("proc_106"));
			else if (S.Number == TEXT("SCP-939")) O.RequiredResearch = FName(TEXT("proc_939"));
			else if (S.Number == TEXT("SCP-096")) O.RequiredResearch = FName(TEXT("proc_096"));
			else if (S.Number == TEXT("SCP-682")) O.RequiredResearch = FName(TEXT("proc_682"));
			else if (S.Number == TEXT("SCP-035")) O.RequiredResearch = FName(TEXT("proc_035"));
			else if (S.Number == TEXT("SCP-16829")) O.RequiredResearch = FName(TEXT("proc_16829"));

			// High-threat anomalies require specialised researched equipment.
			if (S.ThreatLevel >= 5) O.RequiredEquipment = FName(TEXT("micro_hid"));
			else if (S.ThreatLevel == 4) O.RequiredEquipment = FName(TEXT("scranton_anchor"));

			Data.Add(O);
		}
		// Neutralisation ops for every rogue MTF / GOI threat.
		for (const FRogueMTFData& R : GetRogueMTF())
		{
			FContainmentOp O;
			O.Id = FName(*FString::Printf(TEXT("op_rogue_%s"), *FString::FromInt(Data.Num())));
			O.Name = FString::Printf(TEXT("Neutralise: %s"), *R.Name);
			O.TargetDesignation = FString::Printf(TEXT("%s [%s]"), *R.Name, *R.Origin);
			O.Description = R.Notes;
			O.RequiredRank = FMath::Clamp(R.ThreatLevel - 1, 0, 5);
			O.RequiredEquipment = FName(TEXT("epsilon_rifle"));
			O.Difficulty = R.ThreatLevel;
			O.RewardCredits = 180 * R.ThreatLevel;
			O.RewardResearchCredits = 60 * R.ThreatLevel;
			O.RewardXP = 220 * R.ThreatLevel;
			Data.Add(O);
		}
	}
	return Data;
}

const TArray<FPersonnelData>& UFoundationDataLibrary::GetPersonnel()
{
	static TArray<FPersonnelData> Data;
	if (Data.Num() == 0)
	{
		{
			FPersonnelData P;
			P.FileNo = TEXT("S20-PERS-05-GUNZ");
			P.Codename = TEXT("GUNZ");
			P.RealName = TEXT("(CLASSIFIED)");
			P.Position = TEXT("Member of the O5 Council");
			P.Clearance = TEXT("Level 5 / OMEGA BLACK");
			P.Status = TEXT("Active");
			P.Nationality = TEXT("Canadian (former JTF-2 Technical Specialist)");
			P.Affiliation = TEXT("Technological Development Department");
			P.Aliases = TEXT("Gunz, O5, The Technician, Semizuse");
			P.AnomalousTrait = TEXT("Bright blue glowing eyes resulting from repeated SCP-16829 exposure.");
			P.Bio = TEXT("During a breach event originating in Sector-[REDACTED], O5-0 Gunz coordinated real-time containment protocols, ordered the evacuation of non-essential personnel, and personally supervised the sealing of affected corridors. Post-incident analysis confirms his rapid decision-making and technical expertise directly reduced potential casualties.");
			P.O5Notes = TEXT("\"Gunz is suspected to use SCP-16829 for traveling between sites. Some personnel follow orders. Some hold the line when orders fail.\" — Site-20 Security Evaluation Board");
			Data.Add(P);
		}
		{
			FPersonnelData P;
			P.FileNo = TEXT("S20-PERS-00420-CHEMOSIC");
			P.Codename = TEXT("CHEMOSIC");
			P.RealName = TEXT("(REDACTED)");
			P.Position = TEXT("Site Supervisor — Site-20");
			P.Clearance = TEXT("Level 5 / O5 Council Access");
			P.Status = TEXT("Active — Limited Field Deployment");
			P.Nationality = TEXT("Canadian (former JTF-2 Assaulter / Sniper / Team Leader)");
			P.Affiliation = TEXT("Former Commander, MTF XI-87 \"Binge Watchers\"");
			P.Aliases = TEXT("Chemosic");
			P.AnomalousTrait = TEXT("In remission from cancer attributed to repeated exposure to SCP-16829 (\"Dimension Chemosic\"). Under ongoing Site-20 medical monitoring.");
			P.Bio = TEXT("Led 60+ successful operations involving high-risk SCPs, including multiple incursions into SCP-16829. Expert in information warfare, psychological operations and memetic-hazard countermeasures. Medically retired from field command and promoted to Site Supervisor: oversees all Site-20 operations, personnel and containment protocols.");
			P.O5Notes = TEXT("\"Chemosic is one of the few individuals we trust implicitly with both the protection of the Foundation and its darkest secrets. His experience, judgment and loyalty are unmatched. Site-20 is in capable hands.\" — O5-[REDACTED]");
			Data.Add(P);
		}
		{
			FPersonnelData P;
			P.FileNo = TEXT("S20-PERS-RD-LEAD");
			P.Codename = TEXT("HEAD RESEARCHER");
			P.RealName = TEXT("(REDACTED)");
			P.Position = TEXT("Head Researcher — Site-20 Research Division");
			P.Clearance = TEXT("Level 4");
			P.Status = TEXT("Active");
			P.Nationality = TEXT("(REDACTED)");
			P.Affiliation = TEXT("Site-20 Research Division");
			P.Aliases = TEXT("—");
			P.AnomalousTrait = TEXT("None on record.");
			P.Bio = TEXT("Directs Site-20's anomaly research portfolio, including active study files on SCP-173, SCP-096, SCP-049 and SCP-682. Authorises research projects and containment-procedure development that feed the Research Division and Containment Operations.");
			P.O5Notes = TEXT("\"Approves the science that keeps the cells closed. Pragmatic, meticulous, and unflinching under breach conditions.\" — Site-20 Records");
			Data.Add(P);
		}
	}
	return Data;
}

const FEquipmentData* UFoundationDataLibrary::FindEquipment(FName Id)
{
	for (const FEquipmentData& E : GetEquipment()) { if (E.Id == Id) return &E; }
	return nullptr;
}

const FResearchProject* UFoundationDataLibrary::FindResearch(FName Id)
{
	for (const FResearchProject& P : GetResearchProjects()) { if (P.Id == Id) return &P; }
	return nullptr;
}

const FRankData& UFoundationDataLibrary::GetRankData(int32 Level)
{
	const TArray<FRankData>& Ranks = GetRanks();
	const int32 Idx = FMath::Clamp(Level, 0, Ranks.Num() - 1);
	return Ranks[Idx];
}
