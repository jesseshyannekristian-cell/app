import React, { useState } from 'react';
import { View, Text, Pressable } from 'react-native';
import { Screen, Panel } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { sfx } from '../sound';

export default function Personnel({ navigation }) {
  const { boot } = useGame();
  const [open, setOpen] = useState(null);
  return (
    <Screen title="SITE-20 PERSONNEL" onBack={() => navigation.goBack()} testID="personnel-screen">
      {boot.personnel.map((p, i) => {
        const isOpen = open === i;
        return (
          <Pressable key={p.file_no} testID={`dossier-${i}`} onPress={() => { sfx.tap(); setOpen(isOpen ? null : i); }}>
            <Panel title={`${p.clearance}`} accent={isOpen ? C.amber : C.border}>
              <Text style={[F.h2, { color: C.amber }]}>{p.codename}</Text>
              <Text style={[F.small, { marginBottom: 4 }]}>{p.position}</Text>
              {isOpen && (
                <View style={{ marginTop: 6 }}>
                  <Text style={F.small}>File {p.file_no} · Status {p.status}</Text>
                  <Text style={[F.small, { marginTop: 4 }]}>Nationality: {p.nationality}</Text>
                  <Text style={[F.small, { marginBottom: 4 }]}>Anomalous trait: {p.anomalous_trait}</Text>
                  <Text style={[F.body, { marginTop: 6 }]}>{p.bio}</Text>
                  <Text style={[F.small, { color: C.cyan, marginTop: 8, fontStyle: 'italic' }]}>{p.o5_notes}</Text>
                </View>
              )}
              {!isOpen && <Text style={[F.small, { color: C.cyan }]}>tap to open dossier ▾</Text>}
            </Panel>
          </Pressable>
        );
      })}
    </Screen>
  );
}
