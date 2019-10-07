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
	- [2.2 Artificial Neural Network](#22-artificial-neural-network)
	- [2.3 Convolutional Neural Network](#23-convolutional-neural-network)
	- [2.4 Structure of Convolutional Neural Network](#24-structure-of-convolutional-neural-network)
		- [2.3.1 Convolution Layer](#231-convolution-layer)
		- [2.3.2 Pooling](#232-pooling)
		- [2.3.3 Activation Function](#233-activation-function)
	- [2.5 Typical Architecture of AlexNet](#25-typical-architecture-of-alexnet)
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
	- [5.1 Characteristics](#51-characteristics)
	- [5.2 Scheduling](#52-scheduling)
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
<!-- spell-checker: disable -->
Abbreviation	| Meaning
--- 			| ---
AI				| **A**rtificial **I**ntelligence  
ANN				| **A**rtificial **N**eural **N**etwork  
B-RAM			| **B**lock **R**andom **A**ccess **M**emory
CNN				| **C**onvolutional **N**eural **N**etwork
CPU				| **C**entral **P**rocessor **U**nit  
CS				| **C**omputer **S**cience  
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
<!-- spell-checker: enable -->
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
The theoretical background of Machine Learning and Convolutional Neural Networks is being described below.

## 2.1 Machine Learning
Machine Learning (ML), the name of which was first proposed in 1959 by Arthur Samuel [1], is a subset of Artificial Intelligence (AI) and a Computer Science (CS) field that studies algorithms and statistical models capable of performing specific tasks, such as prediction or decision making, without being explicitly programmed. Instead, sample data are used, also known as "training data", for the machine to "learn" to distinguish useful patterns on the input data capable of creating the needed output, e.g., decision or prediction. There are numerous approaches [2] on the learning algorithms types, as well as on the model types used to get trained.

Such algorithm types, at the time of writing, include, but are not limited to:
- Supervised Learning: Algorithms that learn by using "labeled" sample data, data that contain both the inputs and their desired outputs to be used for classification and regression.
- Unsupervised Learning: In contrast with the Supervised Learning, "unlabeled" sample data are used to discover structures that could group or cluster them.
- Reinforcement Learning: Algorithms responsible for taking actions in an environment, often also described as software agents, to maximize a specific metric, many of which use dynamic programming techniques.
- Feature Learning: Algorithms that by combining or even discarding features from the input samples, try to create a new, more useful set of features. One of the most popular algorithms of this category is Principal Components Analysis (PCA).
- Anomaly Detection: Algorithms that try to identify outlier samples, which are characterized by their significant difference compared to the majority of the data used. Such algorithms are often used in noise reduction, data mining, and even security and defense systems.
- Association Rule Learning: Algorithms that aim to discover strong relationships between features.

Such model types, at the time of writing, include, but are not limited to:
- Artificial Neural Networks (ANN): Also known as Connectionist Systems, imitate the biological brain's neural networks.
- Decision Trees: Used to make assumptions about the input items' target value (the decision tree's leaves) via its observations (the decision tree's branches). When the target takes continuous values, the Decision Tree is called a Regression Tree.
- Support Vector Machines (SVM): Used for classification and regression, mostly famous as non-probabilistic, binary, linear classifiers. They can also be used for non-linear classification using the "kernel trick".
- Bayesian Networks: Represented as directed acyclic graphs, they can include probabilistic relationships.

Nowadays, most industries have already used Machine Learning in some sort, indicating the significance and variety of its capabilities. It is estimated [3] that by the year 2021, A.I. and M.L. spending will reach $57.6 Billion. Its applications include but are not limited to [4] [5], web page ranking, image recognition, email filtering and spam detection, database mining, handwriting recognition, speech recognition, natural language processing, computer vision, image/video/text/speech generation, personalized marketing, travelling, dynamic pricing, healthcare, facial & fingerprint recognition and intrusion detection.
## 2.2 Artificial Neural Network
It is widely accepted that the brain's greatest ability is pattern recognition, which is used to combine "data" from the organism's senses in a way to better understand its environment. Artificial Neural Networks (ANN), a highly popular sub-field of Machine Learning, try to imitate the brain's structure to solve such problems, a structure that has been developing and proving its capabilities for thousands of years.

While ANNs are inspired by the biological neural networks, they are not identical. A neural network is a collection of connected neurons, through which electrical signals from sensor organs or other neurons are passed and processed. A biological neuron is comprised of four main parts; Dendrites, Cell body, Axon and Synaptic terminals (Figure 1). A Dendrite and its Dendritic branches are used as the neuron's input, where sensors or other neurons get connected. A neuron can have multiple Dendrites. The neuron's cell body collects all the input signals and applies an "activation" function to create the output signal. Afterwards, the output signal is transported through the Axon and then distributed to the next neurons through the Synaptic terminals. The Synaptic terminals to Dendrites connections are called Synapses.

![](Documentation/Assets/Biological-Neuron.png)  
Figure 1. Standard structure of a biological neuron. [URL](https://nurseslabs.com/nervous-system/)

Similarly to the biological neural networks, an ANN can be represented as a graph, whose vertices represent the biological neurons' cell bodies and its edges the biological synapses. The electrical signal used in biological neurons is represented as a real number and their outputs are calculated by some non-linear function of the inputs' weighted sum. Each edge typically has a weight set during the training process, which amplifies or weakens the edge's signal. 

<!-- Todo: Add Basic components, e.g. neurons, activations, etc -->
<!-- Todo: Explain what is a DNN -->
<!-- Todo: Add NN architectures types, e.g. CNN, RNN, LSTM, etc  -->
## 2.3 Convolutional Neural Network
## 2.4 Structure of Convolutional Neural Network
### 2.3.1 Convolution Layer
### 2.3.2 Pooling
### 2.3.3 Activation Function
## 2.5 Typical Architecture of AlexNet
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
## 5.1 Characteristics
Module | Pixel usage frequency | Pixel usage frequency histogram | Output creation time
:---:|:---:|:---:|:---:
Conv 1 | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv1-pixel-frequency.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv1-pixel-histogram.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv1-output-creation-time.png)
Conv 2 | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv2-pixel-frequency.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv2-pixel-histogram.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv2-output-creation-time.png)
Conv 3 | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv3-pixel-frequency.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv3-pixel-histogram.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv3-output-creation-time.png)
Conv 4 | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv4-pixel-frequency.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv4-pixel-histogram.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv4-output-creation-time.png)
Conv 5 | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv5-pixel-frequency.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv5-pixel-histogram.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/Conv5-output-creation-time.png)
MaxPool 1 | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool1-pixel-frequency.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool1-pixel-histogram.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool1-output-creation-time.png)
MaxPool 2 | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool2-pixel-frequency.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool2-pixel-histogram.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool2-output-creation-time.png)
MaxPool 3 | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool3-pixel-frequency.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool3-pixel-histogram.png) | ![](Documentation/Assets/FPGA-Implementation/Characteristics/MaxPool3-output-creation-time.png)
## 5.2 Scheduling
Architecture | Serial | Pipelined
:---: | :---: | :---:
No Parallelism | ![](Documentation/Assets/FPGA-Implementation/Scheduling/Serial.png) | ![](Documentation/Assets/FPGA-Implementation/Scheduling/Pipelined-1x.png)
Kernel Parallelism | ![](Documentation/Assets/FPGA-Implementation/Scheduling/Serial-Kernels-Parallelism.png) | ![](Documentation/Assets/FPGA-Implementation/Scheduling/Pipelined-Kernels-Parallelism.png)
Kernel & Channels Parallelism | ![](Documentation/Assets/FPGA-Implementation/Scheduling/Serial-K-x-OC-Parallelism.png) | ![](Documentation/Assets/FPGA-Implementation/Scheduling/Pipelined-K-x-OC-Parallelism.png)

Chip Frequency: 100 MHz  
Serial: 7501.22 ms  
Pipelined 1x: 2842.86 ms  
Serial Kernels Parallelism: 501.24 ms  
Pipelined Kernels Parallelism: 172.87 ms  
Serial K*OC Parallelism: 7.27 ms  
Pipelined K*OC Parallelism: 3.48 ms  
# Chapter 6: Results
# Chapter 7: Conclusions and Future Work
## 7.1 Conclusions
## 7.2 Future Work
# References
<!-- Todo: Fix Reference Numbers -->
[1] Samuel, Arthur (1959). "Some Studies in Machine Learning Using the Game of Checkers". IBM Journal of Research and Development. 3 (3): 210–229. CiteSeerX 10.1.1.368.2254. doi:10.1147/rd.33.0210. *url:* https://ieeexplore.ieee.org/document/5392560.  
[x] Alex Krizhevsky, Ilya Sutskever, Geoffrey E. Hinton: *ImageNet classification with deep convolutional neural networks*, (2017) Communications of the ACM. 60 (6): 84–90. doi:10.1145/3065386. ISSN 0001-0782. *url:* https://papers.nips.cc/paper/4824-imagenet-classification-with-deep-convolutional-neural-networks.pdf.  
# External Links
<!-- Todo: Fix Reference Numbers -->
[2] Machine learning - Wikipedia, (23 September 2019) *url:* https://en.wikipedia.org/wiki/Machine_learning#Approaches.  
[5] Roundup Of Machine Learning Forecasts And Market Estimates, 2018
[3] Machine Learning – Applications *url:* https://www.geeksforgeeks.org/machine-learning-introduction/  
[4] Top Machine Learning Applications in 2019 *url:* https://www.geeksforgeeks.org/top-machine-learning-applications-in-2019/  
 *url:* https://www.forbes.com/sites/louiscolumbus/2018/02/18/roundup-of-machine-learning-forecasts-and-market-estimates-2018/#536446aa2225  
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
