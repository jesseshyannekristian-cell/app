"""Regression tests for FOUNDATION — SCP Site Overseer CLI game.

These tests cover the recent refactor:
  * game/breach.py — decomposed run_breach into _assess/_render_assessment/_simulate/_resolve
  * game/hub.py    — extracted status-cell + archive helpers, dispatch dicts
  * game/rng.py    — secrets-backed RNG (roll/choice/chance)

This is a pure-Python terminal app. We exercise the engine modules directly
and also smoke-run the CLI via subprocess (stdin piping) to make sure the menu
loop, banner and 'Progress saved' line still work.
"""
import json
import os
import subprocess
import sys

import pytest

# Make /app importable
APP_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if APP_DIR not in sys.path:
    sys.path.insert(0, APP_DIR)

from game import data, rng  # noqa: E402
from game.archives import Archives, ARCHIVE_PATH  # noqa: E402
from game.state import (  # noqa: E402
    GameState,
    SAVE_PATH,
    STARTING_CREDITS,
    STARTING_RESEARCH_CREDITS,
    LOADOUT_CAPACITY,
)
from game import breach as breach_mod  # noqa: E402


# ---------- fixtures ----------
@pytest.fixture(autouse=True)
def _clean_saves():
    """Delete /app/saves/*.json before AND after every test for isolation."""
    for p in (SAVE_PATH, ARCHIVE_PATH):
        if os.path.exists(p):
            os.remove(p)
    yield
    for p in (SAVE_PATH, ARCHIVE_PATH):
        if os.path.exists(p):
            os.remove(p)


# ====================== CLI smoke ======================
class TestCliSmoke:
    """Drive the program via stdin and assert on stdout."""

    def _run_cli(self, stdin_text, timeout=15):
        env = {**os.environ, "TERM": "xterm", "COLUMNS": "120", "LINES": "45",
               "PYTHONIOENCODING": "utf-8"}
        return subprocess.run(
            [sys.executable, "-m", "game.main"],
            input=stdin_text, cwd=APP_DIR, env=env,
            capture_output=True, text=True, timeout=timeout,
        )

    def test_launch_and_quit(self):
        """`\\nq\\n` → banner, hub, Progress saved, exit 0, no traceback."""
        r = self._run_cli("\nq\n")
        assert r.returncode == 0, f"non-zero exit: {r.returncode}\n{r.stderr}"
        assert "Traceback" not in r.stderr, f"traceback on launch:\n{r.stderr}"
        assert "OVERSEER" in r.stdout
        assert "SITE OVERSEER — MAIN HUB" in r.stdout
        assert "Progress saved" in r.stdout

    def test_fresh_save_file_created(self):
        self._run_cli("\nq\n")
        assert os.path.exists(SAVE_PATH), "save_data.json was not created"
        with open(SAVE_PATH) as f:
            d = json.load(f)
        assert d["credits"] == STARTING_CREDITS == 500
        assert d["research_credits"] == STARTING_RESEARCH_CREDITS == 200
        assert d["rank_level"] == 0
        assert d["xp"] == 0
        assert d["completed_research"] == []
        assert d["owned_equipment"] == []
        assert d["loadout"] == []


# ====================== Data sanity ======================
class TestDataIntegrity:
    def test_counts_match_spec(self):
        assert len(data.SCPS) == 22
        assert len(data.EQUIPMENT) == 15
        assert len(data.RANKS) == 6
        assert len(data.OPERATIONS) == 30
        assert len(data.RESEARCH) >= 1  # derived

    def test_scp_index_9_is_999(self):
        """Breach menu target index 9 (1-based) must be SCP-999."""
        scp = data.SCPS[8]  # 0-based 8 == 1-based 9
        assert scp["number"] == "SCP-999"
        assert scp["breachable"] is True
        assert scp["threat"] == 1

    def test_research_required_ranks(self):
        assert data.find_research("res_kevlar_vest")["required_rank"] == 1
        assert data.find_research("res_faraday_rig")["required_rank"] == 2

    def test_op_scp_999_rank0(self):
        op = data.find_op("op_SCP_999")
        assert op is not None
        assert op["required_rank"] == 0


# ====================== RNG ======================
class TestRng:
    def test_roll_within_bounds(self):
        for _ in range(200):
            v = rng.roll(1, 100)
            assert 1 <= v <= 100
        for _ in range(50):
            v = rng.roll(5, 5)
            assert v == 5  # degenerate inclusive range

    def test_choice_returns_member(self):
        seq = ["a", "b", "c", "d"]
        for _ in range(50):
            assert rng.choice(seq) in seq

    def test_chance_distribution(self):
        # chance(0) always False, chance(100) always True
        assert all(not rng.chance(0) for _ in range(50))
        assert all(rng.chance(100) for _ in range(50))


# ====================== GameState — research / store / ops ======================
class TestGameStateActions:
    def test_research_rank_lock_at_start(self):
        s = GameState()
        ok, msg = s.try_complete_research("res_kevlar_vest")
        assert ok is False
        assert "rank" in msg.lower()

    def test_research_succeeds_when_eligible(self):
        s = GameState()
        s.rank_level = 2  # unlock kevlar (rank 1) and faraday (rank 2)
        rc_before = s.research_credits = 1000
        ok, msg = s.try_complete_research("res_kevlar_vest")
        assert ok is True, msg
        assert "Research complete" in msg
        assert s.research_credits == rc_before - data.find_research("res_kevlar_vest")["cost"]
        assert s.is_researched("res_kevlar_vest")
        # Persisted
        with open(SAVE_PATH) as f:
            assert "res_kevlar_vest" in json.load(f)["completed_research"]

    def test_research_blocks_duplicate(self):
        s = GameState()
        s.rank_level = 2
        s.research_credits = 5000  # plenty
        ok, _ = s.try_complete_research("res_kevlar_vest")
        assert ok is True
        ok, msg = s.try_complete_research("res_kevlar_vest")
        assert ok is False and "Already" in msg

    def test_store_buy_deducts_credits(self):
        s = GameState()
        # flashlight is a basic rank-0 item that does not require research
        fl = next(e for e in data.EQUIPMENT if e["id"] == "flashlight")
        assert fl["required_rank"] == 0
        credits_before = s.credits
        ok, msg = s.try_purchase("flashlight")
        assert ok is True, msg
        assert s.credits == credits_before - fl["cost"]
        assert s.is_owned("flashlight")

    def test_store_blocks_without_research(self):
        s = GameState()
        s.rank_level = 5  # avoid rank lock
        s.credits = 99999
        # Kevlar vest requires research
        ok, msg = s.try_purchase("kevlar_vest")
        assert ok is False
        assert "Research" in msg

    def test_operation_success_chance_formula(self):
        s = GameState()
        # op_SCP_999: difficulty 1, required_rank 0 → 90 - 8 + 0 = 82, clamped 25..95
        assert s.operation_success_chance("op_SCP_999") == 82

    def test_op_lock_when_below_required_rank(self):
        s = GameState()
        # find an op that requires rank >= 1
        locked = next(o for o in data.OPERATIONS if o["required_rank"] >= 1)
        ok, msg = s.try_run_operation(locked["id"])
        assert ok is False
        assert "rank" in msg.lower()

    def test_op_dispatch_updates_currencies_or_xp(self):
        s = GameState()
        cr_before, rc_before, xp_before = s.credits, s.research_credits, s.xp
        ok, msg = s.try_run_operation("op_SCP_999")
        # 82% chance — either success or fail, both update state
        if ok:
            op = data.find_op("op_SCP_999")
            assert s.credits == cr_before + op["reward_credits"]
            assert s.research_credits == rc_before + op["reward_research"]
            assert s.xp >= xp_before + op["reward_xp"]
            assert "SUCCESS" in msg
        else:
            # Failure still grants quarter XP
            assert s.xp > xp_before
            assert "FAILED" in msg


# ====================== Loadout ======================
class TestLoadout:
    def test_toggle_requires_ownership(self):
        s = GameState()
        ok, msg = s.toggle_loadout("flashlight")
        assert ok is False
        assert "own" in msg.lower()

    def test_toggle_add_then_remove(self):
        s = GameState()
        s.try_purchase("flashlight")
        ok, msg = s.toggle_loadout("flashlight")
        assert ok is True and "Added" in msg
        assert s.is_in_loadout("flashlight")
        ok, msg = s.toggle_loadout("flashlight")
        assert ok is True and "Removed" in msg
        assert not s.is_in_loadout("flashlight")

    def test_loadout_capacity(self):
        s = GameState()
        # Directly grant 5 owned items (engine-level test of capacity rule)
        owned_ids = [e["id"] for e in data.EQUIPMENT][:5]
        s.owned_equipment = list(owned_ids)
        for i in owned_ids[:LOADOUT_CAPACITY]:
            ok, _ = s.toggle_loadout(i)
            assert ok
        ok, msg = s.toggle_loadout(owned_ids[LOADOUT_CAPACITY])
        assert ok is False and "full" in msg.lower()


# ====================== Breach ======================
class TestBreach:
    def test_assess_clamps_and_formula(self):
        s = GameState()
        # threat 1, rank 0, no loadout → 60-8+0 = 52, clamped
        chance, base, rb, lb, lines = breach_mod._assess(s, 1)
        assert base == 52
        assert rb == 0 and lb == 0
        assert chance == 52
        assert lines == []

    def test_assess_with_loadout_bonus(self):
        s = GameState()
        # Force loadout — bypass purchase chain
        s.loadout = ["kevlar_vest", "flashlight"]  # 8% + 4% = 12%
        chance, base, rb, lb, _lines = breach_mod._assess(s, 3)
        assert lb == 12
        assert chance == max(10, min(95, base + rb + lb))

    def test_assess_clamp_low_and_high(self):
        s = GameState()
        # extreme threat → would go below 10
        chance, *_ = breach_mod._assess(s, 99)
        assert chance == 10
        # huge bonus → clamp to 95
        s.rank_level = 50
        s.loadout = ["micro_hid", "scranton_anchor", "epsilon_rifle", "kevlar_vest"]
        chance, *_ = breach_mod._assess(s, 1)
        assert chance == 95

    def test_record_breach_result_success(self):
        s = GameState()
        s.rank_level = len(data.RANKS) - 1  # max rank → no further rank-up bonuses
        s.xp = 999999
        cr, rc, xp = s.credits, s.research_credits, s.xp
        s.record_breach_result(True, recontained=1)
        assert s.breaches_survived == 1
        assert s.scps_recontained == 1
        # success rewards: 300+100*1 cr, 120+40*1 RC, 400+150*1 XP
        assert s.credits == cr + 400
        assert s.research_credits == rc + 160
        assert s.xp == xp + 550

    def test_record_breach_result_failure(self):
        s = GameState()
        s.rank_level = len(data.RANKS) - 1
        s.xp = 999999
        cr, rc, xp = s.credits, s.research_credits, s.xp
        s.record_breach_result(False)
        assert s.breaches_failed == 1
        assert s.scps_recontained == 0
        assert s.credits == cr + 50
        assert s.research_credits == rc + 20
        assert s.xp == xp + 75


# ====================== Archives CRUD ======================
class TestArchives:
    def test_add_view_update_delete_persist(self):
        a = Archives.load()
        assert a.custom_scps == []
        entry = a.add("SCP-7777", "TEST_Whisper Lamp", "Euclid",
                      "Speaks softly when unobserved.", True)
        assert entry["id"] == 1
        assert entry["is_breachable"] is True
        assert os.path.exists(ARCHIVE_PATH)

        # Re-load from disk → persistence
        a2 = Archives.load()
        assert len(a2.custom_scps) == 1
        assert a2.get(1)["designation"] == "SCP-7777"
        assert a2.breachable() and a2.breachable()[0]["id"] == 1

        # Update
        a2.update(1, name="TEST_Whisper Lamp v2", is_breachable=False)
        a3 = Archives.load()
        assert a3.get(1)["name"] == "TEST_Whisper Lamp v2"
        assert a3.get(1)["is_breachable"] is False
        assert a3.breachable() == []

        # Delete
        ok = a3.delete(1)
        assert ok is True
        a4 = Archives.load()
        assert a4.custom_scps == []

    def test_next_id_increments(self):
        a = Archives.load()
        a.add("SCP-1", "TEST_a", "Safe", "", False)
        a.add("SCP-2", "TEST_b", "Safe", "", False)
        a.add("SCP-3", "TEST_c", "Safe", "", False)
        ids = [s["id"] for s in a.custom_scps]
        assert ids == [1, 2, 3]
        # delete middle → next id is max+1, not gap fill
        a.delete(2)
        a.add("SCP-4", "TEST_d", "Safe", "", False)
        assert a.custom_scps[-1]["id"] == 4

    def test_breachable_appears_in_breach_targets(self):
        """Custom breachable archive must be selectable in hub.breach_menu targets list."""
        a = Archives.load()
        a.add("SCP-9999", "TEST_BreachMe", "Keter", "anomalous", True)
        site20 = [s for s in data.SCPS if s["breachable"]]
        custom_breach = a.breachable()
        assert any(c["designation"] == "SCP-9999" for c in custom_breach)
        # Total selectable count
        total = len(site20) + len(custom_breach)
        assert total == len(site20) + 1


# ====================== Persistence across processes ======================
class TestProcessPersistence:
    def test_state_and_archives_persist_across_runs(self):
        # Run 1: mutate state directly, save
        s = GameState()
        s.credits = 1234
        s.research_credits = 567
        s.rank_level = 2
        s.owned_equipment = ["flashlight"]
        s.breaches_survived = 3
        s.save()

        a = Archives.load()
        a.add("SCP-8888", "TEST_Persist", "Safe", "persisted", True)

        # Run 2: separate subprocess loads + quits, must show the data on header,
        # then reloading in this process must show same data.
        env = {**os.environ, "TERM": "xterm", "COLUMNS": "120", "LINES": "45"}
        r = subprocess.run(
            [sys.executable, "-m", "game.main"],
            input="\nq\n", cwd=APP_DIR, env=env,
            capture_output=True, text=True, timeout=15,
        )
        assert r.returncode == 0
        assert "Traceback" not in r.stderr
        # Header shows currencies — credits 1234 RESEARCH 567
        assert "1234" in r.stdout
        assert "567" in r.stdout

        s2 = GameState.load()
        assert s2.credits == 1234
        assert s2.research_credits == 567
        assert s2.rank_level == 2
        assert s2.owned_equipment == ["flashlight"]
        assert s2.breaches_survived == 3

        a2 = Archives.load()
        assert any(x["designation"] == "SCP-8888" for x in a2.custom_scps)
