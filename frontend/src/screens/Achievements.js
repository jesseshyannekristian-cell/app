import React from 'react';
import { View, Text } from 'react-native';
import { Feather } from '@expo/vector-icons';
import { Screen, Card, Badge, SectionTabs, Meter } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';

const DOSSIER_TABS = [
  { label: 'Personnel', route: 'Personnel' },
  { label: 'Awards', route: 'Achievements' },
  { label: 'System', route: 'NewGame' },
];

export default function Achievements({ navigation }) {
  const { boot, state } = useGame();
  const got = state.unlocked_achievements.length;
  const total = boot.achievements.length;
  return (
    <Screen eyebrow="// COMMENDATIONS" title="Awards" tab="dossier" nav={navigation} testID="achievements-screen">
      <SectionTabs items={DOSSIER_TABS} active="Achievements" nav={navigation} />
      <Card title="Commendations Earned" accent={C.green}>
        <Meter frac={total ? got / total : 0} color={C.green} label="Progress" value={`${got} / ${total}`} />
      </Card>
      {boot.achievements.map((a) => {
        const unlocked = state.unlocked_achievements.includes(a.id);
        return (
          <Card key={a.id} testID={`ach-${a.id}`}>
            <View style={{ flexDirection: 'row', alignItems: 'flex-start' }}>
              <Feather name={unlocked ? 'award' : 'lock'} size={20} color={unlocked ? C.green : C.faint} style={{ marginRight: 12, marginTop: 2 }} />
              <View style={{ flex: 1 }}>
                <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start' }}>
                  <Text style={[F.cardTitle, { color: unlocked ? C.text : C.dim, flex: 1, marginRight: 10 }]}>{a.name}</Text>
                  <Badge text={unlocked ? 'Unlocked' : 'Locked'} color={unlocked ? C.green : C.faint} />
                </View>
                <Text style={[F.small, { marginTop: 4 }]}>{a.desc}</Text>
              </View>
            </View>
          </Card>
        );
      })}
    </Screen>
  );
}
