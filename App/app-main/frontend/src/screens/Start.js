import React from 'react';
import { View, Image, Pressable, Text, StyleSheet } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';
import { Feather } from '@expo/vector-icons';
import { C, F, MONO } from '../theme';
import { sfx } from '../sound';
import { useGame } from '../store';
import { rankData } from '../logic';

function MenuButton({ icon, label, primary, onPress, testID }) {
  return (
    <Pressable
      testID={testID}
      onPress={() => { sfx.tap(); onPress(); }}
      style={({ pressed }) => [
        st.menuBtn,
        primary ? st.menuBtnPrimary : st.menuBtnSecondary,
        pressed && { opacity: 0.75 },
      ]}
    >
      <View style={{ flexDirection: 'row', alignItems: 'center' }}>
        <Feather name={icon} size={18} color={primary ? C.cyan : C.dim} style={{ marginRight: 14 }} />
        <Text style={[F.h2, { color: primary ? C.text : C.dim, letterSpacing: 1 }]}>{label}</Text>
      </View>
      <Feather name="chevron-right" size={20} color={primary ? C.cyan : C.dim} />
    </Pressable>
  );
}

export default function Start({ navigation }) {
  const insets = useSafeAreaInsets();
  const { boot, state, synced } = useGame();

  const rank = synced && boot && state ? rankData(boot, state.rank_level) : null;
  const clearance = rank ? rank.level : 5;
  const title = rank ? rank.title.toUpperCase() : 'OMEGA BLACK';

  const enter = (route) => navigation.navigate(route === 'Home' ? 'Home' : route);

  return (
    <View style={{ flex: 1, backgroundColor: C.bg }}>
      <Image
        source={require('../Site overseer titlescreen.png')}
        style={{ width: '100%', height: '100%', resizeMode: 'cover', position: 'absolute' }}
      />

      {/* HUD readout */}
      <View style={[st.hud, { top: insets.top + 210 }]}>
        <Text style={st.hudLine}>CLEARANCE LEVEL: {clearance}</Text>
        <Text style={[st.hudLine, { color: C.red, marginTop: 2 }]}>Access Granted.</Text>
        <View style={st.hudBar} />
      </View>
      <View style={[st.hudRight, { top: insets.top + 300 }]}>
        <Text style={st.hudLabel}>USER:</Text>
        <Text style={st.hudUser}>{title}</Text>
        <Text style={st.hudId}>ID: OB-6820-05</Text>
      </View>

      {/* Menu */}
      <View style={[st.menu, { bottom: insets.bottom + 60 }]}>
        <MenuButton testID="start-game-btn" icon="crosshair" label="START GAME" primary onPress={() => enter('Home')} />
        <MenuButton testID="options-btn" icon="settings" label="OPTIONS" onPress={() => enter('NewGame')} />
        <MenuButton testID="credits-btn" icon="file-text" label="CREDITS" onPress={() => enter('Credits')} />
      </View>

      {/* Footer */}
      <View style={[st.footer, { bottom: insets.bottom + 14 }]}>
        <View style={{ flexDirection: 'row', alignItems: 'center' }}>
          <Feather name="shield" size={14} color={C.dim} style={{ marginRight: 8 }} />
          <View>
            <Text style={st.footerTitle}>SCP Foundation</Text>
            <Text style={st.footerSub}>Secure. Contain. Protect.</Text>
          </View>
        </View>
        <Text style={st.footerVersion}>CC BY-SA · 1.0.0</Text>
      </View>
    </View>
  );
}

const st = StyleSheet.create({
  hud: { position: 'absolute', left: 24 },
  hudLine: { fontFamily: MONO, color: C.red, fontSize: 13, fontWeight: '700', letterSpacing: 1 },
  hudBar: { width: 90, height: 3, backgroundColor: C.redDim, marginTop: 6, borderRadius: 2 },
  hudRight: { position: 'absolute', right: 24, alignItems: 'flex-end' },
  hudLabel: { fontFamily: MONO, color: C.dim, fontSize: 11, letterSpacing: 1 },
  hudUser: { fontFamily: MONO, color: C.red, fontSize: 15, fontWeight: '800', letterSpacing: 1, marginTop: 2 },
  hudId: { fontFamily: MONO, color: C.red, fontSize: 11, marginTop: 2, opacity: 0.85 },
  menu: { position: 'absolute', left: 20, right: 20 },
  menuBtn: {
    flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between',
    paddingVertical: 16, paddingHorizontal: 18, borderRadius: 8, borderWidth: 1.5,
    marginBottom: 12, backgroundColor: 'rgba(10,10,14,0.72)',
  },
  menuBtnPrimary: {
    borderColor: C.cyan, backgroundColor: 'rgba(0,217,255,0.10)',
    shadowColor: C.cyan, shadowOpacity: 0.5, shadowRadius: 10, shadowOffset: { width: 0, height: 0 },
  },
  menuBtnSecondary: { borderColor: C.border },
  footer: {
    position: 'absolute', left: 24, right: 24,
    flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between',
  },
  footerTitle: { fontFamily: F.h2.fontFamily, color: C.text, fontSize: 13, fontWeight: '700' },
  footerSub: { fontFamily: MONO, color: C.faint, fontSize: 10, marginTop: 1 },
  footerVersion: { fontFamily: MONO, color: C.faint, fontSize: 10 },
});
