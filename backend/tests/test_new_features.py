"""Tests for the feature batches in FOUNDATION SCP Site Overseer.

Covers:
  * Per-SCP branching breach mini-scenarios (game/scenarios.py + breach._branching)
  * ASCII art per anomaly (game/art.py)
  * Achievements unlock + persistence (game/achievements.py)
  * Iron-man mode + New Game wipe (game/state.GameState.new_game)
  * Difficulty tiers (game/breach.DIFFICULTY_MOD)
  * Save on interrupt (main.py try/except KeyboardInterrupt/EOFError)
  * rng.roll bounds validation

Note: `is None` comparisons are intentional singleton identity checks; boolean
singletons are asserted via truthiness (`assert x` / `assert not x`).
"""
import json
import os
import subprocess
import sys
from typing import Iterator

import pytest

# Make /app importable
APP_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
if APP_DIR not in sys.path:
    sys.path.insert(0, APP_DIR)

from game import achievements, art, breach as breach_mod, rng, scenarios  # noqa: E402
from game.archives import Archives, ARCHIVE_PATH  # noqa: E402
from game.state import GameState, SAVE_PATH  # noqa: E402

_KNOWN_SCENARIO_SCPS = ("SCP-173", "SCP-096", "SCP-106", "SCP-682", "SCP-049", "SCP-16829")


# ---------- isolation fixture ----------
@pytest.fixture(autouse=True)
def _clean_saves() -> Iterator[None]:
    for p in (SAVE_PATH, ARCHIVE_PATH):
        if os.path.exists(p):
            os.remove(p)
    yield
    for p in (SAVE_PATH, ARCHIVE_PATH):
        if os.path.exists(p):
            os.remove(p)


def _silence_breach_ui(monkeypatch: pytest.MonkeyPatch, menu_choice: str = "a") -> None:
    """No-op the breach UI surface and force a fixed menu choice."""
    for name in ("clear", "art", "panel", "slow_print", "info", "bad"):
        monkeypatch.setattr(breach_mod.ui, name, lambda *a, **k: None)
    monkeypatch.setattr(breach_mod.ui, "confirm", lambda *a, **k: True)
    monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: menu_choice)
    from rich.console import Console
    monkeypatch.setattr(breach_mod.ui, "console", Console(quiet=True))
    monkeypatch.setattr(breach_mod, "_render_assessment", lambda *a, **k: None)


# ====================== Scenarios data ======================
class TestScenariosData:
    def test_all_site20_scps_have_scenarios(self) -> None:
        from game import data as gdata
        for s in gdata.SCPS:
            assert scenarios.has(s["number"]), f'{s["number"]} missing a breach scenario'

    def test_known_scps_have_two_stages(self) -> None:
        for num in _KNOWN_SCENARIO_SCPS:
            assert scenarios.has(num)
            assert len(scenarios.stages(num)) == 2

    def test_stage_options_well_formed(self) -> None:
        for num in _KNOWN_SCENARIO_SCPS:
            for st in scenarios.stages(num):
                assert "prompt" in st and "options" in st
                assert len(st["options"]) == 2
                for label, delta, flavor in st["options"]:
                    assert isinstance(label, str) and label
                    assert isinstance(delta, int)
                    assert isinstance(flavor, str) and flavor

    def test_unknown_custom_scp_has_no_scenario(self) -> None:
        assert not scenarios.has("SCP-9999-CUSTOM")
        assert scenarios.stages("SCP-9999-CUSTOM") == []


# ====================== ASCII art ======================
class TestArt:
    def test_specific_art_for_known_scps(self) -> None:
        for num in _KNOWN_SCENARIO_SCPS:
            a = art.for_scp(num)
            assert isinstance(a, str) and a.strip()
            assert "?SCP?" not in a, f"{num} fell through to generic art"

    def test_generic_art_for_unknown(self) -> None:
        a = art.for_scp("SCP-NONEXISTENT")
        assert "?SCP?" in a
        assert "anomalous object" in a

    def test_all_specific_arts_unique(self) -> None:
        arts = [art.for_scp(n) for n in _KNOWN_SCENARIO_SCPS]
        assert len(set(arts)) == len(arts)


# ====================== Branching breach (engine-level) ======================
class TestBranching:
    def test_branching_applies_positive_deltas(self, monkeypatch: pytest.MonkeyPatch) -> None:
        """Picking option 'a' twice for SCP-173 applies +12 then +10 to chance."""
        _silence_breach_ui(monkeypatch, "a")
        s = GameState()
        # 50 + 12 + 10 = 72, within clamp(5..95)
        assert breach_mod._branching(s, "SCP-173", 50) == 72

    def test_branching_applies_negative_deltas(self, monkeypatch: pytest.MonkeyPatch) -> None:
        """Picking option 'b' twice for SCP-173 applies -18 then -22 to chance."""
        _silence_breach_ui(monkeypatch, "b")
        s = GameState()
        # 80 - 18 - 22 = 40
        assert breach_mod._branching(s, "SCP-173", 80) == 40

    def test_branching_clamps_low(self, monkeypatch: pytest.MonkeyPatch) -> None:
        _silence_breach_ui(monkeypatch, "b")
        s = GameState()
        # SCP-096 'b' twice = -28 - 12 = -40; from 30 should clamp to 5
        assert breach_mod._branching(s, "SCP-096", 30) == 5

    def test_branching_clamps_high(self, monkeypatch: pytest.MonkeyPatch) -> None:
        _silence_breach_ui(monkeypatch, "a")
        s = GameState()
        # SCP-682 'a' twice = +10 + +18 = +28; from 90 should clamp to 95
        assert breach_mod._branching(s, "SCP-682", 90) == 95

    def test_branching_improvise_uses_rng(self, monkeypatch: pytest.MonkeyPatch) -> None:
        _silence_breach_ui(monkeypatch, "x")
        monkeypatch.setattr(breach_mod.rng, "chance", lambda p: True)  # good improvise → +8 each
        s = GameState()
        assert breach_mod._branching(s, "SCP-173", 50) == 50 + 8 + 8

    def test_run_breach_branching_resolves_to_contained(self, monkeypatch: pytest.MonkeyPatch) -> None:
        """e2e: run_breach on a scenario SCP with rng.roll=1 -> survived."""
        _silence_breach_ui(monkeypatch, "a")
        monkeypatch.setattr(breach_mod.rng, "roll", lambda lo, hi: 1)  # force survive
        s = GameState()
        result = breach_mod.run_breach(s, {"number": "SCP-173", "name": "Statue", "threat": 3})
        assert result
        assert s.breaches_survived == 1


# ====================== Achievements ======================
class TestAchievements:
    def test_achievements_count_and_ids(self) -> None:
        assert len(achievements.ACHIEVEMENTS) == 8
        ids = {a["id"] for a in achievements.ACHIEVEMENTS}
        assert ids == {
            "first_blood", "survivor", "untouchable", "researcher",
            "containment_specialist", "armed", "archivist", "o5",
        }

    def test_first_blood_unlocks_on_survival(self) -> None:
        s = GameState()
        s.breaches_survived = 1
        newly = achievements.check(s)
        assert "first_blood" in [a["id"] for a in newly]
        assert "first_blood" in s.unlocked_achievements
        with open(SAVE_PATH) as f:
            assert "first_blood" in json.load(f)["unlocked_achievements"]

    def test_untouchable_via_ctx(self) -> None:
        s = GameState()
        s.breaches_survived = 1
        newly = achievements.check(s, None, {"breach_win_no_loadout": True})
        ids = [a["id"] for a in newly]
        assert "untouchable" in ids and "first_blood" in ids

    def test_untouchable_requires_ctx_flag(self) -> None:
        s = GameState()
        s.breaches_survived = 1
        newly = achievements.check(s, None, {"breach_win_no_loadout": False})
        assert "untouchable" not in [a["id"] for a in newly]

    def test_armed_unlocks_at_4_loadout(self) -> None:
        s = GameState()
        s.loadout = ["a", "b", "c", "d"]
        newly = achievements.check(s)
        assert "armed" in [a["id"] for a in newly]

    def test_archivist_unlocks_at_3_archives(self) -> None:
        s = GameState()
        a = Archives.load()
        for i in range(3):
            a.add(f"SCP-{1000+i}", f"TEST_n{i}", "Safe", "", False)
        newly = achievements.check(s, a)
        assert "archivist" in [x["id"] for x in newly]

    def test_lab_coat_at_5_research(self) -> None:
        s = GameState()
        s.completed_research = ["r1", "r2", "r3", "r4", "r5"]
        newly = achievements.check(s)
        assert "researcher" in [a["id"] for a in newly]

    def test_containment_specialist_at_10_ops(self) -> None:
        s = GameState()
        s.completed_ops = [f"op{i}" for i in range(10)]
        newly = achievements.check(s)
        assert "containment_specialist" in [a["id"] for a in newly]

    def test_old_hand_at_10_breaches(self) -> None:
        s = GameState()
        s.breaches_survived = 10
        newly = achievements.check(s)
        ids = [a["id"] for a in newly]
        assert "survivor" in ids and "first_blood" in ids

    def test_o5_at_rank_5(self) -> None:
        s = GameState()
        s.rank_level = 5
        newly = achievements.check(s)
        assert "o5" in [a["id"] for a in newly]

    def test_no_double_unlock(self) -> None:
        s = GameState()
        s.breaches_survived = 1
        assert "first_blood" in [a["id"] for a in achievements.check(s)]
        assert achievements.check(s) == []  # already unlocked


# ====================== Iron-man & New Game ======================
class TestNewGameWipe:
    def _populated_state(self) -> GameState:
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
        return s

    def test_new_game_resets_scalars(self) -> None:
        s = self._populated_state()
        s.new_game(ironman=False)
        assert s.credits == 500
        assert s.research_credits == 200
        assert s.xp == 0
        assert s.rank_level == 0
        assert not s.ironman

    def test_new_game_clears_collections(self) -> None:
        s = self._populated_state()
        s.new_game(ironman=False)
        assert s.completed_research == []
        assert s.owned_equipment == []
        assert s.completed_ops == []
        assert s.loadout == []
        assert s.unlocked_achievements == []

    def test_new_game_resets_breach_stats(self) -> None:
        s = self._populated_state()
        s.new_game(ironman=False)
        assert s.breaches_survived == 0
        assert s.breaches_failed == 0
        assert s.scps_recontained == 0

    def test_new_game_persists_reset(self) -> None:
        s = self._populated_state()
        s.new_game(ironman=False)
        with open(SAVE_PATH) as f:
            d = json.load(f)
        assert d["credits"] == 500
        assert d["unlocked_achievements"] == []
        assert not d["ironman"]

    def test_new_game_enables_ironman_flag(self) -> None:
        s = GameState()
        s.new_game(ironman=True)
        assert s.ironman
        with open(SAVE_PATH) as f:
            assert json.load(f)["ironman"]


class TestIronManBreach:
    def test_ironman_wipe_on_failed_breach(self, monkeypatch: pytest.MonkeyPatch) -> None:
        """A FAILED scenario breach with ironman=True must reset the save."""
        _silence_breach_ui(monkeypatch, "b")  # bad choices
        monkeypatch.setattr(breach_mod.ui, "pause", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.rng, "roll", lambda lo, hi: 100)  # force fail
        s = GameState()
        s.ironman = True
        s.credits = 9999
        s.xp = 2000
        s.rank_level = 3
        s.save()
        result = breach_mod.run_breach(s, {"number": "SCP-173", "name": "Statue", "threat": 3})
        assert result is None  # iron-man wipe path returns None
        with open(SAVE_PATH) as f:
            d = json.load(f)
        assert d["credits"] == 500
        assert d["xp"] == 0
        assert d["rank_level"] == 0
        assert d["ironman"]

    def test_ironman_does_not_wipe_on_success(self, monkeypatch: pytest.MonkeyPatch) -> None:
        _silence_breach_ui(monkeypatch, "a")
        monkeypatch.setattr(breach_mod.rng, "roll", lambda lo, hi: 1)  # force survive
        s = GameState()
        s.ironman = True
        s.credits = 1500
        s.save()
        result = breach_mod.run_breach(s, {"number": "SCP-173", "name": "Statue", "threat": 3})
        assert result
        with open(SAVE_PATH) as f:
            d = json.load(f)
        assert d["ironman"]
        assert d["breaches_survived"] == 1

    def test_header_shows_ironman_marker(self) -> None:
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
        assert "IRON-MAN" in buf.getvalue()


# ====================== Difficulty tiers ======================
class TestDifficulty:
    def test_modifier_table(self) -> None:
        assert breach_mod.DIFFICULTY_MOD == {"Easy": 15, "Normal": 0, "Hard": -15}

    def test_assess_applies_difficulty(self) -> None:
        for diff, expected_mod in (("Easy", 15), ("Normal", 0), ("Hard", -15)):
            s = GameState()
            s.difficulty = diff
            chance, base, rb, lb, dm, _lines = breach_mod._assess(s, 3)
            assert dm == expected_mod
            assert chance == max(10, min(95, base + rb + lb + dm))

    def test_easy_is_higher_than_hard(self) -> None:
        easy = GameState(); easy.difficulty = "Easy"
        hard = GameState(); hard.difficulty = "Hard"
        assert breach_mod._assess(easy, 3)[0] > breach_mod._assess(hard, 3)[0]

    def test_new_game_sets_difficulty_and_persists(self) -> None:
        s = GameState()
        s.new_game(ironman=True, difficulty="Hard")
        assert s.difficulty == "Hard" and s.ironman
        reloaded = GameState.load()
        assert reloaded.difficulty == "Hard" and reloaded.ironman

    def test_default_difficulty_is_normal(self) -> None:
        assert GameState().difficulty == "Normal"

    def test_ironman_wipe_preserves_difficulty(self, monkeypatch: pytest.MonkeyPatch) -> None:
        _silence_breach_ui(monkeypatch, "a")
        monkeypatch.setattr(breach_mod.ui, "pause", lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.rng, "roll", lambda lo, hi: 100)  # force fail
        s = GameState()
        s.new_game(ironman=True, difficulty="Hard")
        res = breach_mod.run_breach(s, {"number": "SCP-173", "name": "x", "threat": 3})
        assert res is None
        assert s.difficulty == "Hard" and s.ironman
        assert s.credits == 500 and s.xp == 0 and s.rank_level == 0


# ====================== Branching key future-proofing ======================
class TestBranchingKeys:
    def test_branching_handles_three_options(self, monkeypatch: pytest.MonkeyPatch) -> None:
        monkeypatch.setattr(scenarios, "stages", lambda num: [
            {"prompt": "p", "options": [
                ("opt a", 5, "fa"), ("opt b", -5, "fb"), ("opt c", 10, "fc")]}
        ])
        monkeypatch.setattr(scenarios, "has", lambda num: True)
        for name in ("panel", "info", "slow_print"):
            monkeypatch.setattr(breach_mod.ui, name, lambda *a, **k: None)
        monkeypatch.setattr(breach_mod.ui, "menu", lambda *a, **k: "c")  # 3rd option
        s = GameState()
        assert breach_mod._branching(s, "SCP-TEST", 50) == 60  # +10 from option c


# ====================== rng.roll bounds ======================
class TestRngBounds:
    def test_roll_raises_on_high_lt_low(self) -> None:
        with pytest.raises(ValueError):
            rng.roll(10, 5)

    def test_roll_equal_bounds_ok(self) -> None:
        assert rng.roll(7, 7) == 7


# ====================== Save on interrupt (CLI) ======================
class TestSaveOnInterrupt:
    """Closing stdin while at the hub causes EOFError → except handler saves."""

    def test_eof_at_hub_saves_state(self) -> None:
        env = {**os.environ, "TERM": "xterm", "COLUMNS": "120", "LINES": "45",
               "PYTHONIOENCODING": "utf-8", "SCP_NO_DELAY": "1"}
        # One newline clears the banner pause; EOF then triggers the handler.
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


# ====================== Site Status briefing ======================
class TestSiteStatus:
    def test_next_objective_no_equipment(self) -> None:
        from game import hub
        s = GameState()
        assert "Store" in hub._next_objective(s)

    def test_next_objective_suggests_research(self) -> None:
        from game import hub
        s = GameState()
        s.owned_equipment = ["flashlight"]
        s.rank_level = 2
        s.research_credits = 9999
        assert "Research" in hub._next_objective(s)

    def test_next_objective_breach_when_loadout_ready(self) -> None:
        from game import hub
        from game import data as gdata
        s = GameState()
        s.owned_equipment = ["flashlight"]
        s.loadout = ["flashlight"]
        s.rank_level = 5
        s.research_credits = 0
        s.completed_research = [p["id"] for p in gdata.RESEARCH]
        s.completed_ops = [o["id"] for o in gdata.OPERATIONS]
        assert "breach" in hub._next_objective(s).lower()

    def test_xp_bar_shows_max_at_top_rank(self) -> None:
        from game import hub
        s = GameState()
        s.rank_level = 5
        assert "MAX RANK" in hub._xp_bar(s)

    def test_xp_bar_has_progress_chars(self) -> None:
        from game import hub
        s = GameState()
        s.xp = 250  # halfway to 500
        bar = hub._xp_bar(s)
        assert "█" in bar and "░" in bar and "250/500" in bar

    def test_briefing_renders_without_clear(self) -> None:
        """site_status(clear=False) prints the briefing + directive, no crash."""
        from io import StringIO
        from rich.console import Console
        from game import ui, hub

        s = GameState()
        a = Archives.load()
        buf = StringIO()
        original = ui.console
        ui.console = Console(file=buf, force_terminal=False, width=160)
        ui.pause = lambda *a_, **k_: None  # skip input
        try:
            hub.site_status(s, a, clear=False)
        finally:
            ui.console = original
        out = buf.getvalue()
        assert "STATUS BRIEFING" in out
        assert "DIRECTIVE" in out

    def test_launch_shows_briefing(self) -> None:
        env = {**os.environ, "TERM": "xterm", "COLUMNS": "120", "LINES": "50",
               "PYTHONIOENCODING": "utf-8", "SCP_NO_DELAY": "1"}
        r = subprocess.run(
            [sys.executable, "-m", "game.main"],
            input="\nq\n", cwd=APP_DIR, env=env,
            capture_output=True, text=True, timeout=15,
        )
        assert r.returncode == 0, r.stderr
        assert "STATUS BRIEFING" in r.stdout
        assert "Recommended next action" in r.stdout


# ====================== CLI: branching breach end-to-end ======================
class TestCliBranchingBreach:
    """Drive main.py via stdin: open Breach menu → SCP-173 → deploy y → a/a."""

    def test_scp173_branching_runs_two_decision_stages(self) -> None:
        env = {**os.environ, "TERM": "xterm", "COLUMNS": "140", "LINES": "60",
               "PYTHONIOENCODING": "utf-8", "SCP_NO_DELAY": "1"}
        # \n banner, 5 breach, 1 SCP-173, y deploy, a/a choices, \n pause, q quit
        stdin = "\n5\n1\ny\na\na\n\nq\n"
        r = subprocess.run(
            [sys.executable, "-m", "game.main"],
            input=stdin, cwd=APP_DIR, env=env,
            capture_output=True, text=True, timeout=20,
        )
        assert r.returncode == 0, f"exit={r.returncode}\n{r.stderr}"
        assert "Traceback" not in r.stderr
        assert "DECISION" in r.stdout
        assert "Survival chance now" in r.stdout
        assert ("BREACH CONTAINED" in r.stdout) or ("CONTAINMENT FAILED" in r.stdout)
