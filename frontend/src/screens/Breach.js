import React, { useEffect, useRef, useState } from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { Feather } from '@expo/vector-icons';
import { Screen, Card, Btn, Badge, Meter, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { breachAssess, recordBreach, checkAchievements } from '../logic';
import { sfx } from '../sound';

const ENC_TABS = [
  { label: 'Operations', route: 'Operations' },
  { label: 'Breach', route: 'Breach' },
];

const ART = {
  'SCP-173': 'concrete + rebar · do not blink',
  'SCP-096': 'do NOT view its face',
  'SCP-106': 'corrosion · the Old Man phases near',
  'SCP-682': 'hard-to-destroy · acid bath',
  'SCP-049': 'the Plague Doctor seeks a cure',
  'SCP-16829': 'temporal loop · no signal',
};

export default function Breach({ navigation }) {
  const { boot, state, setState } = useGame();
  const [phase, setPhase] = useState('select');
  const [target, setTarget] = useState(null);
  const [chance, setChance] = useState(0);
  const [stageIdx, setStageIdx] = useState(0);
  const [log, setLog] = useState([]);
  const [outcome, setOutcome] = useState(null);

  const targets = [
    ...boot.scps.filter((s) => s.breachable).map((s) => ({ number: s.number, name: s.name, threat: s.threat, custom: false })),
    ...state.custom_scps.filter((c) => c.is_breachable).map((c) => ({ number: c.designation, name: c.name, threat: 3, custom: true })),
  ];
  const stages = target ? (boot.scenarios[target.number] || []) : [];

  const start = (t) => {
    sfx.alarm();
    setTarget(t);
    const a = breachAssess(boot, state, t.threat);
    setChance(a.chance); setLog([]); setOutcome(null); setPhase('assess');
  };
  const deploy = () => { sfx.confirm(); setStageIdx(0); setPhase(stages.length ? 'scenario' : 'minigame'); };
  const pickOption = (opt) => {
    const [, delta, flavor] = opt;
    setChance(Math.max(5, Math.min(95, chance + delta)));
    setLog((l) => [...l, { flavor, delta }]);
    if (delta >= 0) sfx.hit(); else sfx.miss();
    if (stageIdx + 1 < stages.length) setStageIdx(stageIdx + 1); else setPhase('minigame');
  };
  const resolve = (finalChance) => {
    const survived = (1 + Math.floor(Math.random() * 100)) <= finalChance;
    if (survived) sfx.success(); else sfx.fail();
    let n = recordBreach(boot, state, survived, survived ? 1 : 0);
    if (!survived && state.ironman) {
      n = { ...require('../logic').DEFAULT_STATE, difficulty: state.difficulty, ironman: true };
      setState(n); setOutcome({ survived: false, wiped: true }); setPhase('result'); return;
    }
    const ctx = { breach_win_no_loadout: survived && state.loadout.length === 0 };
    const { unlocked } = checkAchievements(boot, n, ctx);
    n = { ...n, unlocked_achievements: unlocked };
    setState(n); setOutcome({ survived, wiped: false }); setPhase('result');
  };

  return (
    <Screen eyebrow="// BREACH RESPONSE" title="Breach" tab="encounters" nav={navigation} testID="breach-screen">
      {phase === 'select' && (
        <>
          <SectionTabs items={ENC_TABS} active="Breach" nav={navigation} />
          <Text style={[F.small, { marginBottom: 12 }]}>Select a live anomaly to respond to. Record — survived {state.breaches_survived} · failed {state.breaches_failed}.</Text>
          {targets.map((t, i) => (
            <Card key={t.number + i} testID={`breach-card-${i}`}>
              <View style={{ flexDirection: 'row', justifyContent: 'space-between', alignItems: 'flex-start' }}>
                <Text style={F.designation}>{t.number}</Text>
                <Badge text={t.custom ? 'Custom' : 'Site-20'} color={t.custom ? C.cyan : C.dim} />
              </View>
              <Text style={[F.cardTitle, { marginTop: 6 }]} numberOfLines={1}>{t.name}</Text>
              <View style={{ flexDirection: 'row', alignItems: 'center', marginVertical: 8 }}>
                <Text style={[F.label, { color: C.red }]}>THREAT </Text>
                {Array.from({ length: t.threat }).map((_, k) => <Feather key={k} name="alert-triangle" size={12} color={C.red} style={{ marginRight: 2 }} />)}
              </View>
              <Btn testID={`breach-target-${i}`} label="Respond" accent={C.red} onPress={() => start(t)} />
            </Card>
          ))}
        </>
      )}

      {phase === 'assess' && target && (
        <>
          <Card title="⚠ Containment Breach" accent={C.red}>
            <Text style={[F.designation, { fontSize: 20 }]}>{target.number}</Text>
            <Text style={[F.cardTitle, { marginVertical: 6 }]}>{target.name}</Text>
            <Text style={[F.small, { color: C.amber }]}>{ART[target.number] || 'anomalous object loose in the corridors'}</Text>
          </Card>
          <Card title="Pre-Breach Assessment" accent={C.amber}>
            <Meter frac={chance / 100} color="auto" label="Survival Chance" value={`${chance}%`} />
            <Text style={[F.small, { marginTop: 6 }]}>{state.loadout.length ? `Loadout active (${state.loadout.length})` : 'No loadout equipped — nerve only.'}</Text>
          </Card>
          <Btn testID="deploy-btn" label="Deploy Into The Breach" accent={C.red} onPress={deploy} />
          <Btn label="Abort" variant="outline" accent={C.dim} onPress={() => setPhase('select')} />
        </>
      )}

      {phase === 'scenario' && stages[stageIdx] && (
        <>
          <Card title={`Decision ${stageIdx + 1}/${stages.length}`} accent={C.amber}>
            <Text style={F.body}>{stages[stageIdx].prompt}</Text>
          </Card>
          <Card><Meter frac={chance / 100} color="auto" label="Survival Chance" value={`${chance}%`} /></Card>
          {stages[stageIdx].options.map((opt, i) => (
            <Btn key={i} testID={`decision-${i}`} label={opt[0]} variant="outline" accent={C.cyan} onPress={() => pickOption(opt)} />
          ))}
          {log.length ? (
            <Card>{log.slice(-3).map((l, i) => (
              <Text key={i} style={[F.small, { color: l.delta >= 0 ? C.green : C.red }]}>◦ {l.flavor}</Text>
            ))}</Card>
          ) : null}
        </>
      )}

      {phase === 'minigame' && <MiniGame chance={chance} setChance={setChance} onDone={resolve} />}

      {phase === 'result' && outcome && (
        <>
          <Card title={outcome.survived ? '✔ Breach Contained' : '✘ Containment Failed'} accent={outcome.survived ? C.green : C.red}>
            <Text style={F.body}>
              {outcome.survived
                ? `${target.number} is back behind its door. Hazard pay logged.`
                : outcome.wiped
                  ? 'IRON-MAN: containment failure is fatal. Your save has been wiped.'
                  : `${target.number} overwhelmed the team. You barely reach evac.`}
            </Text>
          </Card>
          <Btn testID="breach-again" label="Back To Anomaly List" onPress={() => setPhase('select')} />
        </>
      )}
    </Screen>
  );
}

function MiniGame({ chance, setChance, onDone }) {
  const [pos, setPos] = useState(0);
  const [round, setRound] = useState(1);
  const [done, setDone] = useState(false);
  const dir = useRef(1);
  const live = useRef(chance);
  const ZONE = [38, 62];

  useEffect(() => {
    const t = setInterval(() => {
      setPos((p) => {
        let np = p + dir.current * 4;
        if (np >= 100) { np = 100; dir.current = -1; }
        if (np <= 0) { np = 0; dir.current = 1; }
        return np;
      });
    }, 28);
    return () => clearInterval(t);
  }, []);

  const lock = () => {
    if (done) return;
    const hit = pos >= ZONE[0] && pos <= ZONE[1];
    live.current = Math.max(5, Math.min(95, live.current + (hit ? 6 : -6)));
    setChance(live.current);
    if (hit) sfx.hit(); else sfx.miss();
    if (round >= 3) { setDone(true); setTimeout(() => onDone(live.current), 500); }
    else setRound(round + 1);
  };

  return (
    <>
      <Card title="Stabilize Containment" accent={C.amber}>
        <Text style={F.small}>Tap LOCK when the marker is inside the lit zone. Round {Math.min(round, 3)}/3.</Text>
        <Meter frac={chance / 100} color="auto" label="Survival Chance" value={`${chance}%`} />
        <View style={mg.track}>
          <View style={[mg.zone, { left: `${ZONE[0]}%`, width: `${ZONE[1] - ZONE[0]}%` }]} />
          <View style={[mg.marker, { left: `${pos}%` }]} />
        </View>
      </Card>
      <Btn testID="lock-btn" label={done ? 'Resolving…' : 'Lock'} accent={C.green} disabled={done} onPress={lock} />
    </>
  );
}

const mg = StyleSheet.create({
  track: { height: 28, backgroundColor: C.bg, borderWidth: 1, borderColor: C.border, borderRadius: 6, marginTop: 12, overflow: 'hidden' },
  zone: { position: 'absolute', top: 0, bottom: 0, backgroundColor: 'rgba(61,255,133,0.22)', borderLeftWidth: 1, borderRightWidth: 1, borderColor: C.green },
  marker: { position: 'absolute', top: 0, bottom: 0, width: 4, backgroundColor: C.amber, marginLeft: -2 },
});
