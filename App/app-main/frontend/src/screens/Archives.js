import React, { useState } from 'react';
import { View, Text, TextInput, Pressable, StyleSheet } from 'react-native';
import { Feather } from '@expo/vector-icons';
import { Screen, Card, Btn, Badge, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { checkAchievements } from '../logic';
import { sfx } from '../sound';

const CLASSES = ['Safe', 'Euclid', 'Keter', 'Thaumiel', 'Apollyon'];
const CLASS_COLOR = { Safe: C.green, Euclid: C.amber, Keter: C.red, Thaumiel: C.cyan, Apollyon: C.red };

const ARCHIVE_TABS = [
  { label: 'Custom', route: '__custom' },
  { label: 'Catalog', route: '__catalog' },
];

export default function Archives({ navigation }) {
  const { boot, state, setState, notify } = useGame();
  const [view, setView] = useState('custom');
  const [showForm, setShowForm] = useState(false);
  const [editId, setEditId] = useState(null);
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
    let n = { ...state, custom_scps: [...state.custom_scps, entry] };
    const { unlocked } = checkAchievements(boot, n);
    n = { ...n, unlocked_achievements: unlocked };
    setState(n);
    notify(`Filed ${entry.designation} — ${entry.name}.`);
    setDesignation('SCP-'); setName(''); setDesc(''); setBreachable(true); setObjClass('Euclid'); setShowForm(false);
  };
  const del = (id) => { sfx.tap(); setState({ ...state, custom_scps: state.custom_scps.filter((s) => s.id !== id) }); setEditId(null); notify('Entry deleted.'); };
  const toggleBr = (s) => setState({ ...state, custom_scps: state.custom_scps.map((x) => x.id === s.id ? { ...x, is_breachable: !x.is_breachable } : x) });

  const SectionRow = (
    <View style={s.sectionTabs}>
      {ARCHIVE_TABS.map((it) => {
        const key = it.route.replace('__', '');
        const on = key === view;
        return (
          <Pressable key={it.route} testID={`archive-tab-${key}`} onPress={() => { sfx.tap(); setView(key); }}
            style={[s.pill, { borderColor: on ? C.red : C.border, backgroundColor: on ? C.redDim : 'transparent' }]}>
            <Text style={[F.label, { color: on ? C.red : C.dim, textAlign: 'center' }]}>{it.label.toUpperCase()}</Text>
          </Pressable>
        );
      })}
    </View>
  );

  return (
    <Screen eyebrow="// FOUNDATION ARCHIVE" title="Database" tab="archive" nav={navigation} testID="archives-screen">
      {SectionRow}

      {view === 'custom' && (
        <>
          <Btn testID="archive-new" label={showForm ? '✕ Cancel' : '+ Create New SCP'} accent={showForm ? C.dim : C.amber} variant={showForm ? 'outline' : 'solid'} onPress={() => setShowForm(!showForm)} />

          {showForm && (
            <Card title="File A New SCP" accent={C.amber} testID="archive-form">
              <Text style={F.label}>DESIGNATION</Text>
              <TextInput testID="archive-designation" value={designation} onChangeText={setDesignation} style={s.field} placeholderTextColor={C.faint} />
              <Text style={F.label}>NAME</Text>
              <TextInput testID="archive-name" value={name} onChangeText={setName} placeholder="e.g. The Whispering Lamp" placeholderTextColor={C.faint} style={s.field} />
              <Text style={F.label}>OBJECT CLASS</Text>
              <View style={{ flexDirection: 'row', flexWrap: 'wrap', marginVertical: 6 }}>
                {CLASSES.map((c) => (
                  <Pressable key={c} testID={`class-${c}`} onPress={() => { sfx.tap(); setObjClass(c); }} style={[s.chip, { borderColor: objClass === c ? CLASS_COLOR[c] : C.border }]}>
                    <Text style={[F.label, { color: objClass === c ? CLASS_COLOR[c] : C.dim }]}>{c.toUpperCase()}</Text>
                  </Pressable>
                ))}
              </View>
              <Text style={F.label}>CONTAINMENT NOTES</Text>
              <TextInput testID="archive-desc" value={desc} onChangeText={setDesc} multiline placeholder="Describe the anomaly & procedures…" placeholderTextColor={C.faint} style={[s.field, { height: 84, textAlignVertical: 'top' }]} />
              <Pressable testID="archive-breachable" onPress={() => { sfx.tap(); setBreachable(!breachable); }} style={[s.chip, { alignSelf: 'flex-start', borderColor: breachable ? C.green : C.border, marginVertical: 8 }]}>
                <View style={{ flexDirection: 'row', alignItems: 'center' }}>
                  <Feather name={breachable ? 'check-square' : 'square'} size={14} color={breachable ? C.green : C.dim} />
                  <Text style={[F.label, { color: breachable ? C.green : C.dim, marginLeft: 6 }]}>BREACHABLE</Text>
                </View>
              </Pressable>
              <Btn testID="archive-file" label="File SCP Report" onPress={file} />
            </Card>
          )}

          <Text style={[F.label, { marginBottom: 10, marginTop: 4 }]}>CUSTOM SCP DATABASE · {state.custom_scps.length}</Text>
          {state.custom_scps.length === 0 ? (
            <Card><Text style={F.small}>No custom SCPs filed yet. Tap “Create New SCP”.</Text></Card>
          ) : state.custom_scps.map((sc) => {
            const open = editId === sc.id;
            return (
              <Card key={sc.id} testID={`scp-card-${sc.id}`} onPress={() => { sfx.tap(); setEditId(open ? null : sc.id); }}>
                <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start' }}>
                  <Text style={F.designation}>{sc.designation}</Text>
                  <Badge text={sc.is_breachable ? 'Active' : 'Contained'} color={sc.is_breachable ? C.green : C.amber} />
                </View>
                <Text style={[F.cardTitle, { marginTop: 6 }]}>{sc.name}</Text>
                {sc.description ? <Text style={[F.small, { marginTop: 4 }]} numberOfLines={open ? 0 : 2}>{sc.description}</Text> : null}
                <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center', marginTop: 10 }}>
                  <Text style={[F.label, { color: CLASS_COLOR[sc.class] || C.dim }]}>CLASS · {String(sc.class).toUpperCase()}</Text>
                  <View style={{ flexDirection: 'row', alignItems: 'center' }}>
                    <Text style={[F.label, { color: C.red }]}>{open ? 'CLOSE' : 'TAP TO EDIT'} </Text>
                    <Feather name={open ? 'chevron-up' : 'chevron-right'} size={14} color={C.red} />
                  </View>
                </View>
                {open && (
                  <View style={{ flexDirection: 'row', marginTop: 12, borderTopWidth: 1, borderTopColor: C.border, paddingTop: 12 }}>
                    <View style={{ flex: 1, marginRight: 6 }}>
                      <Btn testID={`archive-toggle-${sc.id}`} label={sc.is_breachable ? 'Set Contained' : 'Set Breachable'} variant="outline" accent={sc.is_breachable ? C.amber : C.green} onPress={() => toggleBr(sc)} />
                    </View>
                    <View style={{ flex: 1, marginLeft: 6 }}>
                      <Btn testID={`archive-delete-${sc.id}`} label="Delete" variant="outline" accent={C.red} onPress={() => del(sc.id)} />
                    </View>
                  </View>
                )}
              </Card>
            );
          })}
        </>
      )}

      {view === 'catalog' && (
        <>
          <Text style={[F.label, { marginBottom: 10 }]}>SITE-20 CATALOG · {boot.scps.length}</Text>
          {boot.scps.map((sc) => (
            <Card key={sc.number} testID={`catalog-${sc.number}`}>
              <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start' }}>
                <Text style={F.designation}>{sc.number}</Text>
                <Badge text={sc.breachable ? 'Breachable' : 'Stable'} color={sc.breachable ? C.red : C.green} />
              </View>
              <Text style={[F.cardTitle, { marginTop: 6 }]}>{sc.name}</Text>
              <View style={{ flexDirection: 'row', alignItems: 'center', marginTop: 8 }}>
                <Text style={[F.label, { color: C.red }]}>THREAT </Text>
                {Array.from({ length: sc.threat || 0 }).map((_, k) => <Feather key={k} name="alert-triangle" size={11} color={C.red} style={{ marginRight: 2 }} />)}
              </View>
            </Card>
          ))}
        </>
      )}
    </Screen>
  );
}

const s = StyleSheet.create({
  sectionTabs: { flexDirection: 'row', marginBottom: 16 },
  pill: { flex: 1, borderWidth: 1, borderRadius: 6, paddingVertical: 10, marginRight: 8, justifyContent: 'center' },
  field: { fontFamily: F.mono.fontFamily, color: C.text, fontSize: 13, borderWidth: 1, borderColor: C.border, borderRadius: 8, paddingHorizontal: 12, paddingVertical: 10, marginTop: 6, marginBottom: 10, backgroundColor: C.bg },
  chip: { borderWidth: 1, borderRadius: 6, paddingHorizontal: 10, paddingVertical: 6, marginRight: 6, marginBottom: 6 },
});
