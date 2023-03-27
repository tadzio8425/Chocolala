import { StyleSheet, View, Image, Pressable, Alert, Text, TouchableOpacity, handlePress} from 'react-native';
import { useCallback, useState, useEffect, componentDidMount } from 'react';
import AppLoading from "expo-app-loading";
import useFonts from './hooks/useFonts';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import Dialog from "react-native-dialog";
import DialogInput from 'react-native-dialog/lib/Input';
import {ESP32IP} from "./Index";


var setteable_reference = 0;

export default function Main({navigation}) {
  const [visibleRefDia, setVisibleRefDia] = useState(false);

  const showRefDialog = () => {
    setVisibleRefDia(true);
  };

  const handleRefCancel = () =>{
    setVisibleRefDia(false);
  };

  const handleRefOk = () =>{
  
    //PUT REQUEST PARA CAMBIAR LA REFERENCIA
    fetchWithTimeout(`${ESP32IP}/reference`, {
      method: 'PUT',
      timeout:1000,
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ reference:setteable_reference})
    })
  
    setVisibleRefDia(false);

  };

  const [dataJSON, setData] = useState(true);
  const [weightJSON, setWeight] = useState(true);
  const [referenceJSON, setReference] = useState({"value":0});

  const loadData = useCallback(async () => {
    try {
      const response = await fetch(`${ESP32IP}/`);
      const dataJSON = await response.json();
      setData(dataJSON);
      setWeight(dataJSON[3]);
      setReference(dataJSON[0]);
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

      

      <View style={styles.chocoContainer}>
      <TouchableOpacity onPress={handlePress}>
          <Image style = {{width:340, height:121}} source={require("./assets/images/balanza.png")}/>
      </TouchableOpacity>

      <TouchableOpacity style={{zIndex:5}} onPress={showRefDialog}>
          <Image style = {{width:270, height:248,  marginLeft:38}} source={require("./assets/images/chocolatera.png")}/>
          <ChocoTextBox text={referenceJSON["value"].toFixed(2)} units={referenceJSON["unit"]}></ChocoTextBox>
      </TouchableOpacity>
      <TouchableOpacity  onPress={handlePress}>
          <Image style = {{width:236, height:194, position:"absolute", right:-30, bottom:-30, zIndex:1}} source={require("./assets/images/batidora_mangotext.png")}/>
      </TouchableOpacity>
      </View>

      <View style={styles.bottomContainer}>
      <TouchableOpacity onPress={handlePress}>
          <Image style = {{width:61, height:61}} source={require("./assets/images/offButton.png")}/>
      </TouchableOpacity>
      </View>

    
    <Dialog.Container visible={visibleRefDia}>
      <Dialog.Title>Dosificar</Dialog.Title>
      <Dialog.Description>
          Ingrese el valor a dosificar en mL:
        </Dialog.Description>
      <DialogInput keyboardType="numeric" onChangeText={ (inputText) => {setteable_reference = inputText.replace(",",".")} }></DialogInput>
      <Dialog.Button label="Cancel" onPress={handleRefCancel}/>
        <Dialog.Button label="OK" onPress={handleRefOk}/>
    </Dialog.Container>
    </View>


  );
}


const TextBox = ({text, units}) => (
  <View style = {styles.textBox}>
    <Text style={styles.numberText}>{text} {units}</Text>
  </View>
)

const ChocoTextBox = ({text, units}) => (
  <View style = {[styles.textBox,{zIndex:10, position:"absolute", marginTop:150, marginLeft:130,
  backgroundColor:"white"}]}>
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

    chocoContainer:{
      flex: 1,
      flexDirection: 'column-reverse',
      flexWrap: 'wrap',
      width:"100%",
      marginTop:"10%",
      marginLeft:"7%",
      alignItems:"center",
      width:"100%",
      alignContent:"flex-start",
      justifyContent:"flex-start"
    },

    bottomContainer:{
      flex: 1,
      flexDirection: 'row',
      flexWrap: 'wrap',
      width:"100%",
      maxHeight:60,
      marginTop:"10%",
      marginLeft:"7%",
      alignItems:"center",
      marginBottom:20
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
  