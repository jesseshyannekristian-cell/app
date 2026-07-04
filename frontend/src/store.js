import React, { createContext, useContext, useEffect, useRef, useState } from 'react';
import { getBootstrap, getState, putState, deviceId } from './api';
import { DEFAULT_STATE } from './logic';
import bundledBoot from './bootstrapData.json';

const Ctx = createContext(null);
export const useGame = () => useContext(Ctx);

const LOCAL_SAVE_KEY = 'scp_save_v1';

function readLocalSave() {
  try {
    if (typeof localStorage === 'undefined') return null;
    const raw = localStorage.getItem(LOCAL_SAVE_KEY);
    if (!raw) return null;
    return { ...DEFAULT_STATE, ...JSON.parse(raw) };
  } catch (e) { return null; }
}

function writeLocalSave(s) {
  try {
    if (typeof localStorage === 'undefined') return;
    localStorage.setItem(LOCAL_SAVE_KEY, JSON.stringify(s));
  } catch (e) { /* storage full / unavailable — ignore, in-memory state still works this session */ }
}

export function GameProvider({ children }) {
  const [boot, setBoot] = useState(bundledBoot);
  const [state, setState] = useState(() => readLocalSave() || DEFAULT_STATE);
  const [synced, setSynced] = useState(false);
  const [toast, setToast] = useState(null);
  const id = useRef(deviceId());
  const saveTimer = useRef(null);

  const withRetry = async (fn, tries = 3) => {
    let lastErr;
    for (let i = 0; i < tries; i++) {
      try { return await fn(); }
      catch (e) { lastErr = e; if (i < tries - 1) await new Promise((r) => setTimeout(r, 1000 * 2 ** i)); }
    }
    throw lastErr;
  };

  const load = async () => {
    try { setBoot(await withRetry(getBootstrap)); } catch (e) {}
    try {
      const s = await withRetry(() => getState(id.current));
      if (s && !s.new) {
        const clean = { ...DEFAULT_STATE, ...s };
        delete clean.device_id; delete clean.new;
        setState(clean);
      }
    } catch (e) {}
    setSynced(true);
  };

  useEffect(() => { load(); }, []);

  useEffect(() => {
    writeLocalSave(state);
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
