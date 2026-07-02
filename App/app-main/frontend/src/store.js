import React, { createContext, useContext, useEffect, useRef, useState } from 'react';
import { getBootstrap, getState, putState, deviceId } from './api';
import { DEFAULT_STATE } from './logic';
import bundledBoot from './bootstrapData.json';

const Ctx = createContext(null);
export const useGame = () => useContext(Ctx);

export function GameProvider({ children }) {
  // Bundled static config + default state render instantly — no blocking on the network.
  const [boot, setBoot] = useState(bundledBoot);
  const [state, setState] = useState(DEFAULT_STATE);
  const [synced, setSynced] = useState(false);
  const [toast, setToast] = useState(null);
  const id = useRef(deviceId());
  const saveTimer = useRef(null);

  // Retry helper for cold-start / proxy timing: 3 tries, exponential backoff (1s, 2s, 4s).
  const withRetry = async (fn, tries = 3) => {
    let lastErr;
    for (let i = 0; i < tries; i++) {
      try { return await fn(); }
      catch (e) { lastErr = e; if (i < tries - 1) await new Promise((r) => setTimeout(r, 1000 * 2 ** i)); }
    }
    throw lastErr;
  };

  // Background sync: refresh config + load the player's saved state without blocking the UI.
  const load = async () => {
    try { setBoot(await withRetry(getBootstrap)); } catch (e) { /* keep bundled config */ }
    try {
      const s = await withRetry(() => getState(id.current));
      const clean = { ...DEFAULT_STATE, ...s };
      delete clean.device_id; delete clean.new;
      setState(clean);
    } catch (e) { /* keep default state (offline) */ }
    setSynced(true);
  };

  useEffect(() => { load(); }, []);

  // Debounced cloud save — only after the initial sync settles, to avoid clobbering saved progress.
  useEffect(() => {
    if (!synced) return;
    if (saveTimer.current) clearTimeout(saveTimer.current);
    saveTimer.current = setTimeout(() => { putState(id.current, state); }, 600);
  }, [state, synced]);

  const notify = (msg, ok = true) => { setToast({ msg, ok, t: Date.now() }); };

  const apply = (result) => {
    if (result.state) setState(result.state);
    if (result.msg) notify(result.msg, result.ok);
    return result;
  };

  return (
    <Ctx.Provider value={{ boot, state, setState, synced, toast, notify, apply, reload: load, deviceId: id.current }}>
      {children}
    </Ctx.Provider>
  );
}
