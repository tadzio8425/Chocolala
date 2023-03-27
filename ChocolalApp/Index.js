import { StyleSheet, View, Image, Pressable, Alert, Text} from 'react-native';
import { useCallback, useState } from 'react';
import AppLoading from "expo-app-loading";
import useFonts from './hooks/useFonts';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';

export const ESP32IP = 'http://192.168.1.4:8181';


export default function Index({navigation}){
    const [selected, setSelected] = useState(false);
    return(
        
    <View style={styles.container}>
    <View style={{width:121, height:121}}>
      
    <Image
      style={styles.image}
      source={require("./assets/images/hot-chocolate.png")}
    />
    </View>

    <Text style={styles.title}>Chocolala</Text>

    <Pressable 
    onPressIn = {() => {
      setSelected(!selected);
   }}

   onPressOut = {() => {
    setSelected(!selected);      
    connectToESP32(navigation);
    
     
    
 }}
   
   style={selected ? styles.unpressedButton: styles.pressedButton}
   
   >
      <Text style={styles.buttonText}>Conectar</Text>

    </Pressable>

  </View>
    );
}

const styles = StyleSheet.create({
    container: {
      flex: 1,
      alignItems: 'center',
      justifyContent: 'center',
      backgroundColor: '#FFFAF1', //Color de fondo del Tab
      fontFamily: 'Source-Sans-Black'
    },
    title: {
      fontSize: 69,
      fontWeight: 'bold',
      color:'#D49073',
      textShadowColor: '#95573A',
      textShadowOffset: {width: 2, height:2},
      textShadowRadius: 2
    },
    separator: {
      marginVertical: 30,
      height: 1,
      width: '80%',
    },
  
    image: {
      flex: 1,
      width: '100%',
    
    },
  
    pressedButton:{
      backgroundColor:'#95573A',
      width:145,
      height:56,
      borderRadius:60,
      alignItems: 'center',
      justifyContent: 'center',
      marginTop: 50,
      marginBottom:100,
      opacity: 0.7,
      shadowColor: '#171717',
      shadowOffset: {width: -2, height: 4},
      shadowOpacity: 0.4,
      shadowRadius: 3,
    },
  
  
    unpressedButton:{
      backgroundColor:'#A53D0D',
      width:145,
      height:56,
      borderRadius:60,
      alignItems: 'center',
      justifyContent: 'center',
      marginTop: 50,
      marginBottom:100,
      opacity: 0.7,
      shadowColor: '#000000',
    },
  
    buttonText:{
      fontFamily: 'Source-Sans-SemiBold',
      fontSize:20,
      color:"#FFFAF1"
    }
  });
  
  const  connectToESP32 = (navigation) => {
      //GET request
      fetchWithTimeout(ESP32IP, {
        method: 'GET',
        timeout:1000
      })
      .then((response) => {  
        //Success
          if (response.ok){
          alert('¡Conexión exitosa!');
          }
          console.log(response.status);
          navigation.navigate('Main');
      })
      //If response is not in json then in error
      .catch((error) => {
        //Error
        alert('No se ha podido conectar al dispositivo.');
        console.error(error);
      });
  };
  
  
  
  
  async function fetchWithTimeout(resource, options = {}) {
    const {timeout = 8000 } = options;
    
    const controller = new AbortController();
    const id = setTimeout(() => controller.abort(), timeout);
    const response = await fetch(resource, {
      ...options,
      signal: controller.signal  
    });
    clearTimeout(id);
    return response;
  }
  