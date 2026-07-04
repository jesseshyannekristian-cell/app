import React from 'react';
import { View, Text } from 'react-native';
import { Screen, Card, Btn, Badge, SectionTabs } from '../ui';
import { C, F } from '../theme';
import { useGame } from '../store';
import { tryBuy, isEquipResearched, checkAchievements } from '../logic';

const ARMORY_TABS = [
  { label: 'Store', route: 'Store' },
  { label: 'Research', route: 'Research' },
  { label: 'Loadout', route: 'Loadout' },
  { label: 'MTF', route: 'Squad' },
];

const DEPT_ORDER = ['Field', 'Research', 'Logistics'];
const DEPT_ACCENT = { Field: C.amber, Research: C.cyan, Logistics: C.green };

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
    if (state.owned_equipment.includes(e.id)) return <Badge text="Owned" color={C.green} />;
    if (e.requires_research && !isEquipResearched(boot, state, e.id)) return <Badge text="Research Req'd" color={C.amber} />;
    if (state.rank_level < e.required_rank) return <Badge text={`Rank ${e.required_rank}`} color={C.red} />;
    if (state.credits < e.cost) return <Badge text="Need CR" color={C.amber} />;
    return <Badge text="Available" color={C.cyan} />;
