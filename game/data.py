"""Static game data — ported from the deprecated UE5 FoundationDataLibrary.cpp."""
import json
import os

DATA_DIR = os.path.join(os.path.dirname(__file__), "data")


def _load(name):
    with open(os.path.join(DATA_DIR, name), encoding="utf-8") as f:
        return json.load(f)


SCPS = _load("scps.json")
EQUIPMENT = _load("equipment.json")
RANKS = _load("ranks.json")
PERSONNEL = _load("personnel.json")
ROGUE_MTF = _load("rogue_mtf.json")
_PROCEDURES = _load("procedures.json")

# Map of SCPs that gate a containment operation behind a researched procedure.
_OP_PROC_MAP = {
    "SCP-173": "proc_173", "SCP-106": "proc_106", "SCP-939": "proc_939",
    "SCP-096": "proc_096", "SCP-682": "proc_682", "SCP-035": "proc_035",
    "SCP-16829": "proc_16829",
}


def research_id_for_equip(equip_id):
    return f"res_{equip_id}"


def _build_research():
    projects = []
    for e in EQUIPMENT:
        if not e["requires_research"]:
            continue
        projects.append({
            "id": research_id_for_equip(e["id"]),
            "name": f'R&D: {e["name"]}',
            "category": "Equipment",
            "objective": f'Complete prototype testing to authorise {e["name"]} for site requisition.',
            "description": e["description"],
            "required_rank": e["required_rank"],
            "cost": 100 + e["required_rank"] * 150 + (50 if e["dept"] == "Research" else 0),
            "unlocks_equipment": e["id"],
        })
    projects.extend(_PROCEDURES)
    return projects


RESEARCH = _build_research()


def _build_operations():
    ops = []
    for s in SCPS:
        threat = s["threat"]
        ops.append({
            "id": "op_" + s["number"].replace("-", "_"),
            "name": f'Contain {s["number"]}',
            "target": f'{s["number"]} — {s["name"]}',
            "description": s["procedure"],
            "required_rank": max(0, min(5, threat - 1)),
            "difficulty": threat,
            "reward_credits": 150 * threat,
            "reward_research": 80 * threat,
            "reward_xp": 200 * threat,
            "required_research": _OP_PROC_MAP.get(s["number"]),
            "required_equipment": "micro_hid" if threat >= 5 else ("scranton_anchor" if threat == 4 else None),
        })
    for i, r in enumerate(ROGUE_MTF):
        threat = r["threat"]
        ops.append({
            "id": f"op_rogue_{i}",
            "name": f'Neutralise: {r["name"]}',
            "target": f'{r["name"]} [{r["origin"]}]',
            "description": r["notes"],
            "required_rank": max(0, min(5, threat - 1)),
            "difficulty": threat,
            "reward_credits": 180 * threat,
            "reward_research": 60 * threat,
            "reward_xp": 220 * threat,
            "required_research": None,
            "required_equipment": "epsilon_rifle",
        })
    return ops


OPERATIONS = _build_operations()


def find_equipment(equip_id):
    return next((e for e in EQUIPMENT if e["id"] == equip_id), None)


def find_research(project_id):
    return next((p for p in RESEARCH if p["id"] == project_id), None)


def find_op(op_id):
    return next((o for o in OPERATIONS if o["id"] == op_id), None)


def find_scp(number):
    return next((s for s in SCPS if s["number"] == number), None)


def rank_data(level):
    idx = max(0, min(level, len(RANKS) - 1))
    return RANKS[idx]
