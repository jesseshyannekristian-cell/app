import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Card, Btn, Badge, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryRecruit, tryAssignSquad, squadCapacity, checkAchievements } from '../logic';

const ARMORY_TABS = [
  { label: 'Store', route: 'Store' },
  { label: 'Research', route: 'Research' },
  { label: 'Loadout', route: 'Loadout' },
  { label: 'MTF', route: 'Squad' },
];

export default function Squad({ navigation }) {
  const { boot, state, setState, apply } = useGame();
  const cap = squadCapacity(boot);
  const roster = boot.squad_roster || [];

  const recruit = (id) => {
    const r = tryRecruit(boot, state, id);
    apply(r);
    if (r.ok) {
      const { unlocked } = checkAchievements(boot, r.state);
      if (unlocked.length !== r.state.unlocked_achievements.length) setState({ ...r.state, unlocked_achievements: unlocked });
    }
  };
  const assign = (id) => apply(tryAssignSquad(boot, state, id));

  const status = (op) => {
    if (state.casualties.includes(op.id)) return <Badge text="Decommissioned" color={C.red} />;
    if (state.assigned_squad.includes(op.id)) return <Badge text="Deployed" color={C.green} />;
    if (state.squad.includes(op.id)) return <Badge text="On Standby" color={C.cyan} />;
    if (state.rank_level < op.required_rank) return <Badge text={`Rank ${op.required_rank}`} color={C.red} />;
    return <Badge text="Available" color={C.dim} />;
  };

  return (
    <Screen eyebrow="// TACTICAL RESPONSE" title="Mobile Task Forces" tab="armory" nav={navigation} testID="squad-screen">
      <SectionTabs items={ARMORY_TABS} active="Squad" nav={navigation} />
      <Card title={`Deployed · ${state.assigned_squad.length}/${cap} Slots`} accent={C.amber}>
        <Text style={F.small}>MTF units assigned here join your next breach response and boost survival chance — but face real casualty risk if the breach fails. A decommissioned unit cannot be requisitioned again.</Text>
      </Card>
      {roster.map((op) => {
        const owned = state.squad.includes(op.id);
        const kia = state.casualties.includes(op.id);
        const assigned = state.assigned_squad.includes(op.id);
        const canRecruit = !owned && !kia && state.rank_level >= op.required_rank && state.credits >= op.recruit_cost;
        return (
          <Card key={op.id} testID={`squad-card-${op.id}`} style={kia ? { opacity: 0.5 } : undefined}>
            <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start' }}>
              <Text style={[F.cardTitle, { flex: 1, marginRight: 10 }]}>{op.codename}</Text>
              {status(op)}
            </View>
            <Text style={[F.label, { color: C.dim, marginTop: 2, marginBottom: 6 }]}>{op.role.toUpperCase()}</Text>
            <Text style={[F.small, { marginBottom: 8 }]}>{op.bio}</Text>
            <Text style={[F.small, { color: C.green, marginBottom: 8 }]}>◦ +{op.breach_bonus}% survival chance while deployed</Text>
            {!kia && <Text style={[F.small, { color: C.red, marginBottom: 8 }]}>◦ ~{Math.round(op.kia_risk * 100)}% casualty risk per failed breach while deployed</Text>}
            {kia ? (
              <Text style={[F.small, { color: C.red }]}>Decommissioned after heavy losses. This unit cannot be replaced.</Text>
            ) : owned ? (
              <Btn
                testID={`squad-assign-${op.id}`}
                label={assigned ? 'Stand Down' : 'Deploy'}
                variant={assigned ? 'outline' : 'solid'}
                accent={assigned ? C.red : C.green}
                onPress={() => assign(op.id)}
              />
            ) : (
              <Btn
                testID={`squad-recruit-${op.id}`}
                label={`Requisition · ${op.recruit_cost} CR`}
                disabled={!canRecruit}
                onPress={() => recruit(op.id)}
              />
            )}
          </Card>
        );
      })}
    </Screen>
  );
}
