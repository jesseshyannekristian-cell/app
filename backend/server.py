"""FastAPI backend for the SCP Site Overseer mobile (Expo) app.

Reuses the game data/logic modules under /app/game and serves them to the mobile
client, plus cloud save/load of player state keyed by an anonymous device id.
"""
import os
import sys
from datetime import datetime, timezone

from dotenv import load_dotenv
from fastapi import APIRouter, FastAPI
from fastapi.middleware.cors import CORSMiddleware
from motor.motor_asyncio import AsyncIOMotorClient
from pydantic import BaseModel, Field

load_dotenv()

# Reuse the pure-Python game data + rules already built and tested under /app/game.
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from game import data as gdata  # noqa: E402
from game import achievements as gach  # noqa: E402
from game import scenarios as gscen  # noqa: E402
from game.breach import DIFFICULTY_MOD, LOADOUT_BONUS  # noqa: E402

app = FastAPI(title="SCP Site Overseer API")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

client = AsyncIOMotorClient(os.environ["MONGO_URL"])
db = client[os.environ["DB_NAME"]]

api = APIRouter(prefix="/api")


@api.get("/")
async def root():
    return {"status": "ok", "service": "scp-site-overseer"}


@api.get("/game/bootstrap")
async def bootstrap():
    """All static game data the client needs, derived from the game modules."""
    return {
        "scps": gdata.SCPS,
        "equipment": gdata.EQUIPMENT,
        "research": gdata.RESEARCH,
        "operations": gdata.OPERATIONS,
        "ranks": gdata.RANKS,
        "personnel": gdata.PERSONNEL,
        "scenarios": gscen.SCENARIOS,
        "achievements": gach.ACHIEVEMENTS,
        "difficulty_mod": DIFFICULTY_MOD,
        "loadout_bonus": {k: {"bonus": v[0], "flavor": v[1]} for k, v in LOADOUT_BONUS.items()},
        "starting": {"credits": 500, "research_credits": 200, "loadout_capacity": 4},
    }


DEFAULT_STATE = {
    "credits": 500,
    "research_credits": 200,
    "xp": 0,
    "rank_level": 0,
    "difficulty": "Normal",
    "ironman": False,
    "completed_research": [],
    "owned_equipment": [],
    "completed_ops": [],
    "loadout": [],
    "breaches_survived": 0,
    "breaches_failed": 0,
    "scps_recontained": 0,
    "unlocked_achievements": [],
    "custom_scps": [],
}


class PlayerState(BaseModel):
    credits: int = 500
    research_credits: int = 200
    xp: int = 0
    rank_level: int = 0
    difficulty: str = "Normal"
    ironman: bool = False
    completed_research: list = Field(default_factory=list)
    owned_equipment: list = Field(default_factory=list)
    completed_ops: list = Field(default_factory=list)
    loadout: list = Field(default_factory=list)
    breaches_survived: int = 0
    breaches_failed: int = 0
    scps_recontained: int = 0
    unlocked_achievements: list = Field(default_factory=list)
    custom_scps: list = Field(default_factory=list)


@api.get("/game/state/{device_id}")
async def get_state(device_id: str):
    doc = await db.player_state.find_one({"_id": device_id})
    if not doc:
        return {**DEFAULT_STATE, "device_id": device_id, "new": True}
    doc.pop("_id", None)
    doc.pop("updated_at", None)
    return {**doc, "device_id": device_id, "new": False}


@api.put("/game/state/{device_id}")
async def save_state(device_id: str, state: PlayerState):
    payload = state.model_dump()
    payload["updated_at"] = datetime.now(timezone.utc).isoformat()
    await db.player_state.update_one({"_id": device_id}, {"$set": payload}, upsert=True)
    return {"ok": True, "device_id": device_id}


app.include_router(api)
