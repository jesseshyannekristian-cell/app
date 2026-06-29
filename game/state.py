"""Player progression + save/load — ported from OverseerProgression.cpp."""
import json
import os

from . import data, rng

SAVE_DIR = os.path.join(os.path.dirname(os.path.dirname(__file__)), "saves")
SAVE_PATH = os.path.join(SAVE_DIR, "save_data.json")

LOADOUT_CAPACITY = 4
STARTING_CREDITS = 500
STARTING_RESEARCH_CREDITS = 200


class GameState:
    def __init__(self):
        self.credits = STARTING_CREDITS
        self.research_credits = STARTING_RESEARCH_CREDITS
        self.xp = 0
        self.rank_level = 0
        self.completed_research = []
        self.owned_equipment = []
        self.completed_ops = []
        self.loadout = []
        self.breaches_survived = 0
        self.breaches_failed = 0
        self.scps_recontained = 0
        self.ironman = False
        self.difficulty = "Normal"
        self.unlocked_achievements = []

    def new_game(self, ironman=False, difficulty="Normal"):
        """Reset progression in place (used by New Game and Iron-man permadeath)."""
        self.__init__()
        self.ironman = ironman
        self.difficulty = difficulty
        self.save()

    # ---------- persistence ----------
    def to_dict(self):
        return self.__dict__.copy()

    @classmethod
    def from_dict(cls, d):
        s = cls()
        for k, v in d.items():
            if hasattr(s, k):
                setattr(s, k, v)
        return s

    @classmethod
    def load(cls):
        if os.path.exists(SAVE_PATH):
            with open(SAVE_PATH, encoding="utf-8") as f:
                return cls.from_dict(json.load(f))
        s = cls()
        s.save()
        return s

    def save(self):
        os.makedirs(SAVE_DIR, exist_ok=True)
        with open(SAVE_PATH, "w", encoding="utf-8") as f:
            json.dump(self.to_dict(), f, indent=2)

    # ---------- rank ----------
    def current_rank(self):
        return data.rank_data(self.rank_level)

    def has_next_rank(self):
        return self.rank_level + 1 < len(data.RANKS)

    def next_rank(self):
        return data.rank_data(self.rank_level + 1)

    # ---------- queries ----------
    def is_researched(self, project_id):
        return project_id in self.completed_research

    def is_equipment_researched(self, equip_id):
        e = data.find_equipment(equip_id)
        if not e:
            return False
        if not e["requires_research"]:
            return True
        return self.is_researched(data.research_id_for_equip(equip_id))

    def is_owned(self, equip_id):
        return equip_id in self.owned_equipment

    def is_op_complete(self, op_id):
        return op_id in self.completed_ops

    def operation_success_chance(self, op_id):
        op = data.find_op(op_id)
        if not op:
            return 0
        rank_above = max(0, self.rank_level - op["required_rank"])
        chance = 90 - op["difficulty"] * 8 + rank_above * 10
        return max(25, min(95, chance))

    def is_in_loadout(self, equip_id):
        return equip_id in self.loadout

    # ---------- actions ----------
    def toggle_loadout(self, equip_id):
        if not self.is_owned(equip_id):
            return False, "You must own this equipment first."
        if equip_id in self.loadout:
            self.loadout.remove(equip_id)
            self.save()
            return True, "Removed from loadout."
        if len(self.loadout) >= LOADOUT_CAPACITY:
            return False, f"Loadout full ({LOADOUT_CAPACITY} slots)."
        self.loadout.append(equip_id)
        self.save()
        return True, "Added to loadout."

    def try_complete_research(self, project_id):
        p = data.find_research(project_id)
        if not p:
            return False, "Unknown research project."
        if self.is_researched(project_id):
            return False, "Already researched."
        if self.rank_level < p["required_rank"]:
            return False, f'Requires rank {p["required_rank"]}.'
        if self.research_credits < p["cost"]:
            return False, "Not enough Research Credits."
        self.research_credits -= p["cost"]
        self.completed_research.append(project_id)
        self.save()
        return True, f'Research complete: {p["name"]}'

    def try_purchase(self, equip_id):
        e = data.find_equipment(equip_id)
        if not e:
            return False, "Unknown item."
        if self.is_owned(equip_id):
            return False, "Already owned."
        if e["requires_research"] and not self.is_equipment_researched(equip_id):
            return False, "Research required before purchase."
        if self.rank_level < e["required_rank"]:
            return False, f'Requires rank {e["required_rank"]}.'
        if self.credits < e["cost"]:
            return False, "Not enough Credits."
        self.credits -= e["cost"]
        self.owned_equipment.append(equip_id)
        self.save()
        return True, f'Purchased: {e["name"]}'

    def try_run_operation(self, op_id):
        op = data.find_op(op_id)
        if not op:
            return False, "Unknown operation."
        if self.is_op_complete(op_id):
            return False, "Operation already completed."
        if self.rank_level < op["required_rank"]:
            return False, f'Requires rank {op["required_rank"]}.'
        if op["required_research"] and not self.is_researched(op["required_research"]):
            return False, "Required containment procedure not researched."
        if op["required_equipment"] and not self.is_owned(op["required_equipment"]):
            req = data.find_equipment(op["required_equipment"])
            return False, f'Requires equipment: {req["name"] if req else op["required_equipment"]}'

        chance = self.operation_success_chance(op_id)
        roll = rng.roll(1, 100)
        if roll > chance:
            partial = op["reward_xp"] // 4
            self.add_rewards(0, 0, partial)
            self.save()
            return False, f'Operation FAILED ({chance}% chance). Team withdrew. +{partial} XP field experience.'

        self.completed_ops.append(op_id)
        self.add_rewards(op["reward_credits"], op["reward_research"], op["reward_xp"])
        self.save()
        return True, (f'Operation SUCCESS ({chance}% chance): '
                      f'+{op["reward_credits"]} cr, +{op["reward_research"]} RC, +{op["reward_xp"]} XP')

    def record_breach_result(self, survived, recontained=0):
        if survived:
            self.breaches_survived += 1
            self.scps_recontained += max(0, recontained)
            self.add_rewards(300 + 100 * recontained, 120 + 40 * recontained, 400 + 150 * recontained)
        else:
            self.breaches_failed += 1
            self.add_rewards(50, 20, 75)
        self.save()

    # ---------- internals ----------
    def add_rewards(self, credits, research_credits, xp):
        self.credits += credits
        self.research_credits += research_credits
        self.xp += xp
        self._check_rank_up()

    def _check_rank_up(self):
        ranks = data.RANKS
        while self.rank_level + 1 < len(ranks) and self.xp >= ranks[self.rank_level + 1]["xp_required"]:
            self.rank_level += 1
            r = ranks[self.rank_level]
            self.credits += r["credit_reward"]
            self.research_credits += r["research_credit_reward"]
