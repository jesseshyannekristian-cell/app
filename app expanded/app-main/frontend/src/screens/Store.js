import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Card, Btn, Badge, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryBuy, isEquipResearched, checkAchievements } from '../logic';

const ARMORY_TABS = [
  { label: 'Store', route: 'Store' },
  { label: 'Research', route: 'Research' },
  { label: 'Loadout', route: 'Loadout' },
];

export default function Store({ navigation }) {
  const { boot, state, setState, apply } = useGame();
  const buy = (id) => {
    const r = tryBuy(boot, state, id);
    apply(r);
    if (r.ok) {
      const { unlocked } = checkAchievements(boot, r.state);
      if (unlocked.length !== r.state.unlocked_achievements.length) setState({ ...r.state, unlocked_achievements: unlocked });
    }
  };
  const statusFor = (e) => {
    if (state.owned_equipment.includes(e.id)) return <Badge text="Owned" color={C.green} />;
    if (e.requires_research && !isEquipResearched(boot, state, e.id)) return <Badge text="Research Req'd" color={C.amber} />;
    if (state.rank_level < e.required_rank) return <Badge text={`Rank ${e.required_rank}`} color={C.red} />;
    if (state.credits < e.cost) return <Badge text="Need CR" color={C.amber} />;
    return <Badge text="Available" color={C.cyan} />;
  };
  const canBuy = (e) => !state.owned_equipment.includes(e.id)
    && (!e.requires_research || isEquipResearched(boot, state, e.id))
    && state.rank_level >= e.required_rank && state.credits >= e.cost;

  return (
    <Screen eyebrow="// REQUISITION" title="Armory" tab="armory" nav={navigation} testID="store-screen">
      <SectionTabs items={ARMORY_TABS} active="Store" nav={navigation} />
      {boot.equipment.map((e) => (
        <Card key={e.id} testID={`equip-card-${e.id}`}>
          <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start', marginBottom: 4 }}>
            <Text style={[F.cardTitle, { flex: 1, marginRight: 10 }]}>{e.name}</Text>
            {statusFor(e)}
          </View>
          <Text style={[F.label, { color: C.amber, marginBottom: 6 }]}>{e.dept.toUpperCase()} · {e.cost} CR</Text>
          <Text style={[F.small, { marginBottom: 8 }]}>{e.description}</Text>
          <Btn testID={`buy-${e.id}`} label={`Purchase · ${e.cost} CR`} disabled={!canBuy(e)} onPress={() => buy(e.id)} />
        </Card>
      ))}
    </Screen>
  );
}
