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
  { label: 'Squad', route: 'Squad' },
];

const DEPT_ORDER = ['Field', 'Research', 'Logistics'];
const DEPT_ACCENT = { Field: C.amber, Research: C.cyan, Logistics: C.green };

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

  const depts = [
    ...DEPT_ORDER.filter((d) => boot.equipment.some((e) => e.dept === d)),
    ...[...new Set(boot.equipment.map((e) => e.dept))].filter((d) => !DEPT_ORDER.includes(d)),
  ];

  return (
    <Screen eyebrow="// REQUISITION" title="Armory" tab="armory" nav={navigation} testID="store-screen">
      <SectionTabs items={ARMORY_TABS} active="Store" nav={navigation} />
      {depts.map((dept) => (
        <View key={dept}>
          <Text style={[F.h2, { color: DEPT_ACCENT[dept] || C.text, marginTop: 14, marginBottom: 6, letterSpacing: 1 }]}>
            {dept.toUpperCase()}
          </Text>
          {boot.equipment.filter((e) => e.dept === dept).map((e) => (
            <Card key={e.id} testID={`equip-card-${e.id}`} accent={DEPT_ACCENT[dept] || C.border}>
              <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start', marginBottom: 4 }}>
                <Text style={[F.cardTitle, { flex: 1, marginRight: 10 }]}>{e.name}</Text>
                {statusFor(e)}
              </View>
              <View style={{ flexDirection: 'row', alignItems: 'center', marginBottom: 6 }}>
                <Text style={[F.label, { color: C.amber }]}>{e.dept.toUpperCase()} · {e.cost} CR</Text>
                {e.loadout_slot_bonus ? (
                  <Text style={[F.label, { color: C.green, marginLeft: 8 }]}>+{e.loadout_slot_bonus} SLOT</Text>
                ) : null}
              </View>
              <Text style={[F.small, { marginBottom: 8 }]}>{e.description}</Text>
              <Btn testID={`buy-${e.id}`} label={`Purchase · ${e.cost} CR`} disabled={!canBuy(e)} onPress={() => buy(e.id)} />
            </Card>
          ))}
        </View>
      ))}
    </Screen>
  );
}

