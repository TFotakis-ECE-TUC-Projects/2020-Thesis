Convolutional_Layers
The targeting FPGA is ZCU 102 and QFDB
Tools -> Vivado HLS 2017.1
________________________________________________________
For the purpose of this IP four HLS files were created:

1. conv.h
Contains the main parts of the Accelerator
 -Convolution1 : Convolution 1-D
 -Convolution2 : Convolution 2-D
 -PackedFIFO : Pack and stream the Convolutional_Layers' output in 1024 bits streams.
 -ShiftedFIFO : Pack, perform a Shift behavior and stream the Convolutional_Layers output in 512 bits streams.
 -ParallelTask : Activate Task Level Parallelism to create a big pipeline from the first to the last layer.
 -Read_Data : Read Data (kernels, bias, image) from the input stream and protect from the Dataflow Task Level 
              Parallelism using mutexes behavior.

2. Conv_Accel.cpp
Contains the interconnection with the streams (AXI-4 stream)

3. Conv_def.h
Contains useful defines for the decompression of the weights

4. Conv_Accel_Test.cpp
Contains Fully_Connected_Layer TestBench (import weights and image and evaluate the results)

The first three are the main core of the Convolutional_Layers Accelerator.
The 4th is the TestBench for debug reasons and the evaluation of the results.
_________________________________________________________
Folders 

HLS_code -> with the corresponding HLS files
Input_Data_Files -> folder with the data in binary for the TestBench.  
The paths were set locally in the project. Optionally you can import new data by replacing the existing ones, ore change the path.
_________________________________________________________
How to open the project

1. Open Vivado HLS
2. Open Project
3. Select the folder Convolutional_Layers and open
