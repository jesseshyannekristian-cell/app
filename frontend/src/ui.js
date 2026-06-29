import React from 'react';
import { View, Text, Pressable, ScrollView, StyleSheet } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';
import { C, F } from './theme';
import { sfx } from './sound';
import { useGame } from './store';

export function Screen({ title, onBack, children, testID }) {
  const insets = useSafeAreaInsets();
  return (
    <View style={[s.screen, { paddingTop: insets.top }]} testID={testID}>
      {title ? (
        <View style={s.topbar}>
          {onBack ? (
            <Pressable testID="back-btn" onPress={() => { sfx.tap(); onBack(); }} style={s.backBtn}>
              <Text style={[F.mono, { color: C.amber }]}>‹ BACK</Text>
            </Pressable>
          ) : <View style={{ width: 60 }} />}
          <Text style={[F.h2, { flex: 1, textAlign: 'center' }]} numberOfLines={1}>{title}</Text>
          <View style={{ width: 60 }} />
        </View>
      ) : null}
      <StatusStrip />
      <ScrollView contentContainerStyle={s.body} showsVerticalScrollIndicator={false}>
        {children}
        <View style={{ height: 40 }} />
      </ScrollView>
      <Toast />
    </View>
  );
}

export function StatusStrip() {
  const { state, boot } = useGame();
  if (!state || !boot) return null;
  const rank = boot.ranks[Math.min(state.rank_level, boot.ranks.length - 1)];
  return (
    <View style={s.strip} testID="status-strip">
      <Text style={F.small} numberOfLines={1}>
        <Text style={{ color: C.amber }}>{rank.title}</Text>
        <Text style={{ color: C.dim }}>  ·  XP </Text><Text style={{ color: C.cyan }}>{state.xp}</Text>
      </Text>
      <Text style={F.small} numberOfLines={1}>
        <Text style={{ color: C.dim }}>CR </Text><Text style={{ color: C.green }}>{state.credits}</Text>
        <Text style={{ color: C.dim }}>  RC </Text><Text style={{ color: C.cyan }}>{state.research_credits}</Text>
      </Text>
    </View>
  );
}

export function Panel({ title, accent = C.cyan, children, style }) {
  return (
    <View style={[s.panel, { borderColor: accent }, style]}>
      {title ? <Text style={[F.small, { color: accent, marginBottom: 8, letterSpacing: 1 }]}>{title}</Text> : null}
      {children}
    </View>
  );
}

export function Btn({ label, onPress, accent = C.amber, disabled, testID, sub }) {
  return (
    <Pressable
      testID={testID}
      disabled={disabled}
      onPress={() => { if (!disabled) { sfx.confirm(); onPress && onPress(); } }}
      style={({ pressed }) => [
        s.btn,
        { borderColor: disabled ? C.border : accent, opacity: disabled ? 0.4 : 1, backgroundColor: pressed ? C.panelHi : C.panel },
      ]}
    >
      <Text style={[F.mono, { color: disabled ? C.dim : accent, fontWeight: 'bold' }]}>{label}</Text>
      {sub ? <Text style={F.small}>{sub}</Text> : null}
    </Pressable>
  );
}

export function Row({ left, right }) {
  return (
    <View style={s.row}>
      <Text style={[F.small, { flex: 1 }]}>{left}</Text>
      <Text style={[F.mono, { color: C.cyan }]}>{right}</Text>
    </View>
  );
}

export function Tag({ text, color }) {
  return <Text style={[F.small, { color, fontWeight: 'bold' }]}>{text}</Text>;
}

function Toast() {
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
    <View style={[s.toast, { borderColor: toast.ok ? C.green : C.red }]} testID="toast">
      <Text style={[F.mono, { color: toast.ok ? C.green : C.red }]}>{toast.ok ? '✔ ' : '✘ '}{toast.msg}</Text>
    </View>
  );
}

const s = StyleSheet.create({
  screen: { flex: 1, backgroundColor: C.bg },
  topbar: { flexDirection: 'row', alignItems: 'center', paddingHorizontal: 12, paddingVertical: 10, borderBottomWidth: 1, borderColor: C.border },
  backBtn: { width: 60 },
  strip: { flexDirection: 'row', justifyContent: 'space-between', paddingHorizontal: 14, paddingVertical: 8, backgroundColor: C.bg2, borderBottomWidth: 1, borderColor: C.border },
  body: { padding: 14 },
  panel: { borderWidth: 1, borderRadius: 8, padding: 14, marginBottom: 12, backgroundColor: C.panel },
  btn: { borderWidth: 1, borderRadius: 8, paddingVertical: 14, paddingHorizontal: 14, marginBottom: 10, backgroundColor: C.panel },
  row: { flexDirection: 'row', alignItems: 'center', paddingVertical: 4 },
  toast: { position: 'absolute', left: 14, right: 14, bottom: 18, borderWidth: 1, borderRadius: 8, padding: 12, backgroundColor: C.bg2 },
});
