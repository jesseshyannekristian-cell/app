import { Platform } from 'react-native';

export const C = {
  bg: '#0a0a0a',
  bg2: '#111109',
  panel: '#15140d',
  panelHi: '#1d1b10',
  amber: '#ffb000',
  amberDim: '#7a5a10',
  cyan: '#21d4d4',
  green: '#5dff5d',
  red: '#ff4b4b',
  dim: '#8a8a7a',
  border: '#2c2a1e',
  text: '#e8e6d8',
};

export const MONO = Platform.select({ ios: 'Courier New', android: 'monospace', default: 'monospace' });

export const F = {
  h1: { fontFamily: MONO, color: C.amber, fontSize: 26, fontWeight: 'bold', letterSpacing: 2 },
  h2: { fontFamily: MONO, color: C.amber, fontSize: 18, fontWeight: 'bold', letterSpacing: 1 },
  body: { fontFamily: MONO, color: C.text, fontSize: 14 },
  small: { fontFamily: MONO, color: C.dim, fontSize: 12 },
  mono: { fontFamily: MONO, color: C.text, fontSize: 13 },
};
