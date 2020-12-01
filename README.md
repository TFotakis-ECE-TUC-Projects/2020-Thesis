# Diploma Thesis
<!-- ![Technical-University-of-Crete-TUC-logo](https://raw.githubusercontent.com/TFotakis/Thesis/master/Projects/LaTeX/Images/TUC_logo.png =200x100)  
[Technical University of Crete][tucPage] -->

<a href="https://www.tuc.gr">
  <img src="https://raw.githubusercontent.com/TFotakis/Thesis/master/Projects/LaTeX/Images/TUC_logo.png" alt="tuc_logo" width="200"/>
</a>

**Analysis and design methodology of convolutional neural networks mapping on reconfigurable logic**  

Author:  
[Tzanis Fotakis][tzanisPage]

Thesis Committee:  
[Prof. Apostolos Dollas][dollasPage],  
[Prof. Michail Lagoudakis][lagoudakisPage],  
[Prof. Sotirios Ioannidis][ioannidisPage]

*A thesis submitted in fulfillment of the requirements for the Diploma of Electrical and Computer Engineering in the [Microprocessor and Hardware Lab][mhlPage] of [School of Electrical and Computer Engineering][ecePage]*

<!-- Todo: Add Date Here -->
# Abstract
Over the last few years, Convolutional Neural Networks have proved their abilities in several fields of study, with the research community continuing to surprise the world with new and paradoxical use cases, and even more exciting results. The rise of neural networks in general, and especially CNNs, creates a necessity for hardware acceleration of such computationally complex applications to achieve high-performance and energy-efficiency. Due to the fact that neural networks are highly parallelizable, they can exploit FPGA's hardware flexibility. This study presents a hardware platform targeted for FPGA devices for easy and structured implementation of neural network inference accelerators. It is designed with flexibility and versatility in mind, capable of being transferred to various FPGA devices. Furthermore, it is extendable to enable for easy adding of new layer types and new layer accelerators. In addition, it is scalable for multi-FPGA implementations, using platforms such as the FORTH QFDB, a custom four-FPGA platform. Moreover, it can run various CNN models' inference, but most importantly, it provides easy experimentation and development of neural networks hardware accelerator architectures. The proposed platform is implemented for accelerating AlexNet's inference, an award-winning CNN whose robustness analysis is carried out to investigate the FPGA's strengths and weaknesses, studying the computational workloads, memory access patterns, memory and bandwidth reduction, as well as algorithmic optimizations. A comparison in inference performance metrics is presented between the proposed platform, a CPU, a GPU, and other Xilinx developed neural network accelerator platforms. Although there are no performance benefits of using an FPGA over a modern GPU, a potential for performance improvements appears with further development, focusing on the convolution accelerator, which exploits the platform's ease of use, extendability, and expandability.


Technologies used:  
• Xilinx Vivado IDE  
• Xilinx Vivado HLS  
• Xilinx Vitis IDE  
• Matlab  
• PyTorch  
• Convolutional Neural Networks (CNNs)  
• Xilinx ZCU102 FPGA Evaluation Kit  

This thesis and its presentation can be found [here](https://dias.library.tuc.gr/view/86843).

<!-- Reference links -->
[dollasPage]: https://www.ece.tuc.gr/index.php?id=4531&tx_tuclabspersonnel_list%5Bperson%5D=289&tx_tuclabspersonnel_list%5Baction%5D=person&tx_tuclabspersonnel_list%5Bcontroller%5D=List
[lagoudakisPage]: https://www.ece.tuc.gr/index.php?id=4531&tx_tuclabspersonnel_list%5Bperson%5D=313&tx_tuclabspersonnel_list%5Baction%5D=person&tx_tuclabspersonnel_list%5Bcontroller%5D=List
[ioannidisPage]:https://www.tuc.gr/index.php?id=5639&L=612%27A%3D0&tx_tuclabspersonnel_pi3%5Bpersonid%5D=707
[tzanisPage]: https://www.linkedin.com/in/fotakistzanis/
[tucPage]: https://www.tuc.gr
[ecePage]: https://www.ece.tuc.gr/
[mhlPage]: https://www.mhl.tuc.gr/
