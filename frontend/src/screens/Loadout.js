import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Panel, Btn, Tag } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryToggleLoadout, checkAchievements } from '../logic';

export default function Loadout({ navigation }) {
  const { boot, state, setState, apply, notify } = useGame();
  const owned = boot.equipment.filter((e) => state.owned_equipment.includes(e.id));
  const cap = boot.starting.loadout_capacity;
  const toggle = (id) => {
    const r = tryToggleLoadout(boot, state, id, cap);
    apply(r);
    if (r.ok) {
      const { unlocked } = checkAchievements(boot, r.state);
      if (unlocked.length !== r.state.unlocked_achievements.length) setState({ ...r.state, unlocked_achievements: unlocked });
    }
  };
  return (
    <Screen title="PRE-BREACH LOADOUT" onBack={() => navigation.goBack()} testID="loadout-screen">
      <Panel title={`LOADOUT  ${state.loadout.length}/${cap} SLOTS`} accent={C.amber}>
        <Text style={F.small}>Equipped gear boosts your survival chance during a breach.</Text>
      </Panel>
      {owned.length === 0 ? (
        <Panel accent={C.red}><Text style={F.body}>You own no equipment. Visit the Store first.</Text></Panel>
      ) : owned.map((e) => {
        const on = state.loadout.includes(e.id);
        const b = boot.loadout_bonus[e.id];
        return (
          <Panel key={e.id} accent={on ? C.green : C.border}>
            <View style={{ flexDirection: 'row', justifyContent: 'space-between' }}>
              <Text style={[F.body, { color: C.amber, flex: 1 }]}>{e.name}</Text>
              {on ? <Tag text="● EQUIPPED" color={C.green} /> : <Tag text="—" color={C.dim} />}
            </View>
            {b ? <Text style={[F.small, { color: C.cyan }]}>+{b.bonus}% survival</Text> : <Text style={F.small}>no breach bonus</Text>}
            <View style={{ height: 8 }} />
            <Btn testID={`loadout-${e.id}`} label={on ? 'REMOVE FROM LOADOUT' : 'ADD TO LOADOUT'} accent={on ? C.red : C.amber} onPress={() => toggle(e.id)} />
          </Panel>
        );
      })}
    </Screen>
  );
}
