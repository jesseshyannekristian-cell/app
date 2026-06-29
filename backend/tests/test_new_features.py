"""Iteration 3: tests for the NEW batch of features in FOUNDATION SCP Site Overseer.

Covers:
  * Per-SCP branching breach mini-scenarios (game/scenarios.py + breach._branching)
  * ASCII art per anomaly (game/art.py)
  * Achievements unlock + persistence (game/achievements.py)
  * Iron-man mode + New Game wipe (game/state.GameState.new_game)
  * Save on interrupt (main.py try/except KeyboardInterrupt/EOFError)
  * rng.roll bounds validation
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

from game import achievements, art, breach as breach_mod, rng, scenarios  # noqa: E402
from game.archives import Archives, ARCHIVE_PATH  # noqa: E402
from game.state import GameState, SAVE_PATH  # noqa: E402


# ---------- isolation fixture ----------
@pytest.fixture(autouse=True)
def _clean_saves():
    for p in (SAVE_PATH, ARCHIVE_PATH):
        if os.path.exists(p):
            os.remove(p)
    yield
    for p in (SAVE_PATH, ARCHIVE_PATH):
        if os.path.exists(p):
            os.remove(p)


# ====================== Scenarios data ======================
class TestScenariosData:
    def test_six_scenarios_defined(self):
        expected = {"SCP-173", "SCP-096", "SCP-106", "SCP-682", "SCP-049", "SCP-16829"}
        assert set(scenarios.SCENARIOS.keys()) == expected

    def test_has_and_stages(self):
        for num in ("SCP-173", "SCP-096", "SCP-106", "SCP-682", "SCP-049", "SCP-16829"):
            assert scenarios.has(num) is True
            stgs = scenarios.stages(num)
            assert len(stgs) == 2
            for st in stgs:
                assert "prompt" in st and "options" in st
                assert len(st["options"]) == 2
                for label, delta, flavor in st["options"]:
                    assert isinstance(label, str) and label
                    assert isinstance(delta, int)
                    assert isinstance(flavor, str) and flavor

    def test_no_scenario_for_999(self):
        assert scenarios.has("SCP-999") is False
        assert scenarios.stages("SCP-999") == []


# ====================== ASCII art ======================
class TestArt:
    def test_specific_art_for_known_scps(self):
        for num in ("SCP-173", "SCP-096", "SCP-106", "SCP-682", "SCP-049", "SCP-16829"):
            a = art.for_scp(num)
            assert isinstance(a, str) and a.strip()
            assert "?SCP?" not in a, f"{num} fell through to generic art"

    def test_generic_art_for_unknown(self):
        a = art.for_scp("SCP-999")
        assert "?SCP?" in a
        assert "anomalous object" in a

    def test_all_specific_arts_unique(self):
        nums = ["SCP-173", "SCP-096", "SCP-106", "SCP-682", "SCP-049", "SCP-16829"]
        arts = [art.for_scp(n) for n in nums]
        assert len(set(arts)) == len(arts)


# ====================== Branching breach (engine-level) ======================
class TestBranching:
    def test_branching_applies_positive_deltas(self, monkeypatch):
        """Picking option 'a' twice for SCP-173 applies +12 then +10 to chance."""
        # Silence UI and force menu to always return 'a'
        monkeypatch.setattr(breach_mod.ui, "panel", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "a")
        monkeypatch.setattr(breach_mod.ui, "slow_print", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "info", lambda *a, **k: None)
        s = GameState()
        new_chance = breach_mod._branching(s, "SCP-173", 50)
        # 50 + 12 + 10 = 72, within clamp(5..95)
        assert new_chance == 72

    def test_branching_applies_negative_deltas(self, monkeypatch):
        """Picking option 'b' twice for SCP-173 applies -18 then -22 to chance."""
        monkeypatch.setattr(breach_mod.ui, "panel", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "b")
        monkeypatch.setattr(breach_mod.ui, "slow_print", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "info", lambda *a, **k: None)
        s = GameState()
        new_chance = breach_mod._branching(s, "SCP-173", 80)
        # 80 - 18 - 22 = 40
        assert new_chance == 40

    def test_branching_clamps_to_5_95(self, monkeypatch):
        monkeypatch.setattr(breach_mod.ui, "panel", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "b")
        monkeypatch.setattr(breach_mod.ui, "slow_print", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "info", lambda *a, **k: None)
        s = GameState()
        # SCP-096 'b' twice = -28 - 12 = -40; from 30 should clamp to 5 (not -10)
        new_chance = breach_mod._branching(s, "SCP-096", 30)
        assert new_chance == 5

    def test_branching_clamps_high(self, monkeypatch):
        monkeypatch.setattr(breach_mod.ui, "panel", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "a")
        monkeypatch.setattr(breach_mod.ui, "slow_print", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "info", lambda *a, **k: None)
        s = GameState()
        # SCP-682 'a' twice = +10 + +18 = +28; from 90 should clamp to 95
        new_chance = breach_mod._branching(s, "SCP-682", 90)
        assert new_chance == 95

    def test_branching_improvise_uses_rng(self, monkeypatch):
        monkeypatch.setattr(breach_mod.ui, "panel", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "x")
        monkeypatch.setattr(breach_mod.ui, "slow_print", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "info", lambda *a, **k: None)
        # Force rng.chance(50) → True (good improvise) → +8 each stage
        monkeypatch.setattr(breach_mod.rng, "chance", lambda p: True)
        s = GameState()
        new_chance = breach_mod._branching(s, "SCP-173", 50)
        assert new_chance == 50 + 8 + 8

    def test_run_breach_branching_resolves_to_contained(self, monkeypatch):
        """e2e: run_breach on a scenario SCP with high chance + rng.roll=1 -> survived."""
        monkeypatch.setattr(breach_mod.ui, "clear", lambda: None)
        monkeypatch.setattr(breach_mod.ui, "art", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "panel", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "slow_print", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "info", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "bad", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "confirm", lambda *a, **k: True)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "a")
        from rich.console import Console
        monkeypatch.setattr(breach_mod.ui, "console", Console(quiet=True))
        # Avoid render_assessment Table styling
        monkeypatch.setattr(breach_mod, "_render_assessment", lambda *a, **k: None)
        # Force roll low → survive
        monkeypatch.setattr(breach_mod.rng, "roll", lambda lo, hi: 1)
        s = GameState()
        result = breach_mod.run_breach(s, {"number": "SCP-173", "name": "Statue", "threat": 3})
        assert result is True
        assert s.breaches_survived == 1


# ====================== Achievements ======================
class TestAchievements:
    def test_achievements_count_and_ids(self):
        assert len(achievements.ACHIEVEMENTS) == 8
        ids = {a["id"] for a in achievements.ACHIEVEMENTS}
        assert ids == {
            "first_blood", "survivor", "untouchable", "researcher",
            "containment_specialist", "armed", "archivist", "o5",
        }

    def test_first_blood_unlocks_on_survival(self):
        s = GameState()
        s.breaches_survived = 1
        newly = achievements.check(s)
        ids = [a["id"] for a in newly]
        assert "first_blood" in ids
        assert "first_blood" in s.unlocked_achievements
        # persisted to disk
        with open(SAVE_PATH) as f:
            assert "first_blood" in json.load(f)["unlocked_achievements"]

    def test_untouchable_via_ctx(self):
        s = GameState()
        s.breaches_survived = 1
        newly = achievements.check(s, None, {"breach_win_no_loadout": True})
        ids = [a["id"] for a in newly]
        assert "untouchable" in ids and "first_blood" in ids

    def test_untouchable_requires_ctx_flag(self):
        s = GameState()
        s.breaches_survived = 1
        newly = achievements.check(s, None, {"breach_win_no_loadout": False})
        assert "untouchable" not in [a["id"] for a in newly]

    def test_armed_unlocks_at_4_loadout(self):
        s = GameState()
        s.loadout = ["a", "b", "c", "d"]
        newly = achievements.check(s)
        assert "armed" in [a["id"] for a in newly]

    def test_archivist_unlocks_at_3_archives(self):
        s = GameState()
        a = Archives.load()
        for i in range(3):
            a.add(f"SCP-{1000+i}", f"TEST_n{i}", "Safe", "", False)
        newly = achievements.check(s, a)
        assert "archivist" in [x["id"] for x in newly]

    def test_lab_coat_at_5_research(self):
        s = GameState()
        s.completed_research = ["r1", "r2", "r3", "r4", "r5"]
        newly = achievements.check(s)
        assert "researcher" in [a["id"] for a in newly]

    def test_containment_specialist_at_10_ops(self):
        s = GameState()
        s.completed_ops = [f"op{i}" for i in range(10)]
        newly = achievements.check(s)
        assert "containment_specialist" in [a["id"] for a in newly]

    def test_old_hand_at_10_breaches(self):
        s = GameState()
        s.breaches_survived = 10
        newly = achievements.check(s)
        ids = [a["id"] for a in newly]
        assert "survivor" in ids and "first_blood" in ids

    def test_o5_at_rank_5(self):
        s = GameState()
        s.rank_level = 5
        newly = achievements.check(s)
        assert "o5" in [a["id"] for a in newly]

    def test_no_double_unlock(self):
        s = GameState()
        s.breaches_survived = 1
        newly1 = achievements.check(s)
        assert "first_blood" in [a["id"] for a in newly1]
        newly2 = achievements.check(s)
        assert newly2 == []  # already unlocked


# ====================== Iron-man & New Game ======================
class TestIronManAndNewGame:
    def test_new_game_wipes_progression(self):
        s = GameState()
        s.credits = 9999
        s.research_credits = 8888
        s.xp = 5000
        s.rank_level = 4
        s.completed_research = ["x"]
        s.owned_equipment = ["y"]
        s.completed_ops = ["z"]
        s.loadout = ["w"]
        s.breaches_survived = 7
        s.breaches_failed = 2
        s.scps_recontained = 5
        s.unlocked_achievements = ["first_blood"]
        s.save()

        s.new_game(ironman=False)

        assert s.credits == 500
        assert s.research_credits == 200
        assert s.xp == 0
        assert s.rank_level == 0
        assert s.completed_research == []
        assert s.owned_equipment == []
        assert s.completed_ops == []
        assert s.loadout == []
        assert s.breaches_survived == 0
        assert s.breaches_failed == 0
        assert s.scps_recontained == 0
        assert s.unlocked_achievements == []
        assert s.ironman is False
        # Persisted
        with open(SAVE_PATH) as f:
            d = json.load(f)
        assert d["credits"] == 500
        assert d["unlocked_achievements"] == []
        assert d["ironman"] is False

    def test_new_game_enables_ironman_flag(self):
        s = GameState()
        s.new_game(ironman=True)
        assert s.ironman is True
        with open(SAVE_PATH) as f:
            assert json.load(f)["ironman"] is True

    def test_ironman_wipe_on_failed_breach(self, monkeypatch):
        """A FAILED scenario breach with ironman=True must reset save."""
        monkeypatch.setattr(breach_mod.ui, "clear", lambda: None)
        monkeypatch.setattr(breach_mod.ui, "art", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "panel", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "slow_print", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "info", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "bad", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "confirm", lambda *a, **k: True)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "b")  # bad choices
        from rich.console import Console
        monkeypatch.setattr(breach_mod.ui, "console", Console(quiet=True))
        monkeypatch.setattr(breach_mod, "_render_assessment", lambda *a, **k: None)
        # Force resolve roll = 100 → fail
        monkeypatch.setattr(breach_mod.rng, "roll", lambda lo, hi: 100)

        s = GameState()
        s.ironman = True
        s.credits = 9999
        s.xp = 2000
        s.rank_level = 3
        s.save()

        result = breach_mod.run_breach(s, {"number": "SCP-173", "name": "Statue", "threat": 3})
        assert result is None  # iron-man wipe path returns None
        # save file reset
        with open(SAVE_PATH) as f:
            d = json.load(f)
        assert d["credits"] == 500
        assert d["xp"] == 0
        assert d["rank_level"] == 0
        assert d["ironman"] is True

    def test_ironman_does_not_wipe_on_success(self, monkeypatch):
        monkeypatch.setattr(breach_mod.ui, "clear", lambda: None)
        monkeypatch.setattr(breach_mod.ui, "art", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "panel", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "slow_print", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "info", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "bad", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "confirm", lambda *a, **k: True)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "a")
        from rich.console import Console
        monkeypatch.setattr(breach_mod.ui, "console", Console(quiet=True))
        monkeypatch.setattr(breach_mod, "_render_assessment", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.rng, "roll", lambda lo, hi: 1)

        s = GameState()
        s.ironman = True
        s.credits = 1500
        s.save()
        result = breach_mod.run_breach(s, {"number": "SCP-173", "name": "Statue", "threat": 3})
        assert result is True
        # ironman preserved
        with open(SAVE_PATH) as f:
            d = json.load(f)
        assert d["ironman"] is True
        assert d["breaches_survived"] == 1

    def test_header_shows_ironman_marker(self):
        """ui.header prints '☠ IRON-MAN' when state.ironman is True."""
        from io import StringIO
        from rich.console import Console
        from game import ui

        s = GameState()
        s.ironman = True
        buf = StringIO()
        original_console = ui.console
        ui.console = Console(file=buf, force_terminal=False, width=160)
        try:
            ui.header(s)
        finally:
            ui.console = original_console
        out = buf.getvalue()
        assert "IRON-MAN" in out


# ====================== rng.roll bounds ======================
class TestRngBounds:
    def test_roll_raises_on_high_lt_low(self):
        with pytest.raises(ValueError):
            rng.roll(10, 5)

    def test_roll_equal_bounds_ok(self):
        assert rng.roll(7, 7) == 7


# ====================== Save on interrupt (CLI) ======================
class TestSaveOnInterrupt:
    """Closing stdin while at the hub causes EOFError → except handler saves."""

    def test_eof_at_hub_saves_state(self):
        env = {**os.environ, "TERM": "xterm", "COLUMNS": "120", "LINES": "45",
               "PYTHONIOENCODING": "utf-8", "SCP_NO_DELAY": "1"}
        # Only one newline (clears initial banner pause); then EOF triggers the
        # KeyboardInterrupt/EOFError handler at the next ui.menu() prompt.
        r = subprocess.run(
            [sys.executable, "-m", "game.main"],
            input="\n", cwd=APP_DIR, env=env,
            capture_output=True, text=True, timeout=15,
        )
        assert r.returncode == 0, f"non-zero exit: {r.returncode}\n{r.stderr}"
        assert "Traceback" not in r.stderr
        assert "Interrupt received" in r.stdout
        assert os.path.exists(SAVE_PATH)
        with open(SAVE_PATH) as f:
            d = json.load(f)
        assert d["credits"] == 500
        assert "ironman" in d
        assert "unlocked_achievements" in d


# ====================== CLI: branching breach end-to-end ======================
class TestCliBranchingBreach:
    """Drive main.py via stdin: open Breach menu → SCP-173 → deploy y → a/a."""

    def test_scp173_branching_runs_two_decision_stages(self):
        env = {**os.environ, "TERM": "xterm", "COLUMNS": "140", "LINES": "60",
               "PYTHONIOENCODING": "utf-8", "SCP_NO_DELAY": "1"}
        # Stdin sequence:
        #  \n     -> dismiss initial banner pause
        #  5\n    -> open Breach menu
        #  1\n    -> choose SCP-173 (1st breachable target)
        #  y\n    -> confirm deploy
        #  a\n    -> stage-1 option a
        #  a\n    -> stage-2 option a
        #  \n     -> ui.pause after breach
        #  q\n    -> save & quit
        stdin = "\n5\n1\ny\na\na\n\nq\n"
        r = subprocess.run(
            [sys.executable, "-m", "game.main"],
            input=stdin, cwd=APP_DIR, env=env,
            capture_output=True, text=True, timeout=20,
        )
        assert r.returncode == 0, f"exit={r.returncode}\n{r.stderr}"
        assert "Traceback" not in r.stderr
        # DECISION panels appeared
        assert "DECISION" in r.stdout
        # Survival chance was updated by branching
        assert "Survival chance now" in r.stdout
        # Resolved one way or the other
        assert ("BREACH CONTAINED" in r.stdout) or ("CONTAINMENT FAILED" in r.stdout)
