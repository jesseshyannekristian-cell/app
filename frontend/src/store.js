import React, { createContext, useContext, useEffect, useRef, useState } from 'react';
import { getBootstrap, getState, putState, deviceId } from './api';
import { DEFAULT_STATE } from './logic';

const Ctx = createContext(null);
export const useGame = () => useContext(Ctx);

export function GameProvider({ children }) {
  const [boot, setBoot] = useState(null);
  const [state, setState] = useState(null);
  const [error, setError] = useState(null);
  const [toast, setToast] = useState(null);
  const id = useRef(deviceId());
  const saveTimer = useRef(null);

  useEffect(() => {
    (async () => {
      try {
        const b = await getBootstrap();
        setBoot(b);
        const s = await getState(id.current);
        const clean = { ...DEFAULT_STATE, ...s };
        delete clean.device_id; delete clean.new;
        setState(clean);
      } catch (e) {
        setError(String(e));
      }
    })();
  }, []);

  // Debounced cloud save whenever state changes.
  useEffect(() => {
    if (!state) return;
    if (saveTimer.current) clearTimeout(saveTimer.current);
    saveTimer.current = setTimeout(() => { putState(id.current, state).catch(() => {}); }, 600);
  }, [state]);

  const notify = (msg, ok = true) => { setToast({ msg, ok, t: Date.now() }); };

  // Apply a logic result {ok, msg, state?} to context and toast the message.
  const apply = (result, okSound, failSound) => {
    if (result.state) setState(result.state);
    if (result.msg) notify(result.msg, result.ok);
    return result;
  };

  return (
    <Ctx.Provider value={{ boot, state, setState, error, toast, notify, apply, deviceId: id.current }}>
      {children}
    </Ctx.Provider>
  );
}
