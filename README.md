<div>
  <h1> <img src="https://github.com/tadzio8425/ChocoBox/assets/78126968/c7adf459-6a21-4405-a622-e10d413c3f6a" width="60" height="60">    ChocoBox</h1>
</div>

### Environment replicator for cocoa fermentation:
- Electronic system that replicates a real life environment. Includes:
  - Continous environmental humidity and temperature measurement
  - Real-time control of both humidity and temperature
  - Spline generation to emulate environmental changes
  - Intuitive set-up and data collection via .txt files
- Backend developed using the Arduino Framework and an ESP32 development board.
- Frontend mobile app developed using the Expo Framework and React-Native.
- Developed in the *Universidad de Los Andes* for the *Biomicrosystems research group*.

## Developed by: 
<ul>
  <li>Juan Sebastián Ortega (https://github.com/tadzio8425)</li>
  <li>Nicolás Correal (https://github.com/Correaln47)</li>
</ul>

## Quickstart
### 1. Setting-up the files...

#### i. Creating the files
The ChocoBox system uses three *.txt* files inside a **MicroSD Card** in order to function, these files are *environment.txt*, *spline.txt* and *datalog.txt*. Altough these files have the *.txt* format, their content is designed to be read as a *.csv*. For the proper functioning of the system, it is **obligatory** for the files to be named exactly like this. If it is your first time using ChocoBox, or if you are resetting the system, please delete all previous files from the SD Card and create three empty *.txt* files.



<p align="center">
   <img align="center" src="https://github.com/tadzio8425/ChocoBox/assets/78126968/fbb66487-5b82-4f59-ba99-cb83608c9522" width="600"/>
</p>
<p align="center">
   <i>a. Contents of the MicroSD Card</i>
</p>

#### ii. Setting-up the environment
The environment to be replicated can be easily set in the *environment.txt* file. In order to to this, simply write the *csv* headers in the first row. These headers should be **hour, temperature, humidity**. Then, following the header order, write the desired values for specific hours in each new line. For example, this file replicates an environment for 96 hours, setting new desired values each 24 hours. There is no inverval restriction, neither there is a necessity for an interval, just make sure that the next hour is always greater than the previous one! **Please make sure that there are no extra new lines or blank spaces!**

<p align="center">
   <img align="center" src="https://github.com/tadzio8425/ChocoBox/assets/78126968/f976c0fc-785a-4942-922e-417c9dad664d" width="300"/>
</p>
<p align="center">
   <i>b. Example of an environment.txt file</i>
</p>

#### iii. Joining the dots
In order to replicate how these variables behave smoothly in nature, a cubic spline algorithm is used in order to fill the gaps between the dots set in *environment.txt*. The results of this algorithm are saved by the microcontroller in *spline.txt*, so this file must not be modified in any way by the user. If the **reference values** shown by the system are not logical, please check the *spline.txt* file. A *spline.txt* with **Nan** values or other errors indicates that *environment.txt* was wrongly formated! 

<p align="center">
   <img align="center" src="https://github.com/tadzio8425/ChocoBox/assets/78126968/4b548579-42e4-4d59-8f7e-05acb526f57b" width="200"/>
</p>
<p align="center">
   <i>c. Example of a correctly generated spline.txt file</i>
</p>

#### iv. Step by step
A fundamental part of the spline algorithm is the **step**, this value determines the interval for intermediate points to be calculated and also for data capture. For example, a step of **30 minutes** will make the system update its reference values and save its environmental data each, well, 30 minutes. The smaller the step, the more data will be captured and the smoother the system will be. **Beware of using extremely small steps, as they may lead to a memory overflow in the ESP32!** The **step can be modified by the user**, this will be explained in a further section.

#### v. Getting the data
All environmental data captured by the sensors is saved in *datalog.txt*, which is automatically updated by the microcontroller. This file includes the sensed information of each individual humidity and temperature sensor, captured each **step**. This file follows the order of the following headers: **time(min), tempA(degC), tempB(degC), tempC(degC), tempD(degC), humidLeft(%), humidRight(%)**

The presence of **Nan** values in this file may indicate a sensor malfunction!

<p align="center">
   <img align="center" src="https://github.com/tadzio8425/ChocoBox/assets/78126968/3d6e1b88-11cc-4e01-8d4d-01df40d26ecc" width="600"/>
</p>
<p align="center">
   <i>d. Example of a correctly generated datalog.txt file</i>
</p>

## 2. PCB Connections
Handling the PCB should be avoided at all costs. However, in the case that something has gone wrong or a new interface/sensor needs to be added, the following connection diagram should be **strictly** followed. All cables are marked with their respective signal.

| PCB Overview |
| --- |
|   <img src="https://github.com/tadzio8425/ChocoBox/assets/78126968/2ca30238-0328-40cd-a029-abc4c8aa6681"/>|


| DS18B20 (Temp) | DHT22 (Hum) | MicroSD |
| --- | --- | --- |
|<img src="https://github.com/tadzio8425/ChocoBox/assets/78126968/e798d74c-2ffb-4cd0-bbee-7f32eb367f21" width="500" />|<img src="https://github.com/tadzio8425/ChocoBox/assets/78126968/e02e660b-304e-4cde-aec5-374b74cfa1c3"  width="300"/>|<img src="https://github.com/tadzio8425/ChocoBox/assets/78126968/c6e60fec-fcf8-40fd-8cd9-fb2a80f35659"  width="200"/>|

| I2C (LCD Screen and Clock) | Humidifier and Heater Relay | 12V and WaterLevel |
| --- | --- | --- |
|<img src="https://github.com/tadzio8425/ChocoBox/assets/78126968/1ee6aa29-79af-438c-a5d3-52213d45c570" width="400" />|<img src="https://github.com/tadzio8425/ChocoBox/assets/78126968/7eeb919b-750c-42ed-8d26-0e261e60b264"  width="300"/>|<img src="https://github.com/tadzio8425/ChocoBox/assets/78126968/bb71bde5-03ff-42f6-b883-859d3c83a884"  width="300"/>|
