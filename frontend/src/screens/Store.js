import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Panel, Btn, Tag } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryBuy, isEquipResearched, checkAchievements } from '../logic';

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
    if (state.owned_equipment.includes(e.id)) return <Tag text="OWNED" color={C.green} />;
    if (e.requires_research && !isEquipResearched(boot, state, e.id)) return <Tag text="RESEARCH REQ'D" color={C.amber} />;
    if (state.rank_level < e.required_rank) return <Tag text={`RANK ${e.required_rank}`} color={C.red} />;
    if (state.credits < e.cost) return <Tag text="NEED CR" color={C.amber} />;
    return <Tag text="BUY" color={C.cyan} />;
  };
  const canBuy = (e) => !state.owned_equipment.includes(e.id)
    && (!e.requires_research || isEquipResearched(boot, state, e.id))
    && state.rank_level >= e.required_rank && state.credits >= e.cost;

  return (
    <Screen title="REQUISITION STORE" onBack={() => navigation.goBack()} testID="store-screen">
      {boot.equipment.map((e) => (
        <Panel key={e.id} title={`${e.dept.toUpperCase()} · ${e.cost} CR`} accent={C.border}>
          <View style={{ flexDirection: 'row', justifyContent: 'space-between', marginBottom: 4 }}>
            <Text style={[F.body, { color: C.amber, flex: 1 }]}>{e.name}</Text>
            {statusFor(e)}
          </View>
          <Text style={F.small}>{e.description}</Text>
          <View style={{ height: 8 }} />
          <Btn testID={`buy-${e.id}`} label={`PURCHASE · ${e.cost} CR`} disabled={!canBuy(e)} onPress={() => buy(e.id)} />
        </Panel>
      ))}
    </Screen>
  );
}
