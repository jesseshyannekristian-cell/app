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
  // Bundled static config renders instantly — no blocking on the network.
  // Initial player state: hydrate synchronously from the browser's own storage
  // (this device's real save), falling back to a fresh save only if none exists.
  // This is the primary, always-available save path — it does not depend on
  // any backend or database being deployed.
  const [boot, setBoot] = useState(bundledBoot);
  const [state, setState] = useState(() => readLocalSave() || DEFAULT_STATE);
  const [synced, setSynced] = useState(false);
  const [toast, setToast] = useState(null);
  const id = useRef(deviceId());
  const saveTimer = useRef(null);

  // Retry helper for cold-start / proxy timing: 3 tries, exponential backoff (1s, 2s, 4s).
  const withRetry = async (fn, tries = 3) => {
    let lastErr;
