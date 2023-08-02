import { StyleSheet, View, Image, Pressable, Alert, Text, TouchableOpacity, handlePress, TextInput} from 'react-native';
import { useCallback, useState, useEffect, componentDidMount} from 'react';
import AppLoading from "expo-app-loading";
import useFonts from './hooks/useFonts';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import Dialog from "react-native-dialog";
import DialogInput from 'react-native-dialog/lib/Input';
import {ESP32IP} from "./Index";
import {Dimensions} from 'react-native';
import Modal from "react-native-modal";
import Slider from '@react-native-community/slider';
import { height } from '@mui/system';
import MarkSlider from 'react-native-mark-slider';
import { CountdownCircleTimer } from 'react-native-countdown-circle-timer';
import * as WebBrowser from 'expo-web-browser';
import * as DocumentPicker from 'expo-document-picker';
import * as Progress from 'react-native-progress';

const marks = [
  { name: '0  RPM', value: 0 },
  { name: '50', value: 50 },
  { name: '100', value: 100 },
  { name: '150', value: 150 },
  { name: '200', value: 200 },
  { name: '250', value: 250 },
  { name: '300 RPM', value: 300 },
];

const windowWidth = Dimensions.get('window').width;
const windowHeight = Dimensions.get('window').height;

let actualRPM = 0;

var setteable_reference = 0;

const map = (value, x1, y1, x2, y2) => (value - x1) * (y2 - x2) / (y1 - x1) + x2;
var scale_factor = 1;
scale_factor = map(windowHeight,  100, 785, 0, 1,);
resizeMargin = scale_factor * 10;

console.log(scale_factor);


export default function Main({navigation}) {

  const [sliderValue, setSliderValue] = useState(0); // initial value of the slider
  const [toggleSlider, setToggleSlider] =useState(false);  
  

  // function to handle changes in slider value

  


  useEffect(() => {
    // update slider value when desired value changes
    setSliderValue(sliderValue);
    setToggleSlider(toggleSlider);
    console.log(toggleSlider);
  }, [sliderValue, toggleSlider]);


  useEffect(() => {
    // update slider value when desired value changes
  }, [windowHeight]);



  const [visibleRefDia, setVisibleRefDia] = useState(false);
  const [visibleCali, setVisibleCali] = useState(false);
  const [visibleReset, setVisibleReset] = useState(false);
  const [visibleStep, setVisibleStep] = useState(false);
  const [isModalVisible, setModalVisible] = useState(false);
  const [okDisabled, setOkDisabled] = useState(true);
  const [remainingTime, setRemainingTime] = useState(10);

  useEffect(() => {
    setOkDisabled(remainingTime > 0);
  }, [remainingTime]);

  const toggleModal = () => {
    setModalVisible(!isModalVisible);
  };

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

  const handleResetOk = () =>{
    fetchWithTimeout(`${ESP32IP}/reset`, {
      method: 'PUT',
      timeout:1000,
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({reset:true})
    })
    .catch(function (err){
      console.log(err);  // Prints "Error: something went terribly wrong"
  });
    setVisibleReset(false);
};


const handleStepOk = () =>{
  fetchWithTimeout(`${ESP32IP}/step`, {
    method: 'PUT',
    timeout:1000,
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({step:stepVal})
  })
  .catch(function (err){
    console.log(err);  // Prints "Error: something went terribly wrong"
});
  setVisibleStep(false);
};



const handleResetCancel = () =>{
  setVisibleReset(false);
};


const handleStepCancel = () =>{
  setVisibleStep(false);
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

  const putReset = () =>{
    setVisibleReset(true);
  }

  const downloadDataLog = async() =>{
    let result = await WebBrowser.openBrowserAsync(`${ESP32IP}/dataLog`);
  }



const uploadFile = async () => {
  let singleFile = await DocumentPicker.getDocumentAsync({});
  

  // Check if any file is selected or not
  if (singleFile != null) {
    // If file selected then create FormData
    const data = new FormData();

    data.append("environment", singleFile, "environment.txt");

    // return
    try {
      let res = await fetchWithTimeout(ESP32IP + '/upload', {
        method: 'post',
        body: data,
        headers: {
          Accept: 'application/json',
          'Content-Type': 'multipart/form-data',
        },
        timeout: 5000,
      });

      let result = await res.json();
      console.log('result', result);
      if (result.status == 1) {
        Alert.alert('Info', result.msg);
      }
    } catch (error) {
      // Error retrieving data
      // Alert.alert('Error', error.message);
      console.log('error upload', error);
    }
  } else {
    // If no file selected the show alert
    Alert.alert('Please Select File first');``
  }
};

const map = (x,in_min,in_max,out_min,out_max) =>
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
};

  const [avgTemp, setAvgTemp] = useState({"value":0});

  const [dataJSON, setData] = useState(true);

  const [refTemp, setRefTemp] = useState({"value":0});
  const [refHumid, setRefHumid] = useState({"value":0});

  const [tempA, setTempA] = useState({"value":0});
  const [tempB, setTempB] = useState({"value":0});
  const [tempC, setTempC] = useState({"value":0});
  const [tempD, setTempD] = useState({"value":0});

  const [globalHumid, setGlobalHumid] = useState({"value":0});

  const [humidLeft, setHumidLeft] = useState({"value":0});
  const [humidRight, setHumidRight] = useState({"value":0});

  const [globalTemp, setGlobalTemp] = useState({"value":0});

  const[heaterON, setHeaterON] = useState({"value":0});
  const[humidON, setHumidON] = useState({"value":0});

  const[waterLvl, setWater] = useState({"value":0});

  const[resColor, setResColor]= useState("#b47b42");
  const[humidColor, setHumColor]= useState("white");


  const [selected, setSelected] = useState(false);
  const [selectedStep, setSelectedStep] = useState(false);
  
  const [stepVal, setStepValue] = useState(0.5);

  const loadData = useCallback(async () => {
    try {
      const response = await fetch(`${ESP32IP}/`);
      const dataJSON = await response.json();
      setData(dataJSON);

      setRefTemp(dataJSON[0]);
      setRefHumid(dataJSON[1]);
      setTempA(dataJSON[2]);
      setTempB(dataJSON[3]);
      setTempC(dataJSON[4]);
      setTempD(dataJSON[5]);
      setGlobalHumid(dataJSON[6]);
      setHumidLeft(dataJSON[7]);
      setHumidRight(dataJSON[8]);
      setGlobalTemp(dataJSON[9]);
      setHeaterON(dataJSON[10]);
      setHumidON(dataJSON[11]);
      setWater(dataJSON[12]);
    } catch (error) {
      console.log(error);
    }
  }, []);


  

  changeColor = () => {
    if(heaterON["value"] == 1){
      setResColor("red");
    }
    else{
      setResColor("#b47b42");
    }

    if(humidON["value"] == 1){
      setHumColor("blue");
    }
    else{
      setHumColor("white");
    }
  }

  useEffect(() => {
    const interval = setInterval(loadData, 1000);
    return () => clearInterval(interval);
  }, [loadData]);

  useEffect(() => {
    const interval = setInterval(() => changeColor(), 1000);
    return () => {
      clearInterval(interval);
    }
  })


  sliderValueChange = (value) => {
    // Array of desired values
    const array = [0, 10, 20, 50, 100, 150, 200, 250, 300]
  
    const output = array.reduce((prev, curr) => Math.abs(curr - value) < Math.abs(prev - value) ? curr : prev)
  
    setSliderValue(output);
    // ... Pass output to UI, state, whatever

    if(toggleSlider === true){
      setToggleSlider(false);
    }
    else{
      setToggleSlider(true);
    }
    
  }


  


  return (
    <View style={[styles.container]}>
      <View style={styles.titleContainer}>
        <Image
          style={{width: 60, height: 60}}
          source={require("./assets/images/package.png")}
        />
        <Text style={styles.title}>ChocoBox</Text>
      </View>

      <View style={[styles.innerConntainer, {}]}>
        <Text style={styles.variableText}>Temp.</Text>
        <TextBox text={globalTemp["value"].toFixed(2) + " / " + refTemp["value"].toFixed(2)} units="°C"></TextBox>
        <Text style={[styles.variableText, {marginLeft: "5%"}]}>Hum.</Text>
        <TextBox text={globalHumid["value"].toFixed(2) + " / " + refHumid["value"].toFixed(2)}  units="%"></TextBox>
      </View>

      <View style={[styles.innerConntainer, {alignItems:"flex-start"}]}>
        <Text style={[styles.numberText, {fontSize:14}]}>Real / Ref</Text>
        <Text style={[styles.numberText, {fontSize:14}]}>Real / Ref</Text>
      </View>

      <View style={[styles.img, styles.chocoContainer]}>

        <View style={[styles.innerConntainer, {flexDirection:"row", justifyContent:"space-around",
        alignItems:"flex-end", padding:0, marginBottom:10, maxHeight:"30%"}]}> 
          <View style={[styles.fermentador]}>
            <Text style={[styles.numberText, {fontSize:20}]}>C</Text>
            <MiniTextBox  text={tempC["value"].toFixed(2)}  units="°C"></MiniTextBox>
          </View>

          <View style={[styles.fermentador]}>
          <Text style={[styles.numberText, {fontSize:20}]}>A</Text>
            <MiniTextBox  text={tempA["value"].toFixed(2)}  units="°C"></MiniTextBox>
          </View>

          <View style={[styles.fermentador]}>
          <Text style={[styles.numberText, {fontSize:20}]}>B</Text>
            <MiniTextBox  text={tempB["value"].toFixed(2)}  units="°C"></MiniTextBox>
          </View>
        </View>

        <View style={[styles.innerConntainer, {flexDirection:"row", justifyContent:"space-around",
        alignItems:"center", padding:0}]}> 

        <View style = {[styles.miniTextBox, {maxWidth:'25%'}]}>
            <Text style={styles.numberText}>{humidLeft["value"].toFixed(2)} {"%"}</Text>
        </View>

          <Ring size={85} colorXD={humidColor}></Ring>

        <View style = {[styles.miniTextBox, {maxWidth:'25%'}]}>
            <Text style={styles.numberText}>{humidRight["value"].toFixed(2)} {"%"}</Text>
        </View>

        </View>

        <Progress.Bar style={[{marginBottom:10}]} progress={map(waterLvl["value"], 300, 1750, 0, 1)} width={200} />

        <View style={[styles.innerConntainer, {flexDirection:"row", justifyContent:"center",
        alignItems:"center", padding:0}]}>  
          <View style={styles.resistorPin}></View>
          <View style={[styles.resistor, {backgroundColor:resColor, opacity:0.6}]}></View>
          <View style={styles.resistorPin}></View>
          
        </View>
        

        <View style = {[styles.textBox, {marginTop:10, flexDirection:"row", textAlign:"center"}]}>
          <Text style={[styles.numberText, {fontWeight:"bold"}]}>D: </Text>
          <Text style={styles.numberText}>{tempD["value"].toFixed(2)} {"°C"}</Text>
        </View>


      </View>

      <View style={[styles.bottomContainer]}>
      <TouchableOpacity onPress={putReset}>
          <Image style = {{width:50, height:50}} source={require("./assets/images/offButton.png")}/>
      </TouchableOpacity>



      
    <Pressable 
    onPressIn = {() => {
      setSelected(!selected);
    }}

   onPressOut = {() => {
    setSelected(!selected);      
    setModalVisible(true);
    }}
   
   style={[selected ? styles.unpressedButton: styles.pressedButton, {marginLeft:20, marginTop:95}]}
   
   >
      <Text style={styles.buttonText}>Data</Text>

    </Pressable>


    <Pressable 
    onPressIn = {() => {
      setSelectedStep(!selectedStep);
    }}

   onPressOut = {() => {
    setSelectedStep(!selectedStep);      
    setVisibleStep(true);
    }}
  
   style={[selectedStep ? styles.unpressedButton: styles.pressedButton, {marginLeft:50, marginTop:95, width:100}]}
   
   >
      <Text style={[styles.buttonText]}>Step</Text>

    </Pressable>
      </View>

    <Dialog.Container visible={visibleReset}>
      <Dialog.Title>Reset</Dialog.Title>
      <Dialog.Description>
        ¿Está seguro que que desea reiniciar la fermentación desde 0?
      </Dialog.Description>
      <Dialog.Button label="OK" onPress={handleResetOk}/>
      <Dialog.Button label="Cancel" onPress={handleResetCancel}/>
    </Dialog.Container>


    <Dialog.Container visible={visibleStep}>
      <Dialog.Title>Step</Dialog.Title>
      <Dialog.Description>
        Ingrese el paso para la toma de datos en horas, se aceptan decimales:
      </Dialog.Description>
      <Dialog.Input keyboardType="numbers-and-punctuation" onChangeText={text => {setStepValue(text)}}/>
      <Dialog.Button label="OK" onPress={handleStepOk}/>
      <Dialog.Button label="Cancel" onPress={handleStepCancel}/>
    </Dialog.Container>


    <Modal isVisible={isModalVisible}>
      <View style={[{ flex: 1 }, styles.modal]}>
        <Text style={{fontSize:20, fontWeight:"bold", marginTop:20}}>Datos de fermentación</Text>
        

        <Pressable style={[styles.pressedButton, {marginBottom:0,opacity:0.9, margin:0}]}  onPressOut = {() => {downloadDataLog()}}>
        <Text style={[styles.buttonText, {textAlign:"center", fontSize:16}]}>Descargar dataLog</Text>
      </Pressable>

      
      <Pressable style={[styles.pressedButton, {marginBottom:0, opacity:0.9, margin:0}]}  onPressOut = {() => {uploadFile()}}>
        <Text style={[styles.buttonText, {textAlign:"center", fontSize:16}]}>Subir environment.txt</Text>
      </Pressable>

      
      <Pressable style={[styles.pressedButton, {marginBottom:0,width:100,height:40, borderRadius:10, margin:0}]}  onPressOut = {() => {toggleModal()}}>
        <Text style={[styles.buttonText]}>Cerrar</Text>
      </Pressable>

      </View>

    </Modal>


    </View>


  );
}


const TextBox = ({text, units}) => (
  <View style = {styles.textBox}>
    <Text style={styles.numberText}>{text} {units}</Text>
  </View>
)

const MiniTextBox = ({text, units}) => (
  <View style = {styles.miniTextBox}>
    <Text style={styles.numberText}>{text} {units}</Text>
  </View>
)

const ChocoTextBox = ({text, units}) => (
  <View style = {[styles.textBox,{zIndex:10, position:"absolute", marginTop:150, marginLeft:130,
  backgroundColor:"white"}]}>
    <Text style={styles.numberText}>{text} {units}</Text>
  </View>
)


const Ring = ({ size, colorXD }) => {
  return (
    <View
      style={[
        styles.circle,
        {
          width: size,
          height: size,
          borderRadius: size / 2,
          borderWidth: (size * 5) / 100,
          backgroundColor:"#473831 rgba(71, 56, 49, 0.8)",
          borderColor:"#524038",
          marginBottom:20,
          alignItems:"center",
          justifyContent:"center"
        },
      ]}
    >
      <View       style={[
        styles.circle,
        {
          width: size/4,
          height: size/4,
          borderRadius: size / 2,
          borderWidth: (size * 5) / 200,
          backgroundColor: colorXD,
          borderColor:"black",
          opacity:0.6
        },
      ]}></View>
    </View>
  );
};



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
      paddingTop:10,
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
      opacity: 0.8,
      shadowColor: '#171717',
      shadowOffset: {width: -2, height: 4},
      shadowOpacity: 0.4,
      shadowRadius: 3,
    },

    textBox:{
      backgroundColor:'#95573A',
      width:127,
      height:35,
      borderRadius:30,
      alignItems: 'center',
      justifyContent: 'center',
      opacity: 0.54,
      marginLeft:"2%",
      textAlign:"center"
    },

    resistor:{
      width:"75%",
      height:"15%",
      opacity:0.8
    },

    resistorPin:{
      width:"10%",
      height:"5%",
      backgroundColor:"grey"
    },

    miniTextBox:{
      backgroundColor:'#95573A',
      width:"90%",
      height:35,
      borderRadius:30,
      alignItems: 'center',
      justifyContent: 'center',
      opacity: 0.54,
      textAlign:"center"
    },

    fermentador:{
      borderWidth:5,
      width:"30%",
      borderColor:"rgba(149, 87, 58, 0.8)",
      borderRadius:10,
      backgroundColor:'rgba(166, 131, 114, 0.7)',
      justifyContent:"space-evenly",
      alignItems:"center",
      height:"100%"
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
      alignItems:"center",
      justifyContent:"space-around",
      paddingLeft:"5%",
      paddingRight:"5%"
    },

    chocoContainer:{
      flex: 1,
      flexDirection: 'column-reverse',
      alignSelf: 'center',
      minHeight:"40%",
      flexWrap:"nowrap",
      alignItems:"center",
      alignContent:"flex-start",
      borderWidth:5,
      borderRadius:40,
      width:"90%",
      borderColor:"#c4c4c0",
      backgroundColor:"whitesmoke"
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
    },

    modal:{
      width:"80%",
      maxHeight:"50%",
      backgroundColor:"whitesmoke",
      alignSelf:'center',
      borderRadius:20,
      alignItems:"center",
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
  