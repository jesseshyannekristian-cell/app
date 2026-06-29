import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Panel, Btn, Tag } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryRunOp, opSuccessChance, checkAchievements, find } from '../logic';
import { sfx } from '../sound';

export default function Operations({ navigation }) {
  const { boot, state, setState, apply } = useGame();
  const run = (id) => {
    const r = tryRunOp(boot, state, id);
    if (r.success) sfx.success(); else sfx.fail();
    apply(r);
    if (r.state) {
      const { unlocked } = checkAchievements(boot, r.state);
      if (unlocked.length !== r.state.unlocked_achievements.length) setState({ ...r.state, unlocked_achievements: unlocked });
    }
  };
  const status = (o) => {
    if (state.completed_ops.includes(o.id)) return <Tag text="DONE" color={C.green} />;
    if (state.rank_level < o.required_rank) return <Tag text={`RANK ${o.required_rank}`} color={C.red} />;
    return <Tag text={`${opSuccessChance(state, o)}%`} color={C.cyan} />;
  };
  const canRun = (o) => {
    if (state.completed_ops.includes(o.id) || state.rank_level < o.required_rank) return false;
    if (o.required_research && !state.completed_research.includes(o.required_research)) return false;
    if (o.required_equipment && !state.owned_equipment.includes(o.required_equipment)) return false;
    return true;
  };
  const reqText = (o) => {
    const parts = [];
    if (o.required_research) parts.push(`Procedure: ${o.required_research}`);
    if (o.required_equipment) { const e = find(boot.equipment, 'id', o.required_equipment); parts.push(`Gear: ${e ? e.name : o.required_equipment}`); }
    return parts.join('  ·  ');
  };

  return (
    <Screen title="CONTAINMENT OPS" onBack={() => navigation.goBack()} testID="operations-screen">
      <Text style={[F.small, { marginBottom: 10 }]}>Dispatch a team. Success is an RNG roll improved by rank; failure still grants field XP.</Text>
      {boot.operations.map((o) => (
        <Panel key={o.id} title={`RANK ${o.required_rank} · ${o.reward_credits}cr/${o.reward_research}RC/${o.reward_xp}XP`} accent={C.border}>
          <View style={{ flexDirection: 'row', justifyContent: 'space-between', marginBottom: 4 }}>
            <Text style={[F.body, { color: C.amber, flex: 1 }]} numberOfLines={2}>{o.target}</Text>
            {status(o)}
          </View>
          {reqText(o) ? <Text style={[F.small, { color: C.amberDim }]}>{reqText(o)}</Text> : null}
          <View style={{ height: 8 }} />
          <Btn testID={`op-${o.id}`} label={state.completed_ops.includes(o.id) ? 'COMPLETED' : `DISPATCH (${opSuccessChance(state, o)}%)`} disabled={!canRun(o)} onPress={() => run(o.id)} />
        </Panel>
      ))}
    </Screen>
  );
}
