import React, { useEffect, useRef, useState } from 'react';
import { View, Text, Pressable, StyleSheet } from 'react-native';
import { Screen, Panel, Btn, Tag } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { breachAssess, recordBreach, checkAchievements, find } from '../logic';
import { sfx } from '../sound';

const ART = {
  'SCP-173': 'concrete + rebar · do not blink',
  'SCP-096': 'do NOT view its face',
  'SCP-106': 'corrosion · the Old Man phases near',
  'SCP-682': 'hard-to-destroy · acid bath',
  'SCP-049': 'the Plague Doctor seeks a cure',
  'SCP-16829': 'temporal loop · no signal',
};

export default function Breach({ navigation }) {
  const { boot, state, setState, notify } = useGame();
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
    setChance(a.chance);
    setLog([]);
    setOutcome(null);
    setPhase('assess');
  };

  const deploy = () => {
    sfx.confirm();
    setStageIdx(0);
    if (stages.length > 0) setPhase('scenario');
    else setPhase('minigame');
  };

  const pickOption = (opt) => {
    const [, delta, flavor] = opt;
    const nc = Math.max(5, Math.min(95, chance + delta));
    setChance(nc);
    setLog((l) => [...l, { flavor, delta }]);
    if (delta >= 0) sfx.hit(); else sfx.miss();
    if (stageIdx + 1 < stages.length) setStageIdx(stageIdx + 1);
    else setPhase('minigame');
  };

  const resolve = (finalChance) => {
    const survived = (1 + Math.floor(Math.random() * 100)) <= finalChance;
    if (survived) sfx.success(); else sfx.fail();
    let n = recordBreach(boot, state, survived, survived ? 1 : 0);
    if (!survived && state.ironman) {
      // permadeath wipe (preserve difficulty + ironman)
      n = { ...require('../logic').DEFAULT_STATE, difficulty: state.difficulty, ironman: true };
      setState(n);
      setOutcome({ survived: false, wiped: true });
      setPhase('result');
      return;
    }
    const ctx = { breach_win_no_loadout: survived && state.loadout.length === 0 };
    const { unlocked } = checkAchievements(boot, n, ctx);
    n = { ...n, unlocked_achievements: unlocked };
    setState(n);
    setOutcome({ survived, wiped: false });
    setPhase('result');
  };

  return (
    <Screen title="⚠ BREACH RESPONSE" onBack={() => navigation.goBack()} testID="breach-screen">
      {phase === 'select' && (
        <>
          <Text style={[F.small, { marginBottom: 10 }]}>Select a live anomaly to respond to. Record — survived {state.breaches_survived} · failed {state.breaches_failed}.</Text>
          {targets.map((t, i) => (
            <Panel key={t.number + i} accent={C.border}>
              <View style={{ flexDirection: 'row', justifyContent: 'space-between' }}>
                <Text style={[F.body, { color: C.amber, flex: 1 }]} numberOfLines={1}>{t.number} — {t.name}</Text>
                <Tag text={'▲'.repeat(t.threat)} color={C.red} />
              </View>
              <Text style={F.small}>{t.custom ? 'CUSTOM' : 'Site-20'}</Text>
              <View style={{ height: 8 }} />
              <Btn testID={`breach-target-${i}`} label="RESPOND" accent={C.red} onPress={() => start(t)} />
            </Panel>
          ))}
        </>
      )}

      {phase === 'assess' && target && (
        <>
          <Panel title="⚠ CONTAINMENT BREACH" accent={C.red}>
            <Text style={[F.h2, { color: C.red }]}>{target.number}</Text>
            <Text style={[F.body, { marginVertical: 6 }]}>{target.name}</Text>
            <Text style={[F.small, { color: C.amberDim }]}>{ART[target.number] || 'anomalous object loose in the corridors'}</Text>
          </Panel>
          <Panel title="PRE-BREACH ASSESSMENT" accent={C.amber}>
            <ChanceBar chance={chance} />
            <Text style={F.small}>{state.loadout.length ? `Loadout active (${state.loadout.length})` : 'No loadout equipped — nerve only.'}</Text>
          </Panel>
          <Btn testID="deploy-btn" label="DEPLOY INTO THE BREACH" accent={C.red} onPress={deploy} />
          <Btn label="ABORT" accent={C.dim} onPress={() => setPhase('select')} />
        </>
      )}

      {phase === 'scenario' && stages[stageIdx] && (
        <>
          <Panel title={`DECISION ${stageIdx + 1}/${stages.length}`} accent={C.amber}>
            <Text style={F.body}>{stages[stageIdx].prompt}</Text>
          </Panel>
          <ChanceBar chance={chance} />
          {stages[stageIdx].options.map((opt, i) => (
            <Btn key={i} testID={`decision-${i}`} label={opt[0]} accent={C.cyan} onPress={() => pickOption(opt)} />
          ))}
          {log.length ? <Panel accent={C.border}>{log.slice(-3).map((l, i) => (
            <Text key={i} style={[F.small, { color: l.delta >= 0 ? C.green : C.red }]}>◦ {l.flavor}</Text>
          ))}</Panel> : null}
        </>
      )}

      {phase === 'minigame' && (
        <MiniGame chance={chance} setChance={setChance} onDone={resolve} />
      )}

      {phase === 'result' && outcome && (
        <>
          <Panel title={outcome.survived ? '✔ BREACH CONTAINED' : '✘ CONTAINMENT FAILED'} accent={outcome.survived ? C.green : C.red}>
            <Text style={F.body}>
              {outcome.survived
                ? `${target.number} is back behind its door. Hazard pay logged.`
                : outcome.wiped
                  ? 'IRON-MAN: containment failure is fatal. Your save has been wiped.'
                  : `${target.number} overwhelmed the team. You barely reach evac.`}
            </Text>
          </Panel>
          <Btn testID="breach-again" label="BACK TO ANOMALY LIST" onPress={() => setPhase('select')} />
          <Btn label="RETURN TO HUB" accent={C.dim} onPress={() => navigation.goBack()} />
        </>
      )}
    </Screen>
  );
}

function ChanceBar({ chance }) {
  const filled = Math.round((chance / 100) * 20);
  const col = chance >= 60 ? C.green : chance >= 35 ? C.amber : C.red;
  return (
    <View style={{ marginVertical: 8 }}>
      <Text style={F.small}>SURVIVAL CHANCE</Text>
      <Text style={[F.mono, { color: col, fontSize: 16 }]}>{'█'.repeat(filled)}{'░'.repeat(20 - filled)} {chance}%</Text>
    </View>
  );
}

// Tap-timing mini-game: lock the marker inside the target zone, 3 rounds.
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
    if (round >= 3) {
      setDone(true);
      setTimeout(() => onDone(live.current), 500);
    } else {
      setRound(round + 1);
    }
  };

  const markerLeft = `${pos}%`;
  return (
    <>
      <Panel title="STABILIZE CONTAINMENT" accent={C.amber}>
        <Text style={F.small}>Tap LOCK when the marker is inside the lit zone. Round {Math.min(round, 3)}/3.</Text>
        <ChanceBar chance={chance} />
        <View style={mg.track}>
          <View style={[mg.zone, { left: `${ZONE[0]}%`, width: `${ZONE[1] - ZONE[0]}%` }]} />
          <View style={[mg.marker, { left: markerLeft }]} />
        </View>
      </Panel>
      <Btn testID="lock-btn" label={done ? 'RESOLVING…' : 'LOCK'} accent={C.green} disabled={done} onPress={lock} />
    </>
  );
}

const mg = StyleSheet.create({
  track: { height: 26, backgroundColor: '#0c0c08', borderWidth: 1, borderColor: C.border, borderRadius: 6, marginTop: 10, overflow: 'hidden' },
  zone: { position: 'absolute', top: 0, bottom: 0, backgroundColor: 'rgba(93,255,93,0.25)', borderLeftWidth: 1, borderRightWidth: 1, borderColor: C.green },
  marker: { position: 'absolute', top: 0, bottom: 0, width: 4, backgroundColor: C.amber, marginLeft: -2 },
});
