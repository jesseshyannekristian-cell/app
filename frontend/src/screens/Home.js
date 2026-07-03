import React from 'react';
import { View, Text } from 'react-native';
import { Feather } from '@expo/vector-icons';
import { Screen, Card, Btn, Badge, Meter } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { rankData, hasNextRank, nextRank, nextObjective } from '../logic';

function Stat({ icon, label, value, color = C.cyan }) {
  return (
    <View style={{ width: '48%', backgroundColor: C.card, borderWidth: 1, borderColor: C.border, borderRadius: 10, padding: 14, marginBottom: 12 }}>
      <Feather name={icon} size={16} color={color} />
      <Text style={[F.h2, { color, marginTop: 8 }]}>{value}</Text>
      <Text style={[F.label, { marginTop: 2 }]}>{label}</Text>
    </View>
  );
}

export default function Home({ navigation }) {
  const { boot, state } = useGame();

  const rank = rankData(boot, state.rank_level);
  const hasNext = hasNextRank(boot, state);
  const nxt = hasNext ? nextRank(boot, state) : null;
  const lo = rank.xp_required, hi = nxt ? nxt.xp_required : rank.xp_required;
  const frac = hi > lo ? (state.xp - lo) / (hi - lo) : 1;

  return (
    <Screen eyebrow="// SITE-20 OVERSEER" title="TERMINAL" tab="terminal" nav={navigation} testID="home-screen">
      <Card title="Status Briefing" accent={C.red}>
        <Text style={[F.small, { marginBottom: 2 }]}>WELCOME BACK, OVERSEER</Text>
        <View style={{ flexDirection: 'row', alignItems: 'center', justifyContent: 'space-between', marginBottom: 12 }}>
          <Text style={F.cardTitle}>{rank.title}</Text>
          <Badge text={`Clearance L${rank.clearance} · Rank ${rank.level}/${boot.ranks.length - 1}`} color={C.amber} />
        </View>
        <Meter frac={frac} color={C.green} label="XP Progress" value={hasNext ? `${state.xp} / ${hi}` : `${state.xp} · MAX`} />
        <View style={{ flexDirection: 'row', marginTop: 12 }}>
          <Badge text={`Difficulty · ${state.difficulty}`} color={C.cyan} />
          {state.ironman ? <View style={{ marginLeft: 8 }}><Badge text="Iron-Man" color={C.red} /></View> : null}
        </View>
      </Card>

      <Card title="Directive — O5 Command" accent={C.cyan}>
        <Text style={[F.body, { color: C.cyan }]}>{nextObjective(boot, state)}</Text>
      </Card>

      <Text style={[F.label, { marginBottom: 10, marginTop: 4 }]}>SITE METRICS</Text>
      <View style={{ flexDirection: 'row', flexWrap: 'wrap', justifyContent: 'space-between' }}>
        <Stat icon="crosshair" label="Ops Contained" value={`${state.completed_ops.length}/${boot.operations.length}`} color={C.cyan} />
        <Stat icon="shield" label="Breaches Survived" value={state.breaches_survived} color={C.green} />
        <Stat icon="alert-triangle" label="Breaches Failed" value={state.breaches_failed} color={C.red} />
        <Stat icon="award" label="Commendations" value={`${state.unlocked_achievements.length}/${boot.achievements.length}`} color={C.amber} />
      </View>

      <Btn testID="quick-breach" label="Respond to Breach" accent={C.red} onPress={() => navigation.navigate('Breach')} />
      <Btn testID="nav-newgame" label="New Game / Reset" variant="outline" accent={C.dim} onPress={() => navigation.navigate('NewGame')} />

      <Text style={[F.small, { textAlign: 'center', marginTop: 18, color: C.faint }]} testID="cc-attribution">
        SCP content licensed under CC BY-SA 3.0 · scp-wiki.wikidot.com
      </Text>
    </Screen>
  );
}
