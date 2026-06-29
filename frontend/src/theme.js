import { Platform } from 'react-native';

// SCP "Control Room" graphic theme — dark sci-fi dashboard.
export const C = {
  bg: '#08080C',
  bg2: '#0E0E14',
  panel: '#13131B',
  panelHi: '#1B1B26',
  card: '#13131B',
  border: '#24242F',
  borderHi: '#33333F',

  red: '#FF3B47',
  redDim: '#5e1d22',
  amber: '#F5B301',
  amberDim: '#6e5410',
  green: '#3DFF85',
  greenDim: '#1c5e36',
  cyan: '#2DD4D4',

  text: '#F1F1F4',
  dim: '#8A8A96',
  faint: '#56565F',
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
