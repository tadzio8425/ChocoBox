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
  const [visibleStop, setVisibleStop] = useState(false);
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


  const putRPM = (value) =>{

    actualRPM = value;


    fetchWithTimeout(`${ESP32IP}/rpm`, {
      method: 'PUT',
      timeout:1000,
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({rpm:value})
    })
    .catch(function (err){
      console.log(err);  // Prints "Error: something went terribly wrong"
  });
  }


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
      
    } catch (error) {
      console.log(error);
    }
  }, []);

  useEffect(() => {
    const interval = setInterval(loadData, 1000);
    return () => clearInterval(interval);
  }, [loadData]);




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
          source={require("./assets/images/hot-chocolate.png")}
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

          <Ring size={85}></Ring>


        <View style = {[styles.miniTextBox, {maxWidth:'25%'}]}>
            <Text style={styles.numberText}>{humidRight["value"].toFixed(2)} {"%"}</Text>
        </View>

        </View>


        <View style={[styles.innerConntainer, {flexDirection:"row", justifyContent:"center",
        alignItems:"center", padding:0}]}>  

          <View style={styles.resistorPin}></View>
          <View style={styles.resistor}></View>
          <View style={styles.resistorPin}></View>
          
        </View>
        

        <View style = {[styles.textBox, {marginTop:10, flexDirection:"row", textAlign:"center"}]}>
          <Text style={[styles.numberText, {fontWeight:"bold"}]}>D: </Text>
          <Text style={styles.numberText}>{tempD["value"].toFixed(2)} {"°C"}</Text>
        </View>


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
      <DialogInput style={{color:'black'}} keyboardType="numeric" onChangeText={ (inputText) => {setteable_reference = inputText.replace(",",".")} }></DialogInput>
      <Dialog.Button label="Cancel" onPress={handleRefCancel}/>
        <Dialog.Button label="OK" onPress={handleRefOk}/>
    </Dialog.Container>


  <Dialog.Container visible={visibleCali} contentStyle={{paddingBottom: 20}}>
  <Dialog.Title>Calibrar</Dialog.Title>
  <Dialog.Description>
    Se está calibrando la balanza...
  </Dialog.Description>
  <View style={{ alignItems: 'center' }}>
    <CountdownCircleTimer
      isPlaying
      duration={10}
      size = {120}
      colors={['#3a6f8a', '#3a9099', '#32b3a1','#1fd9a7', '#56f0a0', "#99ffa2", "#6ce4c4"]}
      colorsTime={[10,9,7,5,4,2,0]}
      onComplete={() => {setRemainingTime(0), handleCalOk()}}>
      {({ remainingTime }) => {
          setRemainingTime(remainingTime);
          return <Text>{remainingTime}</Text>;
        }}
    </CountdownCircleTimer>
  </View>
</Dialog.Container>


    <Dialog.Container visible={visibleStop}>
      <Dialog.Title>Stop</Dialog.Title>
      <Dialog.Description>
            Se ha detenido y reiniciado la chocolatera.
      </Dialog.Description>
      <Dialog.Button label="OK" onPress={handleStopOk}/>
    </Dialog.Container>


    <Modal isVisible={isModalVisible}>
      <View style={[{ flex: 1 }, styles.modal]}>
        <Text style={{fontSize:20, fontWeight:"bold", marginTop:20}}>Velocidad de mezcla</Text>
        

        <MarkSlider style={{minWidth:"90%", minHeight:40, zIndex:1000}}
              step={1}
              max={300}
              marks={marks}
              onChange={value => {putRPM(value), setSliderValue(value)}}
          />
        <View style={{backgroundColor:"#BD8E79", width:80, borderRadius:20, height:40, flexWrap:"wrap",
        justifyContent:'center', alignContent:"center", alignItems:"center"}}>
          <TextInput
          style = {{color:"black", fontWeight:"bold", marginRight:2}}
          placeholder= {`${actualRPM}`}
          keyboardType="numbers-and-punctuation"
          onChangeText={text => {setSliderValue(text), putRPM(text)}}
          value={`${sliderValue}`}
          />
          <Text style={{ marginLeft:2}}>RPM</Text>
        </View>
      
      <Pressable style={[styles.pressedButton, {width:100,height:40, borderRadius:10, marginBottom:30, marginTop:40}]}  onPressOut = {() => {toggleModal()}}>
        <Text style={[styles.buttonText]}>Cerrar</Text>
      </Pressable>


      <View>
        <Text style={{color:"grey"}}>{tempB["value"].toFixed(0) + " RPM"}</Text>
      </View>

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


const Ring = ({ size }) => {
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
          backgroundColor:"blue",
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
      backgroundColor:"#b47b42",
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
  