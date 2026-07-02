// Lightweight SFX via the Web Audio API (works in the web preview).
// On native it no-ops gracefully (v1 — native builds can add expo-av assets later).
import { Platform } from 'react-native';

let ctx = null;
function tone(freq, dur, type = 'square', vol = 0.05) {
  if (Platform.OS !== 'web' || typeof window === 'undefined') return;
  try {
    const AC = window.AudioContext || window.webkitAudioContext;
    if (!AC) return;
    ctx = ctx || new AC();
    const o = ctx.createOscillator();
    const g = ctx.createGain();
    o.type = type;
    o.frequency.value = freq;
    g.gain.value = vol;
    o.connect(g);
    g.connect(ctx.destination);
    const t = ctx.currentTime;
    o.start(t);
    g.gain.exponentialRampToValueAtTime(0.0001, t + dur);
    o.stop(t + dur);
  } catch (e) {
    /* ignore */
  }
}

export const sfx = {
  tap: () => tone(420, 0.05, 'square', 0.035),
  confirm: () => tone(660, 0.09),
  success: () => { tone(523, 0.1); setTimeout(() => tone(784, 0.18), 90); },
  fail: () => tone(150, 0.3, 'sawtooth', 0.06),
  alarm: () => { tone(300, 0.18, 'sawtooth', 0.06); setTimeout(() => tone(280, 0.18, 'sawtooth', 0.06), 240); },
  hit: () => tone(900, 0.05, 'square', 0.05),
  miss: () => tone(140, 0.12, 'sawtooth', 0.05),
};
