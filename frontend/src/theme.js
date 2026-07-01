import { Platform } from 'react-native';

// SCP "Control Room" graphic theme — dark sci-fi dashboard.
export const C = {
  bg: '#0A0A0E',
  bg2: '#0F0F15',
  panel: '#14141D',
  panelHi: '#1C1C28',
  card: '#14141D',
  border: '#2A2A38',
  borderHi: '#38383E',

  red: '#FF2D3D',      // Brighter, more vibrant red
  redDim: '#4D1820',
  amber: '#FFD700',    // Pure gold
  amberDim: '#7A6B00',
  green: '#00FF7F',    // Bright spring green
  greenDim: '#1a5e3f',
  cyan: '#00D9FF',     // Brighter cyan

  text: '#FFFFFF',     // Pure white for better contrast
  dim: '#A0A0B0',
  faint: '#606070',
};

export const MONO = Platform.select({ ios: 'Courier New', android: 'monospace', default: 'monospace' });
export const SANS = Platform.select({ ios: 'System', android: 'sans-serif', default: 'system-ui, -apple-system, Segoe UI, Roboto, sans-serif' });

export const F = {
  eyebrow: { fontFamily: MONO, color: C.red, fontSize: 12, fontWeight: '700', letterSpacing: 3 },
  h1: { fontFamily: SANS, color: C.text, fontSize: 30, fontWeight: '800', letterSpacing: 1 },
  h2: { fontFamily: SANS, color: C.text, fontSize: 18, fontWeight: '700', letterSpacing: 0.5 },
  designation: { fontFamily: MONO, color: C.red, fontSize: 14, fontWeight: '700', letterSpacing: 1 },
  cardTitle: { fontFamily: SANS, color: C.text, fontSize: 18, fontWeight: '800', letterSpacing: 0.3 },
  label: { fontFamily: MONO, color: C.dim, fontSize: 11, fontWeight: '700', letterSpacing: 1.5 },
  body: { fontFamily: MONO, color: C.text, fontSize: 13, letterSpacing: 0.3 },
  small: { fontFamily: MONO, color: C.dim, fontSize: 12, letterSpacing: 0.3 },
  mono: { fontFamily: MONO, color: C.text, fontSize: 13, letterSpacing: 0.5 },
};
