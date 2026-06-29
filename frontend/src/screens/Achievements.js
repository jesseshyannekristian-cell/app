import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Panel } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';

export default function Achievements({ navigation }) {
  const { boot, state } = useGame();
  return (
    <Screen title="ACHIEVEMENTS" onBack={() => navigation.goBack()} testID="achievements-screen">
      <Panel title={`UNLOCKED ${state.unlocked_achievements.length}/${boot.achievements.length}`} accent={C.green}>
        <Text style={F.small}>Earn these by progressing through Site-20 operations and breaches.</Text>
      </Panel>
      {boot.achievements.map((a) => {
        const got = state.unlocked_achievements.includes(a.id);
        return (
          <Panel key={a.id} accent={got ? C.green : C.border}>
            <View style={{ flexDirection: 'row', alignItems: 'center' }}>
              <Text style={{ color: got ? C.green : C.dim, fontSize: 18, marginRight: 10 }}>{got ? '★' : '☆'}</Text>
              <View style={{ flex: 1 }}>
                <Text style={[F.body, { color: got ? C.green : C.dim }]}>{a.name}</Text>
                <Text style={F.small}>{a.desc}</Text>
              </View>
            </View>
          </Panel>
        );
      })}
    </Screen>
  );
}
