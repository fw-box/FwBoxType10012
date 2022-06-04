FwBoxType10012
======
IoT Load Cell (weighing)
----
 ![image](https://github.com/twhikari/FW-images/blob/main/Weighing.jpg)
 <!--
<div align = center> <image src="https://github.com/twhikari/FW-images/blob/main/Weighing.jpg"></div>
`SHT2X`
 -->

### Composed of `DSI5168`(base on Realtek RTL8711AM WiFi chips), [HX711](https://github.com/twhikari/FwBoxType10012/edit/main/README.md#HX711 "link"), [Load Cell Half Bridge Strain Gauge](https://github.com/twhikari/FwBoxType10012/edit/main/README.md#load-cell-half-bridge-strain-gauge "link"), [WS2812](https://github.com/twhikari/FwBoxType10012/edit/main/README.md#WS2812 "link"), [SSD1327](https://github.com/twhikari/FwBoxType10012/edit/main/README.md#SSD1237"link")

#### Device is used to check the weight of package when factory divide materials into portions.Client could control quantity and weight of package which is being measured by website or mobile phone.Base on result of weighing,we can get the deviation.After that,OLED and RGB will display different description and color according to the deviation.Meanwhile,the results will be recorded on website and mobile phone.

### Device schematic
![image](https://github.com/twhikari/FW-images/blob/main/Load%20cell%20desing%20schematic.png)

### I/O description

#### HX711 
<!--
![image](https://github.com/twhikari/FW-images/blob/main/HX711.png) 

-->

<image src="https://github.com/twhikari/FW-images/blob/main/HX711.png" align = left> 
<br><br> 
• Operation temperature range: -40 ~ +85℃  <br> 
• Operation supply voltage range: 2.6 ~ 5.5V  <br> 
• Selectable 10SPS or 80SPS output data rate  <br> 
• On-chip active low noise PGA with selectable gain of 32, 64 and 128  <br>  
<br> <br>
24-Bit Analog-to-Digital Converter
<br><br><br>
 
#### Load Cell Half Bridge Strain Gauge 

<image src="https://github.com/twhikari/FW-images/blob/main/Half_Bridge.png" align = left> 
<br> 
• Internal 1000Ohm half-bridge strain gauge <br> 
• Range is 50kg  <br> <br>  
User Method : <br>
&ensp;&ensp;&ensp;&ensp;Force the outer side of the strain E-shaped beam portion of the sensor,then the outer edges will form a shear force in the opposite direction (strain beam side by another force should be empty!)
<br><br><br>
Load Cell Half Bridge Strain Gauge
<br><br><br>
 
#### WS2812

<image src="https://github.com/twhikari/FW-images/blob/main/WS2812.png" align = left> 
<br><br><br>
• Single wire communication.  <br>  
• The power supply reverse connection does not damage the IC. <br> 
• Each pixel of the three primary colors can achieve 256 brightness levels <br> 
<br><br><br>
RGB LED built-in circular development board
<br><br><br>
 
#### SSD1327

<image src="https://github.com/twhikari/FW-images/blob/main/SSD1327.png" align = left> 
<br> 
• Internal 1000Ohm half-bridge strain gauge <br> 
• Range is 50kg  <br> <br>  
User Method : <br>
&ensp;&ensp;&ensp;&ensp;Force the outer side of the strain E-shaped beam portion of the sensor,then the outer edges will form a shear force in the opposite direction (strain beam side by another force should be empty!)
<br><br><br>
&ensp;&ensp;Load Cell Half Bridge Strain Gauge
