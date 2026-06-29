import React, { useState } from 'react';
import { View, Text, Pressable } from 'react-native';
import { Feather } from '@expo/vector-icons';
import { Screen, Card, Badge, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { sfx } from '../sound';

const DOSSIER_TABS = [
  { label: 'Personnel', route: 'Personnel' },
  { label: 'Awards', route: 'Achievements' },
  { label: 'System', route: 'NewGame' },
];

export default function Personnel({ navigation }) {
  const { boot } = useGame();
  const [open, setOpen] = useState(null);
  return (
    <Screen eyebrow="// SITE-20 STAFF" title="Dossier" tab="dossier" nav={navigation} testID="personnel-screen">
      <SectionTabs items={DOSSIER_TABS} active="Personnel" nav={navigation} />
      {boot.personnel.map((p, i) => {
        const isOpen = open === i;
        return (
          <Card key={p.file_no} testID={`dossier-${i}`} onPress={() => { sfx.tap(); setOpen(isOpen ? null : i); }}>
            <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start' }}>
              <Text style={F.designation}>FILE {p.file_no}</Text>
              <Badge text={p.clearance} color={C.amber} />
            </View>
            <Text style={[F.cardTitle, { marginTop: 6 }]}>{p.codename}</Text>
            <Text style={[F.small, { marginTop: 2 }]}>{p.position}</Text>
            {isOpen ? (
              <View style={{ marginTop: 12, borderTopWidth: 1, borderTopColor: C.border, paddingTop: 12 }}>
                <Text style={F.small}>STATUS · {p.status}</Text>
                <Text style={[F.small, { marginTop: 4 }]}>NATIONALITY · {p.nationality}</Text>
                <Text style={[F.small, { marginTop: 4 }]}>ANOMALOUS TRAIT · {p.anomalous_trait}</Text>
                <Text style={[F.body, { marginTop: 10, color: C.text }]}>{p.bio}</Text>
                <Text style={[F.small, { color: C.cyan, marginTop: 10, fontStyle: 'italic' }]}>{p.o5_notes}</Text>
              </View>
            ) : (
              <View style={{ flexDirection: 'row', alignItems: 'center', marginTop: 10 }}>
                <Text style={[F.label, { color: C.red }]}>TAP TO OPEN DOSSIER </Text>
                <Feather name="chevron-right" size={14} color={C.red} />
              </View>
            )}
          </Card>
        );
      })}
    </Screen>
  );
}
