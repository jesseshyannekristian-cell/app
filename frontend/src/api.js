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

export async function getBootstrap() {
  const r = await fetch(`${BASE}/api/game/bootstrap`);
  if (!r.ok) throw new Error('bootstrap failed');
  return r.json();
}

export async function getState(id) {
  const r = await fetch(`${BASE}/api/game/state/${id}`);
  if (!r.ok) throw new Error('state load failed');
  return r.json();
}

export async function putState(id, s) {
  const body = { ...s };
  delete body.device_id;
  delete body.new;
  await fetch(`${BASE}/api/game/state/${id}`, {
    method: 'PUT',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(body),
  });
}
