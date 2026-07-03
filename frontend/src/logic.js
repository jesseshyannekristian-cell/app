// Client-side game rules mirroring the Python engine (game/state.py, game/breach.py).
const clamp = (v, lo, hi) => Math.max(lo, Math.min(hi, v));

export const DEFAULT_STATE = {
  credits: 500, research_credits: 200, xp: 0, rank_level: 0,
  difficulty: 'Normal', ironman: false,
  completed_research: [], owned_equipment: [], completed_ops: [], loadout: [],
  breaches_survived: 0, breaches_failed: 0, scps_recontained: 0,
  unlocked_achievements: [], custom_scps: [],
  squad: [], assigned_squad: [], casualties: [],
};

export const find = (arr, key, val) => arr.find((x) => x[key] === val);

export function rankData(boot, level) {
  const r = boot.ranks;
  return r[clamp(level, 0, r.length - 1)];
}
export const hasNextRank = (boot, s) => s.rank_level + 1 < boot.ranks.length;
export const nextRank = (boot, s) => boot.ranks[clamp(s.rank_level + 1, 0, boot.ranks.length - 1)];

export function isEquipResearched(boot, s, equipId) {
  const e = find(boot.equipment, 'id', equipId);
  if (!e) return false;
  if (!e.requires_research) return true;
  return s.completed_research.includes(`res_${equipId}`);
}

export function effectiveCapacity(boot, s) {
  const base = boot.starting.loadout_capacity;
  const bonus = boot.equipment
    .filter((e) => e.loadout_slot_bonus && s.owned_equipment.includes(e.id))
    .reduce((sum, e) => sum + e.loadout_slot_bonus, 0);
  return base + bonus;
}

function applyRankUps(boot, s) {
  const ranks = boot.ranks;
  while (s.rank_level + 1 < ranks.length && s.xp >= ranks[s.rank_level + 1].xp_required) {
    s.rank_level += 1;
    s.credits += ranks[s.rank_level].credit_reward;
    s.research_credits += ranks[s.rank_level].research_credit_reward;
  }
}
function addRewards(boot, s, cr, rc, xp) {
  s.credits += cr; s.research_credits += rc; s.xp += xp;
  applyRankUps(boot, s);
}

export function opSuccessChance(s, op) {
  const rankAbove = Math.max(0, s.rank_level - op.required_rank);
  return clamp(90 - op.difficulty * 8 + rankAbove * 10, 25, 95);
}

export function tryResearch(boot, s, projId) {
  const p = find(boot.research, 'id', projId);
  if (!p) return { ok: false, msg: 'Unknown project.' };
  if (s.completed_research.includes(projId)) return { ok: false, msg: 'Already researched.' };
  if (s.rank_level < p.required_rank) return { ok: false, msg: `Requires rank ${p.required_rank}.` };
  if (s.research_credits < p.cost) return { ok: false, msg: 'Not enough Research Credits.' };
  const n = { ...s, research_credits: s.research_credits - p.cost, completed_research: [...s.completed_research, projId] };
  return { ok: true, msg: `Research complete: ${p.name}`, state: n };
}

export function tryBuy(boot, s, equipId) {
  const e = find(boot.equipment, 'id', equipId);
  if (!e) return { ok: false, msg: 'Unknown item.' };
  if (s.owned_equipment.includes(equipId)) return { ok: false, msg: 'Already owned.' };
  if (e.requires_research && !isEquipResearched(boot, s, equipId)) return { ok: false, msg: 'Research required first.' };
  if (s.rank_level < e.required_rank) return { ok: false, msg: `Requires rank ${e.required_rank}.` };
  if (s.credits < e.cost) return { ok: false, msg: 'Not enough Credits.' };
  const n = { ...s, credits: s.credits - e.cost, owned_equipment: [...s.owned_equipment, equipId] };
  return { ok: true, msg: `Purchased: ${e.name}`, state: n };
}

export function tryToggleLoadout(boot, s, equipId, capacity = 4) {
  if (!s.owned_equipment.includes(equipId)) return { ok: false, msg: 'You must own this item.' };
  if (s.loadout.includes(equipId)) {
    return { ok: true, msg: 'Removed from loadout.', state: { ...s, loadout: s.loadout.filter((x) => x !== equipId) } };
  }
  if (s.loadout.length >= capacity) return { ok: false, msg: `Loadout full (${capacity} slots).` };
  return { ok: true, msg: 'Added to loadout.', state: { ...s, loadout: [...s.loadout, equipId] } };
}

export function tryRecruit(boot, s, opId) {
  const op = find(boot.squad_roster, 'id', opId);
  if (!op) return { ok: false, msg: 'Unknown operative.' };
  if (s.squad.includes(opId)) return { ok: false, msg: 'Already recruited.' };
  if (s.casualties.includes(opId)) return { ok: false, msg: `${op.codename} is KIA and cannot be re-recruited.` };
  if (s.rank_level < op.required_rank) return { ok: false, msg: `Requires rank ${op.required_rank}.` };
  if (s.credits < op.recruit_cost) return { ok: false, msg: 'Not enough Credits.' };
  const n = { ...s, credits: s.credits - op.recruit_cost, squad: [...s.squad, opId] };
  return { ok: true, msg: `Recruited: ${op.codename}`, state: n };
}

export function squadCapacity(boot) {
  return boot.starting.squad_capacity || 2;
}

export function tryAssignSquad(boot, s, opId) {
  if (!s.squad.includes(opId)) return { ok: false, msg: 'This operative is not on your roster.' };
  if (s.assigned_squad.includes(opId)) {
    return { ok: true, msg: 'Stood down.', state: { ...s, assigned_squad: s.assigned_squad.filter((x) => x !== opId) } };
  }
  const cap = squadCapacity(boot);
  if (s.assigned_squad.length >= cap) return { ok: false, msg: `Squad team full (${cap} slots).` };
  return { ok: true, msg: 'Assigned to next breach.', state: { ...s, assigned_squad: [...s.assigned_squad, opId] } };
}

// Resolves squad casualties after a breach. Called once, after the outcome is known.
// Assigned operatives on a failed breach each face independent KIA risk; on a
// survived breach nobody is lost. Iron-man wipes already reset the whole state
// elsewhere, so this only runs for a normal failure/survival.
export function resolveCasualties(boot, s, survived) {
  if (survived || s.assigned_squad.length === 0) {
    return { state: s, lost: [] };
  }
  const lost = [];
  const survivors = [];
  for (const opId of s.assigned_squad) {
    const op = find(boot.squad_roster, 'id', opId);
    const risk = op ? op.kia_risk : 0.2;
    if (Math.random() < risk) lost.push(opId); else survivors.push(opId);
  }
  if (lost.length === 0) return { state: s, lost: [] };
  const n = {
    ...s,
    squad: s.squad.filter((id) => !lost.includes(id)),
    assigned_squad: survivors,
    casualties: [...s.casualties, ...lost],
  };
  return { state: n, lost };
}

export function tryRunOp(boot, s, opId) {
  const op = find(boot.operations, 'id', opId);
  if (!op) return { ok: false, msg: 'Unknown operation.' };
  if (s.completed_ops.includes(opId)) return { ok: false, msg: 'Already completed.' };
  if (s.rank_level < op.required_rank) return { ok: false, msg: `Requires rank ${op.required_rank}.` };
  if (op.required_research && !s.completed_research.includes(op.required_research))
    return { ok: false, msg: 'Required procedure not researched.' };
  if (op.required_equipment && !s.owned_equipment.includes(op.required_equipment)) {
    const req = find(boot.equipment, 'id', op.required_equipment);
    return { ok: false, msg: `Requires equipment: ${req ? req.name : op.required_equipment}` };
  }
  const chance = opSuccessChance(s, op);
  const roll = 1 + Math.floor(Math.random() * 100);
  const n = { ...s };
  if (roll > chance) {
    addRewards(boot, n, 0, 0, Math.floor(op.reward_xp / 4));
    return { ok: false, success: false, msg: `FAILED (${chance}%). Team withdrew. +${Math.floor(op.reward_xp / 4)} XP.`, state: n };
  }
  n.completed_ops = [...s.completed_ops, opId];
  addRewards(boot, n, op.reward_credits, op.reward_research, op.reward_xp);
  return { ok: true, success: true, msg: `SUCCESS (${chance}%): +${op.reward_credits} cr, +${op.reward_research} RC, +${op.reward_xp} XP`, state: n };
}

// ---- breach ----
export function breachAssess(boot, s, threat, targetNumber = null) {
  let loadoutBonus = 0;
  const lines = [];
  for (const id of s.loadout) {
    const b = boot.loadout_bonus[id];
    if (b) { loadoutBonus += b.bonus; lines.push(`+${b.bonus}% ${(find(boot.equipment, 'id', id) || {}).name || id}`); }
  }
  let procBonus = 0;
  if (targetNumber) {
    for (const p of boot.research) {
      if (p.applies_to === targetNumber && s.completed_research.includes(p.id)) {
        procBonus += p.breach_bonus || 0;
        lines.push(`+${p.breach_bonus}% ${p.name}`);
      }
    }
  }
  let squadBonus = 0;
  for (const opId of s.assigned_squad) {
    const op = find(boot.squad_roster, 'id', opId);
    if (op) { squadBonus += op.breach_bonus; lines.push(`+${op.breach_bonus}% ${op.codename} (${op.role})`); }
  }
  const base = 60 - threat * 8;
  const rankBonus = s.rank_level * 5;
  const diffMod = boot.difficulty_mod[s.difficulty] || 0;
  const chance = clamp(base + rankBonus + loadoutBonus + procBonus + squadBonus + diffMod, 10, 95);
  return { base, rankBonus, loadoutBonus, procBonus, squadBonus, diffMod, chance, lines };
}

export function recordBreach(boot, s, survived, recontained = 0) {
  const n = { ...s };
  if (survived) {
    n.breaches_survived += 1;
    n.scps_recontained += Math.max(0, recontained);
    addRewards(boot, n, 300 + 100 * recontained, 120 + 40 * recontained, 400 + 150 * recontained);
  } else {
    n.breaches_failed += 1;
    addRewards(boot, n, 50, 20, 75);
  }
  return n;
}

// ---- achievements ----
export function checkAchievements(boot, s, ctx = {}) {
  const cond = {
    first_blood: () => s.breaches_survived >= 1,
    survivor: () => s.breaches_survived >= 10,
    untouchable: () => !!ctx.breach_win_no_loadout,
    researcher: () => s.completed_research.length >= 5,
    containment_specialist: () => s.completed_ops.length >= 10,
    armed: () => s.loadout.length >= 4,
    archivist: () => s.custom_scps.length >= 3,
    o5: () => s.rank_level >= boot.ranks.length - 1,
  };
  const newly = [];
  const unlocked = [...s.unlocked_achievements];
  for (const a of boot.achievements) {
    if (unlocked.includes(a.id)) continue;
    if (cond[a.id] && cond[a.id]()) { unlocked.push(a.id); newly.push(a); }
  }
  return { newly, unlocked };
}

export function nextObjective(boot, s) {
  if (s.owned_equipment.length === 0) return 'Visit the Requisition Store and buy your first equipment.';
  const ar = boot.research.find((p) => !s.completed_research.includes(p.id) && s.rank_level >= p.required_rank && s.research_credits >= p.cost);
  if (ar) return `Research ready: ${ar.name}.`;
  const ao = boot.operations.find((o) => !s.completed_ops.includes(o.id) && s.rank_level >= o.required_rank);
  if (hasNextRank(boot, s) && ao) return `Run operations to earn XP toward ${nextRank(boot, s).title}.`;
  if (s.loadout.length === 0) return 'Equip a pre-breach loadout, then respond to a breach.';
  return 'Respond to a breach to contain a live anomaly.';
}
