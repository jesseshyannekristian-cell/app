import React from 'react';
import { View, Image, Pressable, Text, Dimensions } from 'react-native';
import { C, F } from '../theme';

const { width, height } = Dimensions.get('window');

export default function Start({ navigation }) {
  const handleEnter = () => {
    navigation.replace('Home');
  };

  return (
    <View style={{ flex: 1, backgroundColor: C.bg, justifyContent: 'center', alignItems: 'center' }}>
      <Image
        source={require('../Site overseer titlescreen.png')}
        style={{
          width: '100%',
          height: '100%',
          resizeMode: 'cover',
          position: 'absolute',
        }}
      />
      <Pressable
        onPress={handleEnter}
        style={{
          paddingHorizontal: 24,
          paddingVertical: 12,
          backgroundColor: C.red,
          borderRadius: 6,
          alignItems: 'center',
          position: 'absolute',
          bottom: 60,
        }}
      >
        <Text style={[F.h2, { color: C.bg, textTransform: 'uppercase', letterSpacing: 2 }]}>Enter Terminal</Text>
      </Pressable>
    </View>
  );
}
