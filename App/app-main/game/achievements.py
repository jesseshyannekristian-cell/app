"""Achievements — unlocked from progression milestones; persisted on GameState."""

ACHIEVEMENTS = [
    {"id": "first_blood", "name": "First Blood", "desc": "Survive your first containment breach."},
    {"id": "survivor", "name": "Old Hand", "desc": "Survive 10 containment breaches."},
    {"id": "untouchable", "name": "Untouchable", "desc": "Survive a breach with NO loadout equipped."},
    {"id": "researcher", "name": "Lab Coat", "desc": "Complete 5 research projects."},
    {"id": "containment_specialist", "name": "Containment Specialist", "desc": "Complete 10 containment operations."},
    {"id": "armed", "name": "Armed to the Teeth", "desc": "Fill all 4 pre-breach loadout slots."},
    {"id": "archivist", "name": "Archivist", "desc": "File 3 custom SCPs in the Archives."},
    {"id": "o5", "name": "OMEGA BLACK", "desc": "Reach the rank of O5 Overseer."},
]

_BY_ID = {a["id"]: a for a in ACHIEVEMENTS}


def _condition(aid, state, archives, ctx):
    if aid == "first_blood":
        return state.breaches_survived >= 1
    if aid == "survivor":
        return state.breaches_survived >= 10
    if aid == "untouchable":
        return bool(ctx and ctx.get("breach_win_no_loadout"))
    if aid == "researcher":
        return len(state.completed_research) >= 5
    if aid == "containment_specialist":
        return len(state.completed_ops) >= 10
    if aid == "armed":
        return len(state.loadout) >= 4
    if aid == "archivist":
        return bool(archives) and len(archives.custom_scps) >= 3
    if aid == "o5":
        return state.rank_level >= 5
    return False


def check(state, archives=None, ctx=None):
    """Unlock any newly-earned achievements; return the list of new ones."""
    newly = []
    for a in ACHIEVEMENTS:
        if a["id"] in state.unlocked_achievements:
            continue
        if _condition(a["id"], state, archives, ctx):
            state.unlocked_achievements.append(a["id"])
            newly.append(a)
    if newly:
        state.save()
    return newly


def get(aid):
    return _BY_ID.get(aid)
