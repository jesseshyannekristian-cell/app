import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Panel, Btn } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { rankData, hasNextRank, nextRank, nextObjective } from '../logic';

const NAV = [
  ['Research', 'Research', C.cyan],
  ['Store', 'Store', C.cyan],
  ['Operations', 'Operations', C.cyan],
  ['Loadout', 'Loadout', C.amber],
  ['Breach', 'Breach', C.red],
  ['Archives', 'Archives', C.amber],
  ['Personnel', 'Personnel', C.cyan],
  ['Achievements', 'Achievements', C.green],
];

export default function Home({ navigation }) {
  const { boot, state, error } = useGame();
  if (error) return <Screen title="SITE-20"><Panel title="CONNECTION" accent={C.red}><Text style={F.body}>Could not reach Site-20 server.{'\n'}{error}</Text></Panel></Screen>;
  if (!boot || !state) return <Screen title="SITE-20"><Panel title="BOOTING"><Text style={F.body}>Establishing uplink to Site-20…</Text></Panel></Screen>;

  const rank = rankData(boot, state.rank_level);
  const hasNext = hasNextRank(boot, state);
  const nxt = hasNext ? nextRank(boot, state) : null;
  const lo = rank.xp_required, hi = nxt ? nxt.xp_required : rank.xp_required;
  const frac = hi > lo ? Math.max(0, Math.min(1, (state.xp - lo) / (hi - lo))) : 1;
  const filled = Math.round(frac * 18);
  const bar = '█'.repeat(filled) + '░'.repeat(18 - filled);

  return (
    <Screen title="◆ SITE-20 OVERSEER ◆" testID="home-screen">
      <Panel title="SITE-20 STATUS BRIEFING" accent={C.amber}>
        <Text style={[F.h2, { marginBottom: 6 }]}>Welcome back,</Text>
        <Text style={[F.body, { color: C.amber, marginBottom: 10 }]}>{rank.title}  (L{rank.level})</Text>
        <Text style={F.small}>XP PROGRESS</Text>
        <Text style={[F.mono, { color: C.cyan, marginBottom: 8 }]}>{bar}  {hasNext ? `${state.xp}/${hi}` : `${state.xp} · MAX`}</Text>
        <Text style={F.small}>DIFFICULTY  <Text style={{ color: C.text }}>{state.difficulty}</Text>{state.ironman ? <Text style={{ color: C.red }}>  · ☠ IRON-MAN</Text> : null}</Text>
        <View style={{ height: 10 }} />
        <Text style={F.small}>Operations contained  <Text style={{ color: C.cyan }}>{state.completed_ops.length}/{boot.operations.length}</Text></Text>
        <Text style={F.small}>Breaches survived/failed  <Text style={{ color: C.cyan }}>{state.breaches_survived} / {state.breaches_failed}</Text></Text>
        <Text style={F.small}>SCPs re-contained  <Text style={{ color: C.cyan }}>{state.scps_recontained}</Text></Text>
        <Text style={F.small}>Achievements  <Text style={{ color: C.green }}>{state.unlocked_achievements.length}/{boot.achievements.length}</Text></Text>
      </Panel>

      <Panel title="DIRECTIVE — O5 COMMAND" accent={C.cyan}>
        <Text style={[F.body, { color: C.cyan }]}>{nextObjective(boot, state)}</Text>
      </Panel>

      <Text style={[F.small, { marginBottom: 8, marginTop: 4 }]}>SITE OVERSEER HUB</Text>
      <View style={{ flexDirection: 'row', flexWrap: 'wrap', justifyContent: 'space-between' }}>
        {NAV.map(([label, route, accent]) => (
          <View key={route} style={{ width: '48%' }}>
            <Btn testID={`nav-${route.toLowerCase()}`} label={label} accent={accent} onPress={() => navigation.navigate(route)} />
          </View>
        ))}
      </View>
      <Btn testID="nav-newgame" label="NEW GAME / RESET" accent={C.red} onPress={() => navigation.navigate('NewGame')} />
    </Screen>
  );
}
