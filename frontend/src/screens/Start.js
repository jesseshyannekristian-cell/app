import React from 'react';
import { View, Image, Pressable, Text, StyleSheet, useWindowDimensions } from 'react-native';
import { useSafeAreaInsets } from 'react-native-safe-area-context';
import { Feather } from '@expo/vector-icons';
import { C, F, MONO } from '../theme';
import { sfx } from '../sound';
import { useGame } from '../store';
import { rankData } from '../logic';

// Native pixel dimensions of the title art — used to map overlay positions
// to fractions of the art itself, so the HUD/menu line up with the artwork
// at any screen size instead of relying on fixed pixel offsets.
const IMG_W = 941;
const IMG_H = 1672;

// Fractional anchor points measured against the artwork (0..1 of image bounds).
const ANCHORS = {
  hudLeftTop: 780 / IMG_H,
  hudLeftX: 150 / IMG_W,
  hudRightTop: 958 / IMG_H,
  hudRightMargin: 150 / IMG_W,
  menuTop: 1120 / IMG_H,
  menuSideMargin: 150 / IMG_W,
  footerTop: 1585 / IMG_H,
  footerSideMargin: 60 / IMG_W,
};

function MenuButton({ icon, label, primary, onPress, testID, compact }) {
  return (
    <Pressable
      testID={testID}
      onPress={() => { sfx.tap(); onPress(); }}
      style={({ pressed }) => [
        st.menuBtn,
        compact && st.menuBtnCompact,
        primary ? st.menuBtnPrimary : st.menuBtnSecondary,
        pressed && { opacity: 0.75 },
      ]}
    >
      <View style={{ flexDirection: 'row', alignItems: 'center' }}>
        <Feather name={icon} size={compact ? 15 : 18} color={primary ? C.cyan : C.dim} style={{ marginRight: compact ? 10 : 14 }} />
        <Text style={[compact ? F.body : F.h2, { color: primary ? C.text : C.dim, letterSpacing: 1, fontWeight: '700' }]}>{label}</Text>
      </View>
      <Feather name="chevron-right" size={compact ? 16 : 20} color={primary ? C.cyan : C.dim} />
    </Pressable>
  );
}

export default function Start({ navigation }) {
  const insets = useSafeAreaInsets();
  const { width: winW, height: winH } = useWindowDimensions();
  const { boot, state } = useGame();

  // Available space inside the safe area — the art + overlay live here.
  const availW = winW;
  const availH = winH - insets.top - insets.bottom;
  const artRatio = IMG_W / IMG_H;
  const availRatio = availW / availH;

  // "contain" math: figure out the art's actual rendered box within the safe area
  // so overlay anchors (fractions of the art) land in the right spot regardless
  // of device aspect ratio, instead of being cropped/misaligned like `cover` would be.
  let artW, artH;
  if (availRatio > artRatio) {
    artH = availH;
    artW = artH * artRatio;
  } else {
    artW = availW;
    artH = artW / artRatio;
  }
  const artLeft = insets.left + (availW - artW) / 2;
  const artTop = insets.top + (availH - artH) / 2;

  const rank = boot && state ? rankData(boot, state.rank_level) : null;
  const clearance = rank ? rank.clearance : 5;
  const title = rank ? rank.title.toUpperCase() : 'OMEGA BLACK';

  // Compact mode: if the safe area is short/wide (small phone in landscape, foldable,
  // or letterboxing eats a lot of vertical room), tighten spacing and text sizes
  // so nothing overlaps or clips off-screen.
  const compact = artH < 560;

  const enter = (route) => navigation.navigate(route);

  return (
    <View style={{ flex: 1, backgroundColor: C.bg }}>
      <Image
        source={require('../Site overseer titlescreen.png')}
        style={{ position: 'absolute', left: artLeft, top: artTop, width: artW, height: artH }}
        resizeMode="contain"
      />

      {/* HUD readout */}
      <View style={[st.hud, { left: artLeft + artW * ANCHORS.hudLeftX, top: artTop + artH * ANCHORS.hudLeftTop }]}>
        <Text style={[st.hudLine, compact && st.hudLineCompact]}>CLEARANCE LEVEL: {clearance}</Text>
        <Text style={[st.hudLine, compact && st.hudLineCompact, { color: C.red, marginTop: 2 }]}>Access Granted.</Text>
        {!compact ? <View style={st.hudBar} /> : null}
      </View>
      <View style={[st.hudRight, { right: artLeft + artW * ANCHORS.hudRightMargin, top: artTop + artH * ANCHORS.hudRightTop }]}>
        <Text style={[st.hudLabel, compact && st.hudLabelCompact]}>USER:</Text>
        <Text style={[st.hudUser, compact && st.hudUserCompact]}>{title}</Text>
        <Text style={[st.hudId, compact && st.hudIdCompact]}>ID: OB-6820-05</Text>
      </View>

      {/* Menu */}
      <View
        style={[
          st.menu,
          {
            left: artLeft + artW * ANCHORS.menuSideMargin,
            right: artLeft + artW * ANCHORS.menuSideMargin,
            top: artTop + artH * ANCHORS.menuTop,
          },
        ]}
      >
        <MenuButton compact={compact} testID="start-game-btn" icon="crosshair" label="START GAME" primary onPress={() => enter('Home')} />
        <MenuButton compact={compact} testID="options-btn" icon="settings" label="OPTIONS" onPress={() => enter('NewGame')} />
        <MenuButton compact={compact} testID="credits-btn" icon="file-text" label="CREDITS" onPress={() => enter('Credits')} />
      </View>

      {/* Footer */}
      <View
        style={[
          st.footer,
          {
            left: artLeft + artW * ANCHORS.footerSideMargin,
            right: artLeft + artW * ANCHORS.footerSideMargin,
            top: Math.min(artTop + artH * ANCHORS.footerTop, winH - insets.bottom - 34),
          },
        ]}
      >
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
  hud: { position: 'absolute' },
  hudLine: { fontFamily: MONO, color: C.red, fontSize: 13, fontWeight: '700', letterSpacing: 1 },
  hudLineCompact: { fontSize: 10 },
  hudBar: { width: 90, height: 3, backgroundColor: C.redDim, marginTop: 6, borderRadius: 2 },
  hudRight: { position: 'absolute', alignItems: 'flex-end' },
  hudLabel: { fontFamily: MONO, color: C.dim, fontSize: 11, letterSpacing: 1 },
  hudLabelCompact: { fontSize: 9 },
  hudUser: { fontFamily: MONO, color: C.red, fontSize: 15, fontWeight: '800', letterSpacing: 1, marginTop: 2 },
  hudUserCompact: { fontSize: 11, marginTop: 1 },
  hudId: { fontFamily: MONO, color: C.red, fontSize: 11, marginTop: 2, opacity: 0.85 },
  hudIdCompact: { fontSize: 9, marginTop: 1 },
  menu: { position: 'absolute' },
  menuBtn: {
    flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between',
    paddingVertical: 16, paddingHorizontal: 18, borderRadius: 8, borderWidth: 1.5,
    marginBottom: 12, backgroundColor: 'rgba(10,10,14,0.72)',
  },
  menuBtnCompact: { paddingVertical: 9, paddingHorizontal: 12, marginBottom: 7, borderRadius: 6 },
  menuBtnPrimary: {
    borderColor: C.cyan, backgroundColor: 'rgba(0,217,255,0.10)',
    shadowColor: C.cyan, shadowOpacity: 0.5, shadowRadius: 10, shadowOffset: { width: 0, height: 0 },
  },
  menuBtnSecondary: { borderColor: C.border },
  footer: {
    position: 'absolute',
    flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between',
  },
  footerTitle: { fontFamily: F.h2.fontFamily, color: C.text, fontSize: 13, fontWeight: '700' },
  footerSub: { fontFamily: MONO, color: C.faint, fontSize: 10, marginTop: 1 },
  footerVersion: { fontFamily: MONO, color: C.faint, fontSize: 10 },
});
