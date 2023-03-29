import { StyleSheet, View, Image, Pressable, Alert, Text} from 'react-native';
import { useCallback, useState, useEffect} from 'react';
import AppLoading from "expo-app-loading";
import useFonts from './hooks/useFonts';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator} from '@react-navigation/native-stack';
import Index from './Index';
import Main from './Main';
import {Asset} from  "expo-asset"
import * as SplashScreen from 'expo-splash-screen';


const Stack = createNativeStackNavigator();

export default function App() {

  const [appIsReady, setAppIsReady] = useState(false);

  const images = [require("./assets/images/balanza.png"), 
  require("./assets/images/chocolatera.png"),
   require("./assets/images/batidora_mangotext.png"), 
   require("./assets/images/offButton.png"),
    require("./assets/images/hot-chocolate.png")];
  
  function cacheImages(images) {
    return images.map(image => {
      if (typeof image === 'string') {
        return Image.prefetch(image);
      } else {
        return Asset.fromModule(image).downloadAsync();
      }
    });
  }

  const LoadFonts = async () => {
    await useFonts();
  };


  useEffect(() => {
    async function loadResourcesAndDataAsync() {
      try {
        SplashScreen.preventAutoHideAsync();

        const imageAssets = cacheImages(images);
        const fontAssets = LoadFonts();

        await Promise.all([imageAssets, fontAssets]);
      } catch (e) {
        // You might want to provide this error information to an error reporting service
        console.warn(e);
      } finally {
        setAppIsReady(true);
        SplashScreen.hideAsync();
      }
    }

    loadResourcesAndDataAsync();
  }, []);

  if (!appIsReady) {
    return null;
  }

  return (
    <NavigationContainer>
      <Stack.Navigator>
        <Stack.Screen name="Index" component={Index} options={{headerShown: false}}/>
        <Stack.Screen name="Main" component={Main} options={{headerShown: false}}/>
      </Stack.Navigator>
    </NavigationContainer>
  );
}






