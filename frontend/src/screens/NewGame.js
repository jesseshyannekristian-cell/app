import React, { useState } from 'react';
import { View, Text, Pressable, StyleSheet } from 'react-native';
import { Feather } from '@expo/vector-icons';
import { Screen, Card, Btn, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { DEFAULT_STATE } from '../logic';
import { sfx } from '../sound';

const DOSSIER_TABS = [
  { label: 'Personnel', route: 'Personnel' },
  { label: 'Awards', route: 'Achievements' },
  { label: 'System', route: 'NewGame' },
];

const DIFFS = [
  ['Easy', '+15% breach survival'],
  ['Normal', 'balanced experience'],
  ['Hard', '-15% breach survival'],
];

function Toggle({ on, label, sub, onPress, testID, color = C.red }) {
  return (
    <Pressable testID={testID} onPress={() => { sfx.tap(); onPress(); }} style={[ng.opt, { borderColor: on ? color : C.border }]}>
      <View style={{ flexDirection: 'row', alignItems: 'center' }}>
        <Feather name={on ? 'check-square' : 'square'} size={18} color={on ? color : C.dim} style={{ marginRight: 10 }} />
        <Text style={[F.body, { color: on ? color : C.text, fontWeight: '700' }]}>{label}</Text>
      </View>
      <Text style={[F.small, { marginTop: 4, marginLeft: 28 }]}>{sub}</Text>
    </Pressable>
  );
}

export default function NewGame({ navigation }) {
  const { state, setState, notify } = useGame();
  const [difficulty, setDifficulty] = useState('Normal');
  const [ironman, setIronman] = useState(false);
  const [wipeArchives, setWipeArchives] = useState(false);

  const start = () => {
    sfx.alarm();
    const n = { ...DEFAULT_STATE, difficulty, ironman };
    if (!wipeArchives) n.custom_scps = state.custom_scps;
    setState(n);
    notify(`New game started — ${difficulty}${ironman ? ' · IRON-MAN' : ''}.`);
    navigation.navigate('Home');
  };

  return (
    <Screen eyebrow="// SYSTEM CONTROL" title="System" tab="dossier" nav={navigation} testID="newgame-screen">
      <SectionTabs items={DOSSIER_TABS} active="NewGame" nav={navigation} />
      <Card title="Warning" accent={C.red}>
        <Text style={F.body}>This wipes your progression (rank, currencies, research, equipment, breach record and achievements). This cannot be undone.</Text>
      </Card>

      <Card title="Difficulty Tier" accent={C.amber}>
        {DIFFS.map(([d, desc]) => (
          <Pressable key={d} testID={`diff-${d}`} onPress={() => { sfx.tap(); setDifficulty(d); }} style={[ng.opt, { borderColor: difficulty === d ? C.amber : C.border }]}>
            <View style={{ flexDirection: 'row', alignItems: 'center' }}>
              <Feather name={difficulty === d ? 'check-circle' : 'circle'} size={18} color={difficulty === d ? C.amber : C.dim} style={{ marginRight: 10 }} />
              <Text style={[F.body, { color: difficulty === d ? C.amber : C.text, fontWeight: '700' }]}>{d}</Text>
            </View>
            <Text style={[F.small, { marginTop: 4, marginLeft: 28 }]}>{desc}</Text>
          </Pressable>
        ))}
      </Card>

      <Toggle testID="toggle-ironman" on={ironman} label="☠ IRON-MAN MODE" sub="A single failed breach permanently wipes your save." onPress={() => setIronman(!ironman)} />
      <Toggle testID="toggle-wipe-archives" on={wipeArchives} label="Delete all custom SCPs" sub={`Currently ${state.custom_scps.length} filed.`} onPress={() => setWipeArchives(!wipeArchives)} />

      <View style={{ height: 6 }} />
      <Btn testID="confirm-newgame" label={`Start New Game — ${difficulty}${ironman ? ' · Iron-Man' : ''}`} accent={C.red} onPress={start} />
    </Screen>
  );
}

const ng = StyleSheet.create({
  opt: { borderWidth: 1, borderRadius: 8, padding: 14, marginBottom: 10, backgroundColor: C.card },
});
