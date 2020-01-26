#ifndef SRC_PLATFORM_CONF_H_
#define SRC_PLATFORM_CONF_H_

#include <xparameters.h>
#include <xconv_core.h>
#include <xmaxpool_core.h>
#include <xlinear_core.h>

#define CONV_CORES_NUM XPAR_XCONV_CORE_NUM_INSTANCES
#define MAXPOOL_CORES_NUM XPAR_XMAXPOOL_CORE_NUM_INSTANCES
#define LINEAR_CORES_NUM XPAR_XLINEAR_CORE_NUM_INSTANCES

#define BASE_ADDR 0x50000000

typedef double matrix_t;

typedef enum {
	Conv,
	Maxpool,
	LinearReLU,
	Linear
} LayerType;

typedef struct {
	LayerType layerType;
	u32 kernelSize;
	u32 stride;
	u32 padding;
	u32 din;
	u32 hin;
	u32 win;
	u32 dout;
	u32 hout;
	u32 wout;
	u32 inFeatures;
	u32 outFeatures;
	u32 xSize;
	u32 weightsSize;
	u32 biasSize;
	u32 resSize;
	matrix_t *xAddr;
	matrix_t *weightsAddr;
	matrix_t *biasAddr;
	matrix_t *resAddr;
	u32 doReLU;
} LayerConf;

typedef struct {
	unsigned int InstanceId;
	void *InstancePtr;
} Core;

Core *Conv_core_list[CONV_CORES_NUM];

Core *Maxpool_core_list[MAXPOOL_CORES_NUM];

Core *Linear_core_list[LINEAR_CORES_NUM];

#if CONV_CORES_NUM == 1
const unsigned int Conv_core_device_ids[CONV_CORES_NUM] = {
	XPAR_XCONV_CORE_0_DEVICE_ID,
};
const unsigned int Conv_core_interrupt_ids[CONV_CORES_NUM] = {
	XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
};
volatile static int Conv_core_result_avail[CONV_CORES_NUM] = {
	0,
};
#elif CONV_CORES_NUM == 2
const unsigned int Conv_core_device_ids[CONV_CORES_NUM] = {
	XPAR_XCONV_CORE_0_DEVICE_ID,
	XPAR_XCONV_CORE_1_DEVICE_ID,
};
const unsigned int Conv_core_interrupt_ids[CONV_CORES_NUM] = {
	XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_1_INTERRUPT_INTR,
};
volatile static int Conv_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
};
#elif CONV_CORES_NUM == 3
const unsigned int Conv_core_device_ids[CONV_CORES_NUM] = {
	XPAR_XCONV_CORE_0_DEVICE_ID,
	XPAR_XCONV_CORE_1_DEVICE_ID,
	XPAR_XCONV_CORE_2_DEVICE_ID,
};
const unsigned int Conv_core_interrupt_ids[CONV_CORES_NUM] = {
	XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_2_INTERRUPT_INTR,
};
volatile static int Conv_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
};
#elif CONV_CORES_NUM == 4
const unsigned int Conv_core_device_ids[CONV_CORES_NUM] = {
	XPAR_XCONV_CORE_0_DEVICE_ID,
	XPAR_XCONV_CORE_1_DEVICE_ID,
	XPAR_XCONV_CORE_2_DEVICE_ID,
	XPAR_XCONV_CORE_3_DEVICE_ID,
};
const unsigned int Conv_core_interrupt_ids[CONV_CORES_NUM] = {
	XPAR_FABRIC_CONV_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_2_INTERRUPT_INTR,
	XPAR_FABRIC_CONV_CORE_3_INTERRUPT_INTR,
};
volatile static int Conv_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
	0,
};
#endif

#if MAXPOOL_CORES_NUM == 1
const unsigned int Maxpool_core_device_ids[MAXPOOL_CORES_NUM] = {
	XPAR_XMAXPOOL_CORE_0_DEVICE_ID,
};
const unsigned int Maxpool_core_interrupt_ids[MAXPOOL_CORES_NUM] = {
	XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
};
volatile static int Maxpool_core_result_avail[CONV_CORES_NUM] = {
	0,
};
#elif MAXPOOL_CORES_NUM == 2
const unsigned int Maxpool_core_device_ids[MAXPOOL_CORES_NUM] = {
	XPAR_XMAXPOOL_CORE_0_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_1_DEVICE_ID,
};
const unsigned int Maxpool_core_interrupt_ids[MAXPOOL_CORES_NUM] = {
	XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_1_INTERRUPT_INTR,
};
volatile static int Maxpool_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
};
#elif MAXPOOL_CORES_NUM == 3
const unsigned int Maxpool_core_device_ids[MAXPOOL_CORES_NUM] = {
	XPAR_XMAXPOOL_CORE_0_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_1_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_2_DEVICE_ID,
};
const unsigned int Maxpool_core_interrupt_ids[MAXPOOL_CORES_NUM] = {
	XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_2_INTERRUPT_INTR,
};
volatile static int Maxpool_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
};
#elif MAXPOOL_CORES_NUM == 4
const unsigned int Maxpool_core_device_ids[MAXPOOL_CORES_NUM] = {
	XPAR_XMAXPOOL_CORE_0_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_1_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_2_DEVICE_ID,
	XPAR_XMAXPOOL_CORE_3_DEVICE_ID,
};
const unsigned int Maxpool_core_interrupt_ids[MAXPOOL_CORES_NUM] = {
	XPAR_FABRIC_MAXPOOL_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_2_INTERRUPT_INTR,
	XPAR_FABRIC_MAXPOOL_CORE_3_INTERRUPT_INTR,
};
volatile static int Maxpool_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
	0,
};
#endif

#if LINEAR_CORES_NUM == 1
const unsigned int Linear_core_device_ids[LINEAR_CORES_NUM] = {
	XPAR_XLINEAR_CORE_0_DEVICE_ID,
};
const unsigned int Linear_core_interrupt_ids[LINEAR_CORES_NUM] = {
	XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
};
volatile static int Linear_core_result_avail[CONV_CORES_NUM] = {
	0,
};
#elif LINEAR_CORES_NUM == 2
const unsigned int Linear_core_device_ids[LINEAR_CORES_NUM] = {
	XPAR_XLINEAR_CORE_0_DEVICE_ID,
	XPAR_XLINEAR_CORE_1_DEVICE_ID,
};
const unsigned int Linear_core_interrupt_ids[LINEAR_CORES_NUM] = {
	XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_1_INTERRUPT_INTR,
};
volatile static int Linear_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
};
#elif LINEAR_CORES_NUM == 3
const unsigned int Linear_core_device_ids[LINEAR_CORES_NUM] = {
	XPAR_XLINEAR_CORE_0_DEVICE_ID,
	XPAR_XLINEAR_CORE_1_DEVICE_ID,
	XPAR_XLINEAR_CORE_2_DEVICE_ID,
};
const unsigned int Linear_core_interrupt_ids[LINEAR_CORES_NUM] = {
	XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_2_INTERRUPT_INTR,
};
volatile static int Linear_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
};
#elif LINEAR_CORES_NUM == 4
const unsigned int Linear_core_device_ids[LINEAR_CORES_NUM] = {
	XPAR_XLINEAR_CORE_0_DEVICE_ID,
	XPAR_XLINEAR_CORE_1_DEVICE_ID,
	XPAR_XLINEAR_CORE_2_DEVICE_ID,
	XPAR_XLINEAR_CORE_3_DEVICE_ID,
};
const unsigned int Linear_core_interrupt_ids[LINEAR_CORES_NUM] = {
	XPAR_FABRIC_LINEAR_CORE_0_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_1_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_2_INTERRUPT_INTR,
	XPAR_FABRIC_LINEAR_CORE_3_INTERRUPT_INTR,
};
volatile static int Linear_core_result_avail[CONV_CORES_NUM] = {
	0,
	0,
	0,
	0,
};
#endif

#endif /* SRC_PLATFORM_CONF_H_ */
