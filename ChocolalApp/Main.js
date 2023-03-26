import { StyleSheet, View, Image, Pressable, Alert, Text} from 'react-native';
import { useCallback, useState, useEffect } from 'react';
import AppLoading from "expo-app-loading";
import useFonts from './hooks/useFonts';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';

const ESP32IP = 'http://192.168.1.4:8181';


export default function Main({navigation}) {
  const [dataJSON, setData] = useState(true);
  const [weightJSON, setWeight] = useState(true);

  const loadData = useCallback(async () => {
    try {
      const response = await fetch(`${ESP32IP}/`);
      const dataJSON = await response.json();
      setData(dataJSON);
      setWeight(dataJSON[3]);
    } catch (error) {
      console.log(error);
    }
  }, []);

  useEffect(() => {
    const interval = setInterval(loadData, 1000);
    return () => clearInterval(interval);
  }, [loadData]);

  return (
    <View style={styles.container}>
      <View style={styles.titleContainer}>
        <Image
          style={{width: 60, height: 60}}
          source={require("./assets/images/hot-chocolate.png")}
        />
        <Text style={styles.title}>Chocolala</Text>
      </View>

      <View style={styles.innerConntainer}>
        <Text style={styles.variableText}>Peso</Text>
        <TextBox text={weightJSON["value"]} units={weightJSON["unit"]}></TextBox>
        <Text style={[styles.variableText, {marginLeft: "5%"}]}>Temperatura</Text>
        <TextBox text="yyy" units="°C"></TextBox>
      </View>
    </View>
  );
}


const TextBox = ({text, units}) => (
  <View style = {styles.textBox}>
    <Text style={styles.numberText}>{text} {units}</Text>
  </View>
)

const styles = StyleSheet.create({
    container: {
      flex: 1,
      alignItems:"flex-start",
      justifyContent: "flex-start",
      backgroundColor: '#FFFAF1', //Color de fondo del Tab
      fontFamily: 'Source-Sans-Black'
    },
    title: {
      fontSize: 36,
      fontWeight: 'bold',
      color:'#D49073',
      textShadowColor: '#95573A',
      textShadowOffset: {width: 1, height:1},
      textShadowRadius: 2,
      paddingTop:20,
      paddingLeft:"3%"
    },
    separator: {
      marginVertical: 30,
      height: 1,
      width: '80%',
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

    textBox:{
      backgroundColor:'#95573A',
      width:91,
      height:35,
      borderRadius:30,
      alignItems: 'center',
      justifyContent: 'center',
      opacity: 0.54,
      marginLeft:"2%",
      textAlign:"center"
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
    },
    titleContainer:{
      flex: 1,
      flexDirection: 'row',
      flexWrap: 'wrap',
      alignItems: 'flex-start', // if you want to fill rows left to right
      width:"100%",
      maxHeight:60,
      marginTop:"10%",
      marginLeft:"5%",
      alignSelf:"start"
    },

    innerConntainer:{
      flex: 1,
      flexDirection: 'row',
      flexWrap: 'wrap',
      width:"100%",
      maxHeight:60,
      marginTop:"10%",
      marginLeft:"7%",
      alignItems:"center"
    },

    variableText:{
      fontFamily: 'Source-Sans-SemiBold',
      fontSize:20,
      color:"#535353"
    },

    numberText:{
      fontFamily: 'Source-Sans-Regular',
      fontSize:20
    }

    }
  );
  

  
  
  
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
  