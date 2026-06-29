import React from 'react';
import { StatusBar } from 'expo-status-bar';
import { NavigationContainer, DarkTheme } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import { SafeAreaProvider } from 'react-native-safe-area-context';
import { GameProvider } from './src/store';
import { C } from './src/theme';

import Home from './src/screens/Home';
import Research from './src/screens/Research';
import Store from './src/screens/Store';
import Operations from './src/screens/Operations';
import Loadout from './src/screens/Loadout';
import Breach from './src/screens/Breach';
import Archives from './src/screens/Archives';
import Achievements from './src/screens/Achievements';
import Personnel from './src/screens/Personnel';
import NewGame from './src/screens/NewGame';

const Stack = createNativeStackNavigator();

const navTheme = {
  ...DarkTheme,
  colors: { ...DarkTheme.colors, background: C.bg, card: C.bg, text: C.text, primary: C.amber, border: C.border },
};

export default function App() {
  return (
    <SafeAreaProvider>
      <GameProvider>
        <NavigationContainer theme={navTheme}>
          <Stack.Navigator screenOptions={{ headerShown: false, contentStyle: { backgroundColor: C.bg } }}>
            <Stack.Screen name="Home" component={Home} />
            <Stack.Screen name="Research" component={Research} />
            <Stack.Screen name="Store" component={Store} />
            <Stack.Screen name="Operations" component={Operations} />
            <Stack.Screen name="Loadout" component={Loadout} />
            <Stack.Screen name="Breach" component={Breach} />
            <Stack.Screen name="Archives" component={Archives} />
            <Stack.Screen name="Achievements" component={Achievements} />
            <Stack.Screen name="Personnel" component={Personnel} />
            <Stack.Screen name="NewGame" component={NewGame} />
          </Stack.Navigator>
        </NavigationContainer>
        <StatusBar style="light" />
      </GameProvider>
    </SafeAreaProvider>
  );
}
