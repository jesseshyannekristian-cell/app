import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Card, Btn, Badge, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryResearch, checkAchievements } from '../logic';

const ARMORY_TABS = [
  { label: 'Store', route: 'Store' },
  { label: 'Research', route: 'Research' },
  { label: 'Loadout', route: 'Loadout' },
];

export default function Research({ navigation }) {
  const { boot, state, setState, apply } = useGame();
  const doResearch = (id) => {
    const r = tryResearch(boot, state, id);
    apply(r);
    if (r.ok) {
      const { unlocked } = checkAchievements(boot, r.state);
      if (unlocked.length !== r.state.unlocked_achievements.length) setState({ ...r.state, unlocked_achievements: unlocked });
    }
  };
  const status = (p) => {
    if (state.completed_research.includes(p.id)) return <Badge text="Authorised" color={C.green} />;
    if (state.rank_level < p.required_rank) return <Badge text={`Rank ${p.required_rank}`} color={C.red} />;
    if (state.research_credits < p.cost) return <Badge text="Need RC" color={C.amber} />;
    return <Badge text="Available" color={C.cyan} />;
  };
  const canDo = (p) => !state.completed_research.includes(p.id) && state.rank_level >= p.required_rank && state.research_credits >= p.cost;

  return (
    <Screen eyebrow="// R&D DIVISION" title="Research" tab="armory" nav={navigation} testID="research-screen">
      <SectionTabs items={ARMORY_TABS} active="Research" nav={navigation} />
      {boot.research.map((p) => (
        <Card key={p.id} testID={`research-card-${p.id}`}>
          <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start', marginBottom: 4 }}>
            <Text style={[F.cardTitle, { flex: 1, marginRight: 10 }]}>{p.name}</Text>
            {status(p)}
          </View>
          <Text style={[F.label, { color: C.amber, marginBottom: 6 }]}>{p.cost} RC · RANK {p.required_rank}</Text>
          <Text style={[F.small, { marginBottom: 8 }]}>{p.objective}</Text>
          {p.applies_to ? (
            <Text style={[F.small, { color: C.green, marginBottom: 8 }]}>◦ +{p.breach_bonus}% survival chance when responding to {p.applies_to} breaches</Text>
          ) : null}
          <Btn testID={`research-${p.id}`} label={`Authorise · ${p.cost} RC`} disabled={!canDo(p)} onPress={() => doResearch(p.id)} />
        </Card>
      ))}
    </Screen>
  );
}
