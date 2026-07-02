import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Card, Btn, Badge, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryRunOp, opSuccessChance, checkAchievements, find } from '../logic';
import { sfx } from '../sound';

const ENC_TABS = [
  { label: 'Operations', route: 'Operations' },
  { label: 'Breach', route: 'Breach' },
];

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
    if (state.completed_ops.includes(o.id)) return <Badge text="Contained" color={C.green} />;
    if (state.rank_level < o.required_rank) return <Badge text={`Rank ${o.required_rank}`} color={C.red} />;
    return <Badge text={`${opSuccessChance(state, o)}%`} color={C.cyan} />;
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
    <Screen eyebrow="// CONTAINMENT" title="Operations" tab="encounters" nav={navigation} testID="operations-screen">
      <SectionTabs items={ENC_TABS} active="Operations" nav={navigation} />
      <Text style={[F.small, { marginBottom: 12 }]}>Dispatch a team. Success is an RNG roll improved by rank; failure still grants field XP.</Text>
      {boot.operations.map((o) => (
        <Card key={o.id} testID={`op-card-${o.id}`}>
          <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start', marginBottom: 6 }}>
            <Text style={[F.cardTitle, { flex: 1, marginRight: 10 }]} numberOfLines={2}>{o.target}</Text>
            {status(o)}
          </View>
          <Text style={[F.label, { color: C.amber, marginBottom: 6 }]}>RANK {o.required_rank} · {o.reward_credits}CR / {o.reward_research}RC / {o.reward_xp}XP</Text>
          {reqText(o) ? <Text style={[F.small, { marginBottom: 6 }]}>{reqText(o)}</Text> : null}
          <View style={{ height: 6 }} />
          <Btn testID={`op-${o.id}`} label={state.completed_ops.includes(o.id) ? 'Completed' : `Dispatch (${opSuccessChance(state, o)}%)`} disabled={!canRun(o)} onPress={() => run(o.id)} />
        </Card>
      ))}
    </Screen>
  );
}
