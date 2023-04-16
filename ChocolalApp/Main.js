import { StyleSheet, View, Image, Pressable, Alert, Text, TouchableOpacity, handlePress} from 'react-native';
import { useCallback, useState, useEffect, componentDidMount } from 'react';
import AppLoading from "expo-app-loading";
import useFonts from './hooks/useFonts';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import Dialog from "react-native-dialog";
import DialogInput from 'react-native-dialog/lib/Input';
import {ESP32IP} from "./Index";
import {Dimensions} from 'react-native';

const windowWidth = Dimensions.get('window').width;
const windowHeight = Dimensions.get('window').height;

var setteable_reference = 0;
var scale_factor = 1;


if(windowHeight < 560){
  scale_factor = 0.8;
}

export default function Main({navigation}) {
  const [visibleRefDia, setVisibleRefDia] = useState(false);
  const [visibleCali, setVisibleCali] = useState(false);
  const [visibleStop, setVisibleStop] = useState(false);

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

    putWaterFill(true);
  
    setVisibleRefDia(false);

  };

  const handleCalOk = () =>{
      setVisibleCali(false);
  };

  const handleStopOk = () =>{
    setVisibleStop(false);
};

  const putCalibrate = () =>{

    //Shows the dialog
    setVisibleCali(true);

    //PUT REQUEST PARA CALIBRAR
    fetchWithTimeout(`${ESP32IP}/calibrate`, {
      method: 'PUT',
      timeout:1000,
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({calibrate:true})
    })
    .catch(function (err){
      console.log(err);  // Prints "Error: something went terribly wrong"
  });
  };

  const putWaterFill = (state) =>{
        //PUT REQUEST PARA CALIBRAR
        fetchWithTimeout(`${ESP32IP}/waterFill`, {
          method: 'PUT',
          timeout:1000,
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({waterFill:state})
        })
        .catch(function (err){
          console.log(err);  // Prints "Error: something went terribly wrong"
      });
  };

  const putStop = () =>{
    setVisibleStop(true);
    fetchWithTimeout(`${ESP32IP}/stop`, {
      method: 'PUT',
      timeout:1000,
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({stop:true})
    })
    .catch(function (err){
      console.log(err);  // Prints "Error: something went terribly wrong"
  });
  }


  const [dataJSON, setData] = useState(true);
  const [weightJSON, setWeight] = useState({"value":0});
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
        <TextBox text={weightJSON["value"].toFixed(2)} units={weightJSON["unit"]}></TextBox>
        <Text style={[styles.variableText, {marginLeft: "5%"}]}>Temperatura</Text>
        <TextBox text="yyy" units="°C"></TextBox>
      </View>

      

      <View style={[styles.img, styles.chocoContainer]}>
      <TouchableOpacity onPress={putCalibrate}>
          <Image style = {{width:340*scale_factor, height:121*scale_factor}} source={require("./assets/images/balanza.png")}/>
      </TouchableOpacity>

      <TouchableOpacity style={[{zIndex:5}, styles.img]} onPress={showRefDialog}>
          <Image  style = {{width:270*scale_factor, height:248*scale_factor,  marginLeft:38}} source={require("./assets/images/chocolatera.png")}/>
          <ChocoTextBox text={referenceJSON["value"].toFixed(2)} units={referenceJSON["unit"]}></ChocoTextBox>
      </TouchableOpacity>
      <TouchableOpacity  onPress={handlePress}>
          <Image style = {{width:236*scale_factor, height:194*scale_factor, zIndex:1000, bottom:-35, left:-85}} source={require("./assets/images/batidora_mangotext.png")}/>
      </TouchableOpacity>
      </View>

      <View style={styles.bottomContainer}>
      <TouchableOpacity onPress={putStop}>
          <Image style = {{width:50, height:50}} source={require("./assets/images/offButton.png")}/>
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


    <Dialog.Container visible={visibleCali}>
      <Dialog.Title>Calibrar</Dialog.Title>
      <Dialog.Description>
            !Se ha calibrado exitosamente la balanza!
      </Dialog.Description>
      <Dialog.Button label="OK" onPress={handleCalOk}/>
    </Dialog.Container>

    <Dialog.Container visible={visibleStop}>
      <Dialog.Title>Stop</Dialog.Title>
      <Dialog.Description>
            Se ha detenido y reiniciado la chocolatera.
      </Dialog.Description>
      <Dialog.Button label="OK" onPress={handleStopOk}/>
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
      fontFamily: 'Source-Sans-Black',
      height:"100%"
    },
    title: {
      fontSize: 36,
      fontWeight: 'bold',
      color:'#D49073',
      textShadowColor: '#95573A',
      textShadowOffset: {width: 1, height:1},
      textShadowRadius: 2,
      paddingTop:20,
      paddingLeft:"3%",
      width:"100%"
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
      alignItems: 'flex-start', // if you want to fill rows left to right
      minHeight:60,
      marginTop:10,
      marginLeft:"5%"
    },

    innerConntainer:{
      flex: 1,
      flexDirection: 'row',
      width:"100%",
      marginTop:10,
      alignItems:"center",
      justifyContent:"space-around",
      paddingLeft:"5%",
      paddingRight:"5%"
    },

    chocoContainer:{
      flex: 1,
      flexDirection: 'column-reverse',
      alignSelf: 'center',
      minHeight:"62%",
      marginTop:"10%",
      alignItems:"center",
      alignContent:"flex-start"
    },

    bottomContainer:{
      flex: 1,
      flexDirection: 'row',
      width:"100%",
      maxHeight:60,
      marginTop:40,
      marginLeft:"5%",
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
    },

    
    img:{
      maxWidth: "100%",
      maxHeight: "100%"
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
  