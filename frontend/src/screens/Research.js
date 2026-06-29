import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Panel, Btn, Tag } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryResearch, checkAchievements } from '../logic';

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
    if (state.completed_research.includes(p.id)) return <Tag text="DONE" color={C.green} />;
    if (state.rank_level < p.required_rank) return <Tag text={`RANK ${p.required_rank}`} color={C.red} />;
    if (state.research_credits < p.cost) return <Tag text="NEED RC" color={C.amber} />;
    return <Tag text="AVAILABLE" color={C.cyan} />;
  };
  const canDo = (p) => !state.completed_research.includes(p.id) && state.rank_level >= p.required_rank && state.research_credits >= p.cost;

  return (
    <Screen title="RESEARCH DIVISION" onBack={() => navigation.goBack()} testID="research-screen">
      {boot.research.map((p) => (
        <Panel key={p.id} title={`${p.cost} RC · RANK ${p.required_rank}`} accent={C.border}>
          <View style={{ flexDirection: 'row', justifyContent: 'space-between', marginBottom: 4 }}>
            <Text style={[F.body, { color: C.amber, flex: 1 }]}>{p.name}</Text>
            {status(p)}
          </View>
          <Text style={F.small}>{p.objective}</Text>
          <View style={{ height: 8 }} />
          <Btn testID={`research-${p.id}`} label={`AUTHORISE · ${p.cost} RC`} disabled={!canDo(p)} onPress={() => doResearch(p.id)} />
        </Panel>
      ))}
    </Screen>
  );
}
