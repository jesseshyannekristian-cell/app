import React, { useState } from 'react';
import { View, Text, Pressable, StyleSheet } from 'react-native';
import { Screen, Panel, Btn } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { DEFAULT_STATE } from '../logic';
import { sfx } from '../sound';

const DIFFS = [
  ['Easy', '+15% breach survival'],
  ['Normal', 'balanced experience'],
  ['Hard', '-15% breach survival'],
];

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
    <Screen title="NEW GAME / RESET" onBack={() => navigation.goBack()} testID="newgame-screen">
      <Panel title="WARNING" accent={C.red}>
        <Text style={F.body}>This wipes your progression (rank, currencies, research, equipment, breach record and achievements). This cannot be undone.</Text>
      </Panel>

      <Panel title="DIFFICULTY TIER" accent={C.amber}>
        {DIFFS.map(([d, desc]) => (
          <Pressable key={d} testID={`diff-${d}`} onPress={() => { sfx.tap(); setDifficulty(d); }} style={[ng.opt, { borderColor: difficulty === d ? C.amber : C.border }]}>
            <Text style={[F.body, { color: difficulty === d ? C.amber : C.dim }]}>{difficulty === d ? '◉' : '○'} {d}</Text>
            <Text style={F.small}>{desc}</Text>
          </Pressable>
        ))}
      </Panel>

      <Pressable testID="toggle-ironman" onPress={() => { sfx.tap(); setIronman(!ironman); }} style={[ng.opt, { borderColor: ironman ? C.red : C.border }]}>
        <Text style={[F.body, { color: ironman ? C.red : C.dim }]}>{ironman ? '☑' : '☐'} ☠ IRON-MAN MODE</Text>
        <Text style={F.small}>A single failed breach permanently wipes your save.</Text>
      </Pressable>
      <Pressable testID="toggle-wipe-archives" onPress={() => { sfx.tap(); setWipeArchives(!wipeArchives); }} style={[ng.opt, { borderColor: wipeArchives ? C.red : C.border }]}>
        <Text style={[F.body, { color: wipeArchives ? C.red : C.dim }]}>{wipeArchives ? '☑' : '☐'} Also delete all custom SCPs</Text>
        <Text style={F.small}>Currently {state.custom_scps.length} filed.</Text>
      </Pressable>

      <View style={{ height: 6 }} />
      <Btn testID="confirm-newgame" label={`START NEW GAME — ${difficulty}${ironman ? ' · IRON-MAN' : ''}`} accent={C.red} onPress={start} />
      <Btn label="CANCEL" accent={C.dim} onPress={() => navigation.goBack()} />
    </Screen>
  );
}

const ng = StyleSheet.create({
  opt: { borderWidth: 1, borderRadius: 8, padding: 12, marginBottom: 10, backgroundColor: C.panel },
});
