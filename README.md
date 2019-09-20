# Diploma Thesis
![Technical-University-of-Crete-TUC-logo](Documentation/Assets/Technical-University-of-Crete-logo.png)  
[Technical University of Crete][tucPage]

Reconfigurable-based Design and Implementation of AlexNet Deep Learning Network  

Author:  
[Tzanis Fotakis][tzanisPage]

Thesis Committee:  
[Prof. Apostolos Dollas][dollasPage],  
[Prof. Michail Lagoudakis][lagoudakisPage],  
[Dr. Christos Kozanitis][kozanitisPage]

*A thesis submitted in fulfillment of the requirements for the Diploma of Electrical and Computer Engineering in the [Microprocessor and Hardware Lab][mhlPage] of [School of Electrical and Computer Engineering][ecePage]*

<!-- Todo: Add Date Here -->
# Abstract
# Acknowledgments
# Contents
- [Diploma Thesis](#diploma-thesis)
- [Abstract](#abstract)
- [Acknowledgments](#acknowledgments)
- [Contents](#contents)
- [List of Figures](#list-of-figures)
- [List of Tables](#list-of-tables)
- [List of Algorithms](#list-of-algorithms)
- [List of Abbreviations](#list-of-abbreviations)
- [Chapter 1: Introduction](#chapter-1-introduction)
	- [1.1 Motivation](#11-motivation)
	- [1.2 Scientific Contributions](#12-scientific-contributions)
	- [1.3 Thesis Outline](#13-thesis-outline)
- [Chapter 2: Theoretical Background](#chapter-2-theoretical-background)
	- [2.1 Machine Learning](#21-machine-learning)
	- [2.2 Convolutional Neural Network](#22-convolutional-neural-network)
	- [2.3 Structure of Convolutional Neural Network](#23-structure-of-convolutional-neural-network)
		- [2.3.1 Convolution Layer](#231-convolution-layer)
		- [2.3.2 Pooling](#232-pooling)
		- [2.3.3 Activation Function](#233-activation-function)
	- [2.4 Typical Architecture of AlexNet](#24-typical-architecture-of-alexnet)
- [Chapter 3: Related Work](#chapter-3-related-work)
	- [3.1 Google Brain Project](#31-google-brain-project)
		- [3.1.1 DistBelief](#311-distbelief)
		- [3.1.2 TensorFlow](#312-tensorflow)
		- [3.1.3 Tensor Processing Unit (TPU)](#313-tensor-processing-unit-tpu)
	- [3.2 GPU Approach](#32-gpu-approach)
	- [3.3 The FPGA Perspective](#33-the-fpga-perspective)
	- [3.4 AlexNet](#34-alexnet)
	- [3.5 Thesis Approach](#35-thesis-approach)
- [Chapter 4: AlexNet Robustness Analysis](#chapter-4-alexnet-robustness-analysis)
	- [4.1 Algorithms:](#41-algorithms)
		- [Algorithm [x]: Convolution](#algorithm-x-convolution)
		- [Algorithm [x]: ReLU](#algorithm-x-relu)
		- [Algorithm [x]: Max Pooling](#algorithm-x-max-pooling)
		- [Algorithm [x]: Linear](#algorithm-x-linear)
		- [Algorithm [x]: Log SoftMax](#algorithm-x-log-softmax)
- [Chapter 5: FPGA Implementation](#chapter-5-fpga-implementation)
- [Chapter 6: Results](#chapter-6-results)
- [Chapter 7: Conclusions and Future Work](#chapter-7-conclusions-and-future-work)
	- [7.1 Conclusions](#71-conclusions)
	- [7.2 Future Work](#72-future-work)
- [References](#references)
- [External Links](#external-links)
# List of Figures
# List of Tables
# List of Algorithms
<!-- Todo: Fix Reference Numbers -->
- [Algorithm [x]: Convolution](#algorithm-x-convolution)
- [Algorithm [x]: ReLU](#algorithm-x-relu)
- [Algorithm [x]: Max Pooling](#algorithm-x-max-pooling)
- [Algorithm [x]: Linear](#algorithm-x-linear)
- [Algorithm [x]: Log Soft Max](#algorithm-x-log-soft-max)
# List of Abbreviations
Abbreviation	| Meaning
--- 			| ---
AI				| **A**rtificial **I**ntelligence  
ANN				| **A**rtificial **N**eural **N**etwork  
B-RAM			| **B**lock **R**andom **A**ccess **M**emory
CNN				| **C**onvolutional **N**eural **N**etwork
CPU				| **C**entral **P**rocessor **U**nit  
D-RAM			| **D**ynamic **R**andom **A**ccess **M**emory
DNN				| **D**eep **N**eural **N**etwork  
DP				| **D**eep **L**earning  
DSP				| **D**igital **S**ignal **P**rocessor
FC				| **F**ully **C**onnected  
FF				| **F**lip **F**lops
FPGA			| **F**ield **P**rogrammable **G**ate **A**rray  
GPU				| **G**raphic **P**rocessor **U**nit  
HLS				| **H**igh **L**evel **S**ynthesis
LUT				| **L**ook **U**p **T**able
ML				| **M**achine **L**earning  
QFDB			| **Q**uad **F**PGA **D**aughter **B**oard
RAM				| **R**andom Access Memory
ReLU			| **R**ectified **L**inear **U**nit
SDK				| **S**oftware **D**evelopment **K**it
SLC				| **S**econd **L**evel **C**odebook
TPU				| **T**ensor **P**rocessor **U**nit  
# Chapter 1: Introduction
## 1.1 Motivation
## 1.2 Scientific Contributions
## 1.3 Thesis Outline
In this section this thesis organization is outlined.
* **Chapter 2:** Chapter 2 description
* **Chapter 3:** Chapter 3 description
* **Chapter 4:** Chapter 4 description
* **Chapter 5:** Chapter 5 description
* **Chapter 6:** Chapter 6 description
* **Chapter 7:** Chapter 7 description
# Chapter 2: Theoretical Background
In this chapter Machine Learning's and Convolutional Neural Networks' theoretical background is described in detail.
## 2.1 Machine Learning
## 2.2 Convolutional Neural Network
## 2.3 Structure of Convolutional Neural Network
### 2.3.1 Convolution Layer
### 2.3.2 Pooling
### 2.3.3 Activation Function
## 2.4 Typical Architecture of AlexNet
# Chapter 3: Related Work
## 3.1 Google Brain Project
### 3.1.1 DistBelief
### 3.1.2 TensorFlow
### 3.1.3 Tensor Processing Unit (TPU)
## 3.2 GPU Approach
## 3.3 The FPGA Perspective
## 3.4 AlexNet
## 3.5 Thesis Approach
# Chapter 4: AlexNet Robustness Analysis
## 4.1 Algorithms:
<!-- Todo: Fix Reference Numbers -->
### Algorithm [x]: Convolution
```
procedure Convolution (image, kernels, bias, stride, padding)
	Hout ← (image.height + 2 * padding - kernels.size) / stride + 1
	Wout ← (image.width + 2 * padding - kernels.size) / stride + 1

	for i:=1 to image.channels do
		for j:=1 to (image.height + 2 * padding) do
			for k:=1 to (image.width + 2 * padding) do
				arr(i, j, k) ← 0

	for i:=1 to image.channels do
		for j:=padding to (image.height + padding) do
			for k:=padding to (image.width + padding) do
				arr(i, j, k) ← image(i, j, k)

	for i:=1 to kernels.number do
		for j:=1 to Hout do
			for k:=1 to Wout do
				res(i, j, k) ← 0

	for out_channel:=1 to kernels.number do
		for oh:=1 to Hout do
			for ow:=1 to Wout do
				imgStartH ← oh * stride
				imgEndH ← imgStartH + kernels.size
				imgStartW ← ow * stride
				imgEndW ← imgStartW + kernels.size

				pixel ← 0
				for in_channel:=1 to image.channels do
					for i:=1 to kernels.size do
						for j:=1 to kernels.size do
							pixel ← arr(in_channel, i + imgStartH, j + imgStartW) * kernels(out_channel, in_channel, i, j)

				res(out_channel, oh, ow) ← pixel + bias(out_channel)
	return res
```
### Algorithm [x]: ReLU
```
procedure ReLU (input):
	for i:=1 to input.length do
		if input(i) > 0 then
			res(i) ← input(i)
		else
			res(i) ← 0
	return res
```
### Algorithm [x]: Max Pooling
```
procedure Max Pooling (image, kernel_size, stride):
	Hout ← (image.height - kernel_size) / stride + 1
	Wout ← (image.width - kernel_size) / stride + 1

	for i:=1 to image.channels do
		for j:=1 to Hout do
			for k:=1 to Wout do
				res(i, j, k) ← 0

	for channel:=1 to image.channels do
		for oh:=1 to Hout do
			for ow:=1 to Wout do
				a ← oh * stride
				b ← ow * stride
				
				max ← x(channel, a, b)
				for i:=a to (a + kernel_size) do
					for j:=b to (b + kernel_size) do
						if max < x(channel, i, j) then
							max ← x(channel, i, j)
				res(channel, oh, ow) ← max
	return res
```
### Algorithm [x]: Linear
```
procedure Linear (input, kernels, bias):
	for i:=1 to kernels.number do
		res(i) ← bias(i)
		for j:=1 to input.length do
			res(i) ← res(i) + input(j) * kernels(i, j)
	return res
```
### Algorithm [x]: Log SoftMax
```
procedure Log SoftMax (input):
	sum = 0
	for i:=1 to input.length do
		sum ← sum + exp(input(i))

	for i:=1 to input.length do
		res(i) ← log(exp(input(i)) / sum)
	return res
```
# Chapter 5: FPGA Implementation
# Chapter 6: Results
# Chapter 7: Conclusions and Future Work
## 7.1 Conclusions
## 7.2 Future Work
# References
<!-- Todo: Fix Reference Numbers -->
[x] Alex Krizhevsky, Ilya Sutskever, Geoffrey E. Hinton: *ImageNet classification with deep convolutional neural networks*, (2017) Communications of the ACM. 60 (6): 84–90. doi:10.1145/3065386. ISSN 0001-0782. *url:* https://papers.nips.cc/paper/4824-imagenet-classification-with-deep-convolutional-neural-networks.pdf.  
# External Links
<!-- Todo: Fix Reference Numbers -->
[x] Udacity Intro to Deep Learning with PyTorch by Facebook AI. *url:* https://www.udacity.com/course/deep-learning-pytorch--ud188.  
[x] Kaggle. *url:* https://www.kaggle.com/.  
[x] MATLAB. *url:* https://www.mathworks.com/.  
[x] TensorFlow. *url:* https://www.tensorflow.org/.  
[x] PyTorch. *url:* https://pytorch.org/.  
[x] Vivado Design Suite - HLx Editions. *url:* https://www.xilinx.com/products/design-tools/vivado.html.  
[x] ZCU102 User Guide. *url:* https://www.xilinx.com/support/documentation/boards_and_kits/zcu102/ug1182-zcu102-eval-bd.pdf.  

<!-- Reference links -->
[dollasPage]: https://www.ece.tuc.gr/index.php?id=4531&tx_tuclabspersonnel_list%5Bperson%5D=289&tx_tuclabspersonnel_list%5Baction%5D=person&tx_tuclabspersonnel_list%5Bcontroller%5D=List
[lagoudakisPage]: https://www.ece.tuc.gr/index.php?id=4531&tx_tuclabspersonnel_list%5Bperson%5D=313&tx_tuclabspersonnel_list%5Baction%5D=person&tx_tuclabspersonnel_list%5Bcontroller%5D=List
[kozanitisPage]: https://www.linkedin.com/in/christos-kozanitis-a3b173a8/
[tzanisPage]: https://www.linkedin.com/in/fotakistzanis/
[tucPage]: https://www.tuc.gr
[ecePage]: https://www.ece.tuc.gr/
[mhlPage]: https://www.mhl.tuc.gr/
