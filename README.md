#Sensor Visualizer <br />
<!--![alt tag](https://github.com/wrn14897/Sensor-Visualizer/blob/master/imgs/demoImg.jpg) <br />-->
Hardware: Raspberry Pi, ADC(MCP3008), Test Sensors(microphone module, photoresistor) <br />
Description: Sensor Visualizer is a console program for ARM embedded system (Raspberry Pi) which visualizes the data read from 'Analog to Digital Converter' (MCP3008). Currently it has two kinds of modes to visulize the data, in which user can interact with it by several simple commands.<br />
##SPI input driver 
  * For information about SPI: <br />
    Basically, SPI uses synchronous bus which keeps data and clock sides perfect sync. The data will be sent through MOSI(Master Out/ Slave In) which controls different input values. Note that the "Master" here stands for our Pi. And Pi will get the response through MISO pin. Of course we also need CLK pin to generate clock signals to keep Pi read bit data each cycles. For more detail description, please check the following link: <br />
    https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi
    
  * Read data from MCP3008: <br />
    - For information about MCP3008: <br />
    https://learn.adafruit.com/reading-a-analog-in-and-controlling-audio-volume-with-the-raspberry-pi/connecting-the-cobbler-to-a-mcp3008 
    - Circuit sketch: <br />
    The variable resistor here can be replaced with different sensors. The following is how we connent MCP3008 with Pi: 
    ![alt tag](https://github.com/wrn14897/Sensor-Visualizer/blob/master/imgs/CircuitSketch.jpg) <br />
    - Implementation: <br />
    Here we use bitbang SPI implementation described in [resource link](https://learn.adafruit.com/reading-a-analog-in-and-controlling-audio-volume-with-the-raspberry-pi/script). The idea is to use general GPIO pins instead of SPI interface since it is not included in all distributions. 
    ![alt tag](https://github.com/wrn14897/Sensor-Visualizer/blob/master/imgs/ADCtoPi.JPG) <br />
    - Test: <br />
    To test the driver actually works, we use the console program to print out the data from different sensors. <br />

##GL library
  * Sine and cosine implementation (sine.h): <br />
    Since we don't have float or double type for bare metal programming, the easiest way to implement sine and cosine functions is to build up a lookup table. Each corresponding value has been multiplied with a constant [ex: when the constant is 100, the range for sine or cosine will be within (-100, 100)]
  * Model design: <br />
    The model we are using to visualize the data is similar to the shape of automobile dashboard. Before drawing the model, we should build up 'draw_empty_circle()' function. Our result looks like (two modes): <br />
    - Mode 1 (volume bars) 
      ![alt tag](https://github.com/wrn14897/Sensor-Visualizer/blob/master/imgs/demoBar.JPG) <br />
    - Mode 2 (dashboard) 
      ![alt tag](https://github.com/wrn14897/Sensor-Visualizer/blob/master/imgs/demoCir.JPG) <br />  
  * Optimization: <br />
    In order to speed up the clearing graphic process to synchronize the animation with input data, our implementation only clears the graphic buffer for specific range (the buffer would be updated each time). Also, we apply double buffer to avoid the circumstance that GPU and CPU both access the same memory location.  

##Summary
The further implementation would be to visualize more channels (8 channels for MCP3008). More interestingly, by storing data in a buffer, we can keep track of past data and viaulize it through trajectories and even transfer this chuck of data to complex domain (ex: Fourier transform). For practical usage, we can implement more mathematical functions (ex: log) and then display actual physical quantities (ex: dB).





