Fully_Connected_Layer
The targeting FPGA is ZCU 102 and QFDB
Tools -> Vivado HLS 2017.1
________________________________________________________
For the purpose of this IP four HLS files were created

1. Dense.h
Contains the main parts of the Accelerator

2. Dense_Accel.cpp
Contains the interconnection with the streams (AXI-4 stream)

3. Dense_def.h
Contains useful defines for the decompression of the weights

4. Dense_Accel_Test.cpp
Contains Fully_Connected_Layer TestBench (import weights and image and evaluate the results)

The first three are the main core of the Fully_Connected_Layer Accelerator.
The 4th is the TestBench for debug reason and the evaluation of the results.
_________________________________________________________
Folders 

HLS_code -> with the corresponding HLS files
Input_Data_Files -> folder with the data in binary for the TestBench.  
The paths were set locally in the project. Optionally you can import new data by replacing the existing ones, ore change the path.
_________________________________________________________
How to open the project

1. Open Vivado HLS
2. Open Project
3. Select the folder Fully_Connected_Layer and open
