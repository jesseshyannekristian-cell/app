"""JSON-backed Archives — player-authored custom SCP dossiers."""
import json
import os

ARCHIVE_DIR = os.path.join(os.path.dirname(os.path.dirname(__file__)), "saves")
ARCHIVE_PATH = os.path.join(ARCHIVE_DIR, "archives.json")


class Archives:
    def __init__(self):
        self.custom_scps = []

    @classmethod
    def load(cls):
        a = cls()
        if os.path.exists(ARCHIVE_PATH):
            with open(ARCHIVE_PATH, encoding="utf-8") as f:
                a.custom_scps = json.load(f).get("custom_scps", [])
        return a

    def save(self):
        os.makedirs(ARCHIVE_DIR, exist_ok=True)
        with open(ARCHIVE_PATH, "w", encoding="utf-8") as f:
            json.dump({"custom_scps": self.custom_scps}, f, indent=2)

    def _next_id(self):
        return (max((s["id"] for s in self.custom_scps), default=0) + 1)

    def add(self, designation, name, object_class, description, breachable):
        entry = {
            "id": self._next_id(),
            "designation": designation,
            "name": name,
            "class": object_class,
            "description": description,
            "is_breachable": breachable,
        }
        self.custom_scps.append(entry)
        self.save()
        return entry

    def update(self, entry_id, **fields):
        for s in self.custom_scps:
            if s["id"] == entry_id:
                s.update(fields)
                self.save()
                return s
        return None

    def delete(self, entry_id):
        before = len(self.custom_scps)
        self.custom_scps = [s for s in self.custom_scps if s["id"] != entry_id]
        self.save()
        return len(self.custom_scps) < before

    def get(self, entry_id):
        return next((s for s in self.custom_scps if s["id"] == entry_id), None)

    def breachable(self):
        return [s for s in self.custom_scps if s.get("is_breachable")]
