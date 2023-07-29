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
