import React from 'react';
import { View, Text, Linking, Pressable } from 'react-native';
import { Feather } from '@expo/vector-icons';
import { Screen, Card, Badge } from '../ui';
import { C, F } from '../theme';

function LinkRow({ label, url }) {
  return (
    <Pressable onPress={() => Linking.openURL(url)} style={{ flexDirection: 'row', alignItems: 'center', paddingVertical: 8 }}>
      <Feather name="external-link" size={14} color={C.cyan} style={{ marginRight: 8 }} />
      <Text style={[F.body, { color: C.cyan, flex: 1 }]}>{label}</Text>
    </Pressable>
  );
}

export default function Credits({ navigation }) {
  return (
    <Screen eyebrow="// ATTRIBUTION" title="Credits" onBack={() => navigation.goBack()} testID="credits-screen">
      <Card title="FOUNDATION — SCP Site Overseer" accent={C.amber}>
        <Text style={F.body}>
          A non-official, fan-made game. Not affiliated with or endorsed by the SCP Foundation or its authors.
        </Text>
        <View style={{ flexDirection: 'row', marginTop: 10 }}>
          <Badge text="CC BY-SA 3.0" color={C.amber} />
        </View>
      </Card>

      <Card title="License" accent={C.cyan}>
        <Text style={F.body}>
          This project's source code and content are licensed under the Creative Commons
          Attribution-ShareAlike 3.0 Unported License. Any derivative work must remain under the
          same license and credit the SCP Foundation community.
        </Text>
        <LinkRow label="creativecommons.org/licenses/by-sa/3.0" url="https://creativecommons.org/licenses/by-sa/3.0/" />
      </Card>

      <Card title="SCP Foundation Community" accent={C.green}>
        <Text style={[F.body, { marginBottom: 8 }]}>
          All anomaly concepts, designations, and lore referenced in this game are the creative work
          of the SCP Foundation collaborative writing community, used under CC BY-SA 3.0.
        </Text>
        <LinkRow label="scp-wiki.wikidot.com" url="https://scp-wiki.wikidot.com/" />
        <LinkRow label="Licensing Guide" url="https://scp-wiki.wikidot.com/licensing-guide" />
      </Card>

      <Card title="Original Work" accent={C.red}>
        <Text style={F.body}>
          Game framework, economy, rank progression, containment-operation logic, breach scenarios,
          achievements, and all original art in this project are original work, licensed CC BY-SA 3.0.
        </Text>
      </Card>

      <Card title="Trademarks" accent={C.dim}>
        <Text style={F.small}>
          "SCP Foundation" is a trademark of its respective owners. Use here is nominative/descriptive
          and does not imply affiliation or endorsement.
        </Text>
      </Card>
    </Screen>
  );
}
