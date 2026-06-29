const BASE = process.env.EXPO_PUBLIC_BACKEND_URL
  || (typeof window !== 'undefined' && window.location ? window.location.origin : '');

export function deviceId() {
  try {
    if (typeof localStorage !== 'undefined') {
      let id = localStorage.getItem('scp_device_id');
      if (!id) {
        id = 'dev-' + Math.random().toString(36).slice(2, 12);
        localStorage.setItem('scp_device_id', id);
      }
      return id;
    }
  } catch (e) { /* ignore */ }
  return 'native-device';
}

// Request with an abort-based timeout that covers BOTH the response headers
// AND the body read — so a stalled/buffered body (e.g. proxy cold start) aborts
// and lets the retry logic in store.js recover instead of hanging forever.
async function request(url, opts = {}, timeoutMs = 8000) {
  const ctrl = new AbortController();
  const t = setTimeout(() => ctrl.abort(), timeoutMs);
  try {
    const r = await fetch(url, { ...opts, signal: ctrl.signal });
    if (!r.ok) throw new Error(`HTTP ${r.status}`);
    const text = await r.text();
    return text ? JSON.parse(text) : null;
  } finally {
    clearTimeout(t);
  }
}

export function getBootstrap() {
  return request(`${BASE}/api/game/bootstrap`);
}

export function getState(id) {
  return request(`${BASE}/api/game/state/${id}`);
}

export function putState(id, s) {
  const body = { ...s };
  delete body.device_id;
  delete body.new;
  return request(`${BASE}/api/game/state/${id}`, {
    method: 'PUT',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(body),
  }).catch(() => {});
}
