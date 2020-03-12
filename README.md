# Diploma Thesis
![Technical-University-of-Crete-TUC-logo](Documentation/Assets/Technical-University-of-Crete-logo.png)  
[Technical University of Crete][tucPage]

**Reconfigurable-based Design and Implementation of AlexNet Deep Learning Network**  

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
ASIC			| **A**pplication **S**pecific **I**ntegrated **C**ircuit
B-RAM			| **B**lock **R**andom **A**ccess **M**emory
CNN				| **C**onvolutional **N**eural **N**etwork
CPU				| **C**entral **P**rocessor **U**nit
CS				| **C**omputer **S**cience
DDR4			| **D**ouble **D**ata **R**ate type 4 memory
D-RAM			| **D**ynamic **R**andom **A**ccess **M**emory
DNN				| **D**eep **N**eural **N**etwork
DP				| **D**eep **L**earning
DSP				| **D**igital **S**ignal **P**rocessor
FC				| **F**ully **C**onnected
FF				| **F**lip **F**lops
FPGA			| **F**ield **P**rogrammable **G**ate **A**rray
FORTH			| **Fo**undation of **R**esearch and **T**echnology **H**ellas
GDDR6			| **G**raphics **D**ouble **D**ata **R**ate type **6** memory
GPU				| **G**raphic **P**rocessor **U**nit
HBM				| **H**igh **B**andwidth **M**emory
HLS				| **H**igh **L**evel **S**ynthesis
HPC				| **H**ight **P**erformance **C**omputing
LUT				| **L**ook **U**p **T**able
ML				| **M**achine **L**earning
MPSoC			| **M**ulti **P**rocessor **S**ystem **o**n **C**hip
QFDB			| **Q**uad **F**PGA **D**aughter **B**oard
RAM				| **R**andom Access Memory
ReLU			| **R**ectified **L**inear **U**nit
SDK				| **S**oftware **D**evelopment **K**it
SLC				| **S**econd **L**evel **C**odebook
SSD				| **S**olid **S**tate **D**rive
TDP				| **T**hermal **D**esign **P**ower
TPU				| **T**ensor **P**rocessor **U**nit
USD				| **U**nited **S**tates **D**ollar
<!-- spell-checker: enable -->
# Chapter 1: Introduction
Since the invention of the first computer, humankind is rapidly solving problems that are intellectually difficult for human beings but relatively easy for computers, as such problems can be described in detail with a formal list of mathematical rules. However, problems that are easy for humans, that are solved intuitively, like distinguishing the difference between a car and a person, or a spoken word and a bird's chirp, is a real challenge for computers and engineers [[8]](#references). Those problems cannot be described, at the time of writing, with sharply defined mathematical rules. Artificial Intelligence (AI) and Machine Learning (ML) study those types of problems, with many successes in the cost of highly computationally complex algorithms.  
It is estimated that by the year 2025, the total amount of data created worldwide will rise to 163 ZettaBytes, while every minute of the year 2019, Americans used more than 4.4 PetaBytes of data [[6]](#external-links). It is evident that data management systems and knowledge extraction from them, also called Data Analysis, are urgent. Although such problems can be tackled using Artificial Intelligence and Machine Learning, it is extremely computationally intensive, if not even non-feasible, in a reasonable amount of time.  
Fortunately, most of the algorithms used to tackle such problems come with great parallelism. Therefore, they can be expanded in the space domain, in other words, they can utilize more hardware resources in order to cut down on needs from the time domain. Of course, there are many different types of hardware resources, each one of them with their advantages and disadvantages, from parallelism capabilities and energy efficiency to cost of production, reconfigurability and reusability.
## 1.1 Motivation
Nowadays, the computational complexity of the aforementioned algorithms makes hardware acceleration a necessity, since running them on Central Processing Units (CPUs) is, while possible, the least efficient and fast solution. Although writing software for CPUs may be fast and easy, its running speed due to low parallelism and high power consumption, as a general propose piece of hardware, are far from ideal. For reference, at the time of writing, a top grade server CPU, AMD EPYC 7002 Series, can provide up to 64 cores and 128 threads, at up to 2.25GHz base clock and 3.4GHz boost clock, with a rated Thermal Design Power (TDP) of 225Watts, and a list price of 4,425 USD [[7]](#external-links). 

![AMD-EPYC-Chip](Documentation/Assets/Hardware/amd-epyc.png)  
![AMD-EPYC-Chip](Documentation/Assets/Hardware/amd-epyc-dies.png)  
[AMD Epyc 7002 series chip.](https://www.amd.com/en/processors/epyc-7002-series)

Graphics Processing Units (GPUs), on the other hand, provide much higher parallelism, while still being relatively easy for their software to be written. However, they can be costly to scale up, and their power consumption can be really high. For reference, at the time of writing, a top grade GPU for ML, NVIDIA Titan RTX, provides up to 72 Streaming Multiprocessors, up to 4,608 CUDA Cores and up to 576 Tensor cores, with a rated base clock of 1,350 MHz and boost clock of 1,770 MHz, 24 GB of Graphics Double Data Rate (GDDR6) Memory and a power consumption of 280 Watts at a price of 2,500 USD [[9]](#external-links).  

![NVIDIA-Titan-RTX](Documentation/Assets/Hardware/NVIDIA-Titan-RTX.png)  
[NVIDIA Titan RTX card.](https://www.nvidia.com/en-us/deep-learning-ai/products/titan-rtx/)

Moreover, there are Application Specific Integrated Circuits (ASICs), which for a particular application can provide the best parallelism capabilities and the lowest power consumption. Unfortunately, they are very expensive to develop and produce, and they can only serve a single purpose, a single application. An example of such an ASIC is the Google Cloud Tensor Processing Unit (TPU), which, for the third version (v3), in a single chip there are two TPU cores, each of which contains two scalar, vector and matrix units (MXUs), and 16 GB of High Bandwidth Memory (HBM) [[10]](#external-links).  

![TPU-v3](Documentation/Assets/Hardware/tpu-v3.png)  
[Google's TPU v3 - 4 chips, 2 cores per chip.](https://cloud.google.com/tpu/docs/system-architecture)

Field Programmable Gate Arrays (FPGAs), on the contrary, are bridging the gap between the GPUs' flexibility and the ASICs' performance and power consumption. An example FPGA Hardware targeted for High Performance Computing (HPC) is the Quad-FPGA Daughter Board (QFDB) [[12]](#references), developed by the Foundation of Research and Technology Hellas (FORTH) [[11]](#external-links), combines 4 interconnected Xilinx Zynq Ultrascale+ Multi Processor Systems on Chip (MPSoCs), with 16GB of DDR4 memory and a M.2 Solid State Drive(SSD).

![QFDB-top-and-bottom-view](Documentation/Assets/Hardware/QFDB.png)  
[FORTH QFDB, top-view (left) and bottom-view (right).](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=8945720)

In this work, the FPGAs' benefits are being utilized in order to create a hardware accelerator that can speed up the inference of Convolutional Neural Networks (CNNs), a branch of Deep Neural Networks (DNNs), which is a subfield of Machine Learning.  
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
Machine Learning, the name of which was first proposed in 1959 by Arthur Samuel [[1]](#references), is a subset of Artificial Intelligence and a Computer Science (CS) field that studies algorithms and statistical models capable of performing specific tasks, such as prediction or decision making, without being explicitly programmed. Instead, sample data are used, also known as "training data", for the machine to "learn" to distinguish useful patterns on the input data capable of creating the needed output, e.g., decision or prediction. There are numerous approaches [[2]](#external-links) on the learning algorithms types, as well as on the model types used to get trained.
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

Nowadays, most industries have already used Machine Learning in some sort, indicating the significance and variety of its capabilities. It is estimated [[3]](#external-links) that by the year 2021, A.I. and M.L. spending will reach $57.6 Billion. Its applications include but are not limited to [[4]](#external-links) [[5]](#external-links), web page ranking, image recognition, email filtering and spam detection, database mining, handwriting recognition, speech recognition, natural language processing, computer vision, image/video/text/speech generation, personalized marketing, travelling, dynamic pricing, healthcare, facial & fingerprint recognition and intrusion detection.
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
<!-- spell-checker: disable -->
<!-- Todo: Fix Reference Numbers -->
[1] Samuel, Arthur (1959). "Some Studies in Machine Learning Using the Game of Checkers". IBM Journal of Research and Development. 3 (3): 210–229. CiteSeerX 10.1.1.368.2254. doi:10.1147/rd.33.0210. *url:* https://ieeexplore.ieee.org/document/5392560.  
[8] Ian Goodfellow, Yoshua Bengio, Aaron Courville. (2016) Deep Learning (page 1), MIT Press. *url:* http://www.deeplearningbook.org/.  
[12] F. Chaix, A.D. Ioannou, N. Kossifidis, N. Dimou, G. Ieronymakis, M. Marazakis, V. Papaefstathiou, V. Flouris, M. Ligerakis, G. Ailamakis, T.C. Vavouris, A. Damianakis, M. G. H. Katevenis, I. Mavroidis (2019). Implementation and impact of an ultra-compact multi-FPGA board for large system prototyping. 2019 IEEE/ACM International Workshop on Heterogeneous High-performance Reconfigurable Computing (H2RC). *url:* https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=8945720.
[x] Alex Krizhevsky, Ilya Sutskever, Geoffrey E. Hinton: *ImageNet classification with deep convolutional neural networks*, (2017) Communications of the ACM. 60 (6): 84–90. doi:10.1145/3065386. ISSN 0001-0782. *url:* https://papers.nips.cc/paper/4824-imagenet-classification-with-deep-convolutional-neural-networks.pdf.  
<!-- spell-checker: enable -->
# External Links
<!-- spell-checker: disable -->
<!-- Todo: Fix Reference Numbers -->
[2] Machine learning - Wikipedia, (23 September 2019) *url:* https://en.wikipedia.org/wiki/Machine_learning#Approaches.  
[3] Machine Learning – Applications *url:* https://www.geeksforgeeks.org/machine-learning-introduction/  
[4] Top Machine Learning Applications in 2019 *url:* https://www.geeksforgeeks.org/top-machine-learning-applications-in-2019/  
[5] Roundup Of Machine Learning Forecasts And Market Estimates, 2018
 *url:* https://www.forbes.com/sites/louiscolumbus/2018/02/18/roundup-of-machine-learning-forecasts-and-market-estimates-2018/#536446aa2225  
[6] Forbes - How Much Data Is Collected Every Minute Of The Day (August 7 2019). *url:* https://www.forbes.com/sites/nicolemartin1/2019/08/07/how-much-data-is-collected-every-minute-of-the-day/#747555a33d66.
[7] AMD EPYC™ 7002 Series Processors. *url:* https://www.amd.com/en/processors/epyc-7002-series.
[9] NVIDIA Titan RTX GPU. *url:* https://www.nvidia.com/en-us/deep-learning-ai/products/titan-rtx/.
[10] Google Cloud TPU. *url:* https://cloud.google.com/tpu/docs/system-architecture.
[11] Foundation of Research and Technology Hellas (FORTH). *url:* https://www.forth.gr/.  
[x] Udacity Intro to Deep Learning with PyTorch by Facebook AI. *url:* https://www.udacity.com/course/deep-learning-pytorch--ud188.  
[x] Kaggle. *url:* https://www.kaggle.com/.  
[x] MATLAB. *url:* https://www.mathworks.com/.  
[x] TensorFlow. *url:* https://www.tensorflow.org/.  
[x] PyTorch. *url:* https://pytorch.org/.  
[x] Vivado Design Suite - HLx Editions. *url:* https://www.xilinx.com/products/design-tools/vivado.html.  
[x] ZCU102 User Guide. *url:* https://www.xilinx.com/support/documentation/boards_and_kits/zcu102/ug1182-zcu102-eval-bd.pdf.  
<!-- spell-checker: enable -->

<!-- Reference links -->
[dollasPage]: https://www.ece.tuc.gr/index.php?id=4531&tx_tuclabspersonnel_list%5Bperson%5D=289&tx_tuclabspersonnel_list%5Baction%5D=person&tx_tuclabspersonnel_list%5Bcontroller%5D=List
[lagoudakisPage]: https://www.ece.tuc.gr/index.php?id=4531&tx_tuclabspersonnel_list%5Bperson%5D=313&tx_tuclabspersonnel_list%5Baction%5D=person&tx_tuclabspersonnel_list%5Bcontroller%5D=List
[kozanitisPage]: https://www.linkedin.com/in/christos-kozanitis-a3b173a8/
[tzanisPage]: https://www.linkedin.com/in/fotakistzanis/
[tucPage]: https://www.tuc.gr
[ecePage]: https://www.ece.tuc.gr/
[mhlPage]: https://www.mhl.tuc.gr/
