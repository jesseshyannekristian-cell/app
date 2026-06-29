"""Smoke regression tests for the Emergent template backend after lint fixes."""
import os
import pytest
import requests

BASE_URL = os.environ.get('REACT_APP_BACKEND_URL', 'https://scp-foundation-6.preview.emergentagent.com').rstrip('/')


@pytest.fixture
def api_client():
    s = requests.Session()
    s.headers.update({"Content-Type": "application/json"})
    return s


# Root endpoint
class TestRoot:
    def test_root_returns_hello_world(self, api_client):
        r = api_client.get(f"{BASE_URL}/api/", timeout=15)
        assert r.status_code == 200, f"unexpected status {r.status_code}: {r.text}"
        data = r.json()
        assert data == {"message": "Hello World"}


# Status check CRUD
class TestStatus:
    def test_create_status_check(self, api_client):
        payload = {"client_name": "TEST_qa-test"}
        r = api_client.post(f"{BASE_URL}/api/status", json=payload, timeout=15)
        assert r.status_code == 200, f"unexpected status {r.status_code}: {r.text}"
        body = r.json()
        assert "id" in body and isinstance(body["id"], str) and len(body["id"]) > 0
        assert body["client_name"] == "TEST_qa-test"
        assert "timestamp" in body and isinstance(body["timestamp"], str)
        # stash id for next test via class attribute
        TestStatus.created_id = body["id"]
        TestStatus.created_client = body["client_name"]

    def test_list_status_includes_created(self, api_client):
        r = api_client.get(f"{BASE_URL}/api/status", timeout=15)
        assert r.status_code == 200, f"unexpected status {r.status_code}: {r.text}"
        items = r.json()
        assert isinstance(items, list)
        ids = [item.get("id") for item in items]
        assert getattr(TestStatus, "created_id", None) in ids, \
            f"created id {getattr(TestStatus, 'created_id', None)} not present in list"
        # no MongoDB _id leak
        for item in items:
            assert "_id" not in item
