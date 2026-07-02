import React from 'react';
import { View, Text, Pressable, ScrollView, StyleSheet } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';
import { useIsFocused } from '@react-navigation/native';
import { Feather } from '@expo/vector-icons';
import { C, F } from './theme';
import { sfx } from './sound';
import { useGame } from './store';

export const TABS = [
  { key: 'terminal', label: 'TERMINAL', icon: 'terminal', route: 'Home' },
  { key: 'archive', label: 'ARCHIVE', icon: 'archive', route: 'Archives' },
  { key: 'encounters', label: 'ENCOUNTERS', icon: 'crosshair', route: 'Operations' },
  { key: 'armory', label: 'ARMORY', icon: 'box', route: 'Store' },
  { key: 'dossier', label: 'DOSSIER', icon: 'user', route: 'Personnel' },
];

export function Screen({ eyebrow, title, onBack, tab, nav, children, testID }) {
  const insets = useSafeAreaInsets();
  const focused = useIsFocused();
  return (
    <View style={[s.screen, { paddingTop: insets.top }]} testID={testID}>
      <View style={s.header}>
        <View style={{ flexDirection: 'row', alignItems: 'center' }}>
          {onBack ? (
            <Pressable testID="back-btn" onPress={() => { sfx.tap(); onBack(); }} hitSlop={12} style={{ marginRight: 10 }}>
              <Feather name="chevron-left" size={26} color={C.red} />
            </Pressable>
          ) : null}
          <Text style={F.eyebrow} numberOfLines={1}>{(eyebrow || '// SITE-20').toUpperCase()}</Text>
        </View>
        <Text style={[F.h1, { marginTop: 2 }]} numberOfLines={1}>{(title || '').toUpperCase()}</Text>
      </View>
      <View style={s.rule} />
      <CurrencyBar />
      <ScrollView contentContainerStyle={s.body} showsVerticalScrollIndicator={false}>
        {children}
        <View style={{ height: 30 }} />
      </ScrollView>
      {tab && focused ? <TabBar active={tab} nav={nav} insets={insets} /> : null}
      {focused ? <Toast bottom={tab ? 84 + insets.bottom : 24} /> : null}
    </View>
  );
}

function CurrencyBar() {
  const { state, boot } = useGame();
  if (!state || !boot) return null;
  const rank = boot.ranks[Math.min(state.rank_level, boot.ranks.length - 1)];
  return (
    <View style={s.currency} testID="currency-bar">
      <View style={s.chip}>
        <Feather name="shield" size={12} color={C.red} />
        <Text style={[F.mono, { color: C.text, fontSize: 11 }]} numberOfLines={1}>  {rank.title}</Text>
      </View>
      <View style={{ flexDirection: 'row' }}>
        <View style={s.chip}>
          <Feather name="disc" size={12} color={C.amber} />
          <Text style={[F.mono, { color: C.amber, fontSize: 12 }]}>  {state.credits}</Text>
        </View>
        <View style={[s.chip, { marginLeft: 8 }]}>
          <Feather name="zap" size={12} color={C.cyan} />
          <Text style={[F.mono, { color: C.cyan, fontSize: 12 }]}>  {state.research_credits}</Text>
        </View>
      </View>
    </View>
  );
}

function TabBar({ active, nav, insets }) {
  return (
    <View style={[s.tabbar, { paddingBottom: Math.max(insets.bottom, 8) }]} testID="tab-bar">
      <View style={s.tabRule} />
      <View style={{ flexDirection: 'row' }}>
        {TABS.map((t) => {
          const on = t.key === active;
          return (
            <Pressable
              key={t.key}
              testID={`tab-${t.key}`}
              onPress={() => { sfx.tap(); if (nav) nav.navigate(t.route); }}
              style={s.tab}
            >
              <Feather name={t.icon} size={20} color={on ? C.red : C.faint} />
              <Text style={[s.tabLabel, { color: on ? C.red : C.faint }]}>{t.label}</Text>
            </Pressable>
          );
        })}
      </View>
    </View>
  );
}

export function SectionTabs({ items, active, nav }) {
  return (
    <View style={s.sectionTabs} testID="section-tabs">
      {items.map((it) => {
        const on = it.route === active;
        return (
          <Pressable
            key={it.route}
            testID={`section-${it.route.toLowerCase()}`}
            onPress={() => { if (!on) { sfx.tap(); nav.replace ? nav.replace(it.route) : nav.navigate(it.route); } }}
            style={[s.sectionPill, { borderColor: on ? C.red : C.border, backgroundColor: on ? C.redDim : 'transparent' }]}
          >
            <Text style={[F.label, { color: on ? C.red : C.dim, textAlign: 'center' }]} numberOfLines={1}>{it.label.toUpperCase()}</Text>
          </Pressable>
        );
      })}
    </View>
  );
}

export function Card({ title, accent = C.border, children, style, testID, onPress }) {
  const inner = (
    <View style={[s.card, style, { borderWidth: 2, borderColor: accent === C.border ? C.border : accent }]}>
      {title ? <Text style={[F.label, { color: accent === C.border ? C.dim : accent, marginBottom: 8 }]}>{title.toUpperCase()}</Text> : null}
      {children}
    </View>
  );
  if (onPress) return <Pressable testID={testID} onPress={() => { sfx.tap(); onPress(); }}>{inner}</Pressable>;
  return <View testID={testID}>{inner}</View>;
}
// Back-compat alias
export const Panel = Card;

export function Btn({ label, onPress, accent = C.amber, variant = 'solid', disabled, testID, sub }) {
  const solid = variant === 'solid';
  return (
    <Pressable
      testID={testID}
      disabled={disabled}
      onPress={() => { if (!disabled) { sfx.confirm(); onPress && onPress(); } }}
      style={({ pressed }) => [
        s.btn,
        solid
          ? { backgroundColor: disabled ? C.border : accent, opacity: pressed ? 0.85 : 1 }
          : { backgroundColor: pressed ? C.panelHi : 'transparent', borderWidth: 1.5, borderColor: disabled ? C.border : accent },
        disabled && { opacity: 0.45 },
      ]}
    >
      <Text style={[
        F.mono,
        { fontWeight: '800', letterSpacing: 1, fontSize: 13 },
        solid ? { color: disabled ? C.dim : '#0A0A0E' } : { color: disabled ? C.dim : accent },
      ]}>{label.toUpperCase()}</Text>
      {sub ? <Text style={[F.small, { marginTop: 2 }]}>{sub}</Text> : null}
    </Pressable>
  );
}

export function Badge({ text, color = C.green }) {
  return (
    <View style={[s.badge, { borderColor: color }]}>
      <Text style={[F.label, { color, letterSpacing: 1 }]}>{String(text).toUpperCase()}</Text>
    </View>
  );
}
// Back-compat: Tag now renders a Badge
export function Tag({ text, color }) { return <Badge text={text} color={color} />; }

export function Row({ left, right }) {
  return (
    <View style={s.row}>
      <Text style={[F.small, { flex: 1 }]}>{left}</Text>
      <Text style={[F.mono, { color: C.cyan }]}>{right}</Text>
    </View>
  );
}

export function Meter({ frac, color = C.red, label, value }) {
  const pct = Math.max(0, Math.min(1, frac));
  const col = color === 'auto' ? (pct >= 0.6 ? C.green : pct >= 0.35 ? C.amber : C.red) : color;
  return (
    <View style={{ marginVertical: 6 }}>
      {label ? (
        <View style={{ flexDirection: 'row', justifyContent: 'space-between', marginBottom: 4 }}>
          <Text style={F.label}>{label}</Text>
          {value != null ? <Text style={[F.mono, { color: col, fontSize: 12 }]}>{value}</Text> : null}
        </View>
      ) : null}
      <View style={s.meterTrack}>
        <View style={[s.meterFill, { width: `${pct * 100}%`, backgroundColor: col }]} />
      </View>
    </View>
  );
}

function Toast({ bottom = 24 }) {
  const { toast } = useGame();
  const [show, setShow] = React.useState(false);
  React.useEffect(() => {
    if (!toast) return;
    setShow(true);
    const t = setTimeout(() => setShow(false), 2600);
    return () => clearTimeout(t);
  }, [toast]);
  if (!show || !toast) return null;
  return (
    <View style={[s.toast, { bottom, borderLeftColor: toast.ok ? C.green : C.red }]} testID="toast">
      <Feather name={toast.ok ? 'check-circle' : 'alert-triangle'} size={16} color={toast.ok ? C.green : C.red} />
      <Text style={[F.body, { color: C.text, marginLeft: 8, flex: 1 }]}>{toast.msg}</Text>
    </View>
  );
}

const s = StyleSheet.create({
  screen: { flex: 1, backgroundColor: C.bg },
  header: { paddingHorizontal: 18, paddingTop: 14, paddingBottom: 10 },
  rule: { height: 2, backgroundColor: C.red, marginHorizontal: 0, opacity: 1 },
  currency: { flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center', paddingHorizontal: 14, paddingVertical: 8, backgroundColor: C.bg2 },
  chip: { flexDirection: 'row', alignItems: 'center', paddingHorizontal: 10, paddingVertical: 5, borderRadius: 6, backgroundColor: C.panel, borderWidth: 1, borderColor: C.border },
  body: { padding: 16, paddingBottom: 110 },

  sectionTabs: { flexDirection: 'row', marginBottom: 16 },
  sectionPill: { flex: 1, borderWidth: 1, borderRadius: 6, paddingHorizontal: 6, paddingVertical: 10, marginRight: 8, justifyContent: 'center' },

  card: { borderWidth: 2, borderColor: C.border, borderRadius: 8, padding: 16, marginBottom: 12, backgroundColor: C.card },
  btn: { borderRadius: 8, paddingVertical: 14, paddingHorizontal: 16, marginBottom: 10, alignItems: 'center' },
  badge: { borderWidth: 1, borderRadius: 4, paddingHorizontal: 8, paddingVertical: 3, alignSelf: 'flex-start' },
  row: { flexDirection: 'row', alignItems: 'center', paddingVertical: 4 },

  meterTrack: { height: 8, borderRadius: 4, backgroundColor: C.panelHi, overflow: 'hidden' },
  meterFill: { height: '100%', borderRadius: 4 },

  tabbar: { position: 'absolute', left: 0, right: 0, bottom: 0, backgroundColor: C.bg2 },
  tabRule: { height: 2, backgroundColor: C.red, opacity: 1 },
  tab: { flex: 1, alignItems: 'center', paddingTop: 10, paddingBottom: 4 },
  tabLabel: { fontFamily: F.label.fontFamily, fontSize: 9, fontWeight: '700', letterSpacing: 0.5, marginTop: 4 },

  toast: { position: 'absolute', left: 14, right: 14, flexDirection: 'row', alignItems: 'center', borderWidth: 1, borderLeftWidth: 4, borderColor: C.border, borderRadius: 8, padding: 12, backgroundColor: C.panelHi },
});
