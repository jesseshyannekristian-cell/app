import React, { useState } from 'react';
import { View, Text, TextInput, Pressable, StyleSheet } from 'react-native';
import { Screen, Panel, Btn, Tag } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { checkAchievements } from '../logic';
import { sfx } from '../sound';

const CLASSES = ['Safe', 'Euclid', 'Keter', 'Thaumiel', 'Apollyon'];

export default function Archives({ navigation }) {
  const { boot, state, setState, notify } = useGame();
  const [designation, setDesignation] = useState('SCP-');
  const [name, setName] = useState('');
  const [objClass, setObjClass] = useState('Euclid');
  const [desc, setDesc] = useState('');
  const [breachable, setBreachable] = useState(true);

  const nextId = () => (state.custom_scps.reduce((m, s) => Math.max(m, s.id), 0) + 1);

  const file = () => {
    if (!name.trim()) { notify('Name is required.', false); return; }
    sfx.confirm();
    const entry = { id: nextId(), designation: designation.trim() || 'SCP-XXXX', name: name.trim(), class: objClass, description: desc.trim(), is_breachable: breachable };
    const custom = [...state.custom_scps, entry];
    let n = { ...state, custom_scps: custom };
    const { unlocked } = checkAchievements(boot, n);
    n = { ...n, unlocked_achievements: unlocked };
    setState(n);
    notify(`Filed ${entry.designation} — ${entry.name}.`);
    setDesignation('SCP-'); setName(''); setDesc(''); setBreachable(true); setObjClass('Euclid');
  };

  const del = (id) => {
    sfx.tap();
    setState({ ...state, custom_scps: state.custom_scps.filter((s) => s.id !== id) });
    notify('Entry deleted.');
  };

  const toggleBr = (s) => {
    setState({ ...state, custom_scps: state.custom_scps.map((x) => x.id === s.id ? { ...x, is_breachable: !x.is_breachable } : x) });
  };

  return (
    <Screen title="ARCHIVES" onBack={() => navigation.goBack()} testID="archives-screen">
      <Panel title="FILE A NEW SCP" accent={C.amber}>
        <Text style={F.small}>DESIGNATION</Text>
        <TextInput testID="archive-designation" value={designation} onChangeText={setDesignation} style={inp.field} placeholderTextColor={C.dim} />
        <Text style={F.small}>NAME</Text>
        <TextInput testID="archive-name" value={name} onChangeText={setName} placeholder="e.g. The Whispering Lamp" placeholderTextColor={C.dim} style={inp.field} />
        <Text style={F.small}>OBJECT CLASS</Text>
        <View style={{ flexDirection: 'row', flexWrap: 'wrap', marginVertical: 6 }}>
          {CLASSES.map((c) => (
            <Pressable key={c} testID={`class-${c}`} onPress={() => { sfx.tap(); setObjClass(c); }} style={[inp.chip, { borderColor: objClass === c ? C.amber : C.border }]}>
              <Text style={[F.small, { color: objClass === c ? C.amber : C.dim }]}>{c}</Text>
            </Pressable>
          ))}
        </View>
        <Text style={F.small}>CONTAINMENT NOTES</Text>
        <TextInput testID="archive-desc" value={desc} onChangeText={setDesc} multiline placeholder="Describe the anomaly & procedures…" placeholderTextColor={C.dim} style={[inp.field, { height: 80, textAlignVertical: 'top' }]} />
        <Pressable testID="archive-breachable" onPress={() => { sfx.tap(); setBreachable(!breachable); }} style={[inp.chip, { alignSelf: 'flex-start', borderColor: breachable ? C.green : C.border, marginVertical: 6 }]}>
          <Text style={[F.small, { color: breachable ? C.green : C.dim }]}>{breachable ? '☑' : '☐'} Breachable (appears in Breach menu)</Text>
        </Pressable>
        <Btn testID="archive-file" label="FILE SCP REPORT" onPress={file} />
      </Panel>

      <Text style={[F.small, { marginBottom: 8 }]}>CUSTOM SCP DATABASE ({state.custom_scps.length})</Text>
      {state.custom_scps.length === 0 ? (
        <Panel accent={C.border}><Text style={F.small}>No custom SCPs filed yet.</Text></Panel>
      ) : state.custom_scps.map((s) => (
        <Panel key={s.id} accent={C.border}>
          <View style={{ flexDirection: 'row', justifyContent: 'space-between' }}>
            <Text style={[F.body, { color: C.amber, flex: 1 }]}>{s.designation} — {s.name}</Text>
            <Tag text={s.class} color={C.cyan} />
          </View>
          {s.description ? <Text style={F.small}>{s.description}</Text> : null}
          <View style={{ flexDirection: 'row', marginTop: 8 }}>
            <View style={{ flex: 1, marginRight: 6 }}>
              <Btn testID={`archive-toggle-${s.id}`} label={s.is_breachable ? 'BREACHABLE ✓' : 'NOT BREACHABLE'} accent={s.is_breachable ? C.green : C.dim} onPress={() => toggleBr(s)} />
            </View>
            <View style={{ flex: 1, marginLeft: 6 }}>
              <Btn testID={`archive-delete-${s.id}`} label="DELETE" accent={C.red} onPress={() => del(s.id)} />
            </View>
          </View>
        </Panel>
      ))}
    </Screen>
  );
}

const inp = StyleSheet.create({
  field: { fontFamily: F.body.fontFamily, color: C.text, borderWidth: 1, borderColor: C.border, borderRadius: 6, paddingHorizontal: 10, paddingVertical: 8, marginVertical: 6, backgroundColor: '#0c0c08' },
  chip: { borderWidth: 1, borderRadius: 6, paddingHorizontal: 10, paddingVertical: 6, marginRight: 6, marginBottom: 6 },
});
