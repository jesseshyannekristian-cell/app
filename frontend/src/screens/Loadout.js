import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Card, Btn, Badge, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryToggleLoadout, checkAchievements, effectiveCapacity } from '../logic';

const ARMORY_TABS = [
  { label: 'Store', route: 'Store' },
  { label: 'Research', route: 'Research' },
  { label: 'Loadout', route: 'Loadout' },
  { label: 'MTF', route: 'Squad' },
];

export default function Loadout({ navigation }) {
  const { boot, state, setState, apply } = useGame();
  const owned = boot.equipment.filter((e) => state.owned_equipment.includes(e.id));
  const cap = effectiveCapacity(boot, state);
  const toggle = (id) => {
    const r = tryToggleLoadout(boot, state, id, cap);
    apply(r);
    if (r.ok) {
      const { unlocked } = checkAchievements(boot, r.state);
      if (unlocked.length !== r.state.unlocked_achievements.length) setState({ ...r.state, unlocked_achievements: unlocked });
    }
  };
  return (
    <Screen eyebrow="// FIELD KIT" title="Loadout" tab="armory" nav={navigation} testID="loadout-screen">
      <SectionTabs items={ARMORY_TABS} active="Loadout" nav={navigation} />
      <Card title={`Loadout · ${state.loadout.length}/${cap} Slots`} accent={C.amber}>
        <Text style={F.small}>Equipped gear boosts your survival chance during a breach.</Text>
      </Card>
      {owned.length === 0 ? (
        <Card><Text style={F.body}>You own no equipment. Visit the Store first.</Text></Card>
      ) : owned.map((e) => {
        const on = state.loadout.includes(e.id);
        const b = boot.loadout_bonus[e.id];
        return (
          <Card key={e.id} testID={`loadout-card-${e.id}`}>
            <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start' }}>
              <Text style={[F.cardTitle, { flex: 1, marginRight: 10 }]}>{e.name}</Text>
              {on ? <Badge text="Equipped" color={C.green} /> : <Badge text="Stored" color={C.dim} />}
            </View>
            <Text style={[F.label, { color: b ? C.cyan : (e.loadout_slot_bonus ? C.amber : C.dim), marginVertical: 8 }]}>
              {b ? `+${b.bonus}% SURVIVAL` : e.loadout_slot_bonus ? `+${e.loadout_slot_bonus} LOADOUT SLOT (PASSIVE)` : 'NO BREACH BONUS'}
            </Text>
            <Btn testID={`loadout-${e.id}`} label={on ? 'Remove From Loadout' : 'Add To Loadout'} variant={on ? 'outline' : 'solid'} accent={on ? C.red : C.amber} onPress={() => toggle(e.id)} />
          </Card>
        );
      })}
    </Screen>
  );
}
