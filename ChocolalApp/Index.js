import { StyleSheet, View, Image, Pressable, Text, TouchableOpacity} from 'react-native';
import {useState } from 'react';
import Dialog from "react-native-dialog";
import DialogInput from 'react-native-dialog/lib/Input';


export let ESP32IP = 'http://192.168.1.35:8181';


export default function Index({navigation}){
    const [selected, setSelected] = useState(false);
    const [visibleIPSetter, setVisibleIPSetter] = useState(false);

    const handleOKIP = () =>{
      setVisibleIPSetter(false);
    }

    const setIP = () =>{
      setVisibleIPSetter(true);
    }

    return(
        
    <View style={styles.container}>

    <TouchableOpacity style={{width:121, height:121}} onPress={setIP}>
    <Image 
      style={styles.image} 
      source={require("./assets/images/hot-chocolate.png")}
    />
    </TouchableOpacity>


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


    <Dialog.Container visible={visibleIPSetter}>
      <Dialog.Title>IP</Dialog.Title>
      <Dialog.Description>
          Ingrese la IP de la chocolatera:
        </Dialog.Description>
      <DialogInput style={{color:'black'}} onChangeText={ (inputText) => ESP32IP = `http://${inputText}`}></DialogInput>
        <Dialog.Button label="OK" onPress={handleOKIP}/>
    </Dialog.Container>

  </View>

  
    );
}

const styles = StyleSheet.create({
    container: {
      flex: 1,
      alignItems: 'center',
      justifyContent: 'center',
      backgroundColor: '#FFFAF1', //Color de fondo del Tab
      fontFamily: 'Source-Sans-Black',
      height:"100%"
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
  