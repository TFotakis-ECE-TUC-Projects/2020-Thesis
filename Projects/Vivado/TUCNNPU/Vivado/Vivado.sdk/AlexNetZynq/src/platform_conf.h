#ifndef SRC_PLATFORM_CONF_H_
#define SRC_PLATFORM_CONF_H_

#include <xparameters.h>

typedef float matrix_t;

typedef enum {
	CONV_LAYER_TYPE,
	MAXPOOL_LAYER_TYPE,
	LINEAR_LAYER_TYPE,
	LINEAR_RELU_LAYER_TYPE
} LayerType;

typedef struct {
	LayerType layerType;
	unsigned int kernelSize;
	unsigned int stride;
	unsigned int padding;
	unsigned int din;
	unsigned int hin;
	unsigned int win;
	unsigned int dout;
	unsigned int hout;
	unsigned int wout;
	unsigned int inFeatures;
	unsigned int outFeatures;
	unsigned int xSize;
	unsigned int weightsSize;
	unsigned int biasSize;
	unsigned int resSize;
	matrix_t *xAddr;
	matrix_t *weightsAddr;
	matrix_t *biasAddr;
	matrix_t *resAddr;
	unsigned int doReLU;
} LayerConf;

unsigned int LAYERS_NUMBER;
LayerConf *LAYERS_CONF;

typedef struct {
	unsigned int InstanceId;
	void *InstancePtr;
} Core;

#ifdef XPAR_XCONV_CORE_NUM_INSTANCES
#include <xconv_core.h>
#define CONV_CORES_NUM XPAR_XCONV_CORE_NUM_INSTANCES
Core *Conv_core_list[CONV_CORES_NUM];
#endif

#ifdef XPAR_XMAXPOOL_CORE_NUM_INSTANCES
#include <xmaxpool_core.h>
#define MAXPOOL_CORES_NUM XPAR_XMAXPOOL_CORE_NUM_INSTANCES
Core *Maxpool_core_list[MAXPOOL_CORES_NUM];
#endif

#ifdef XPAR_XLINEAR_CORE_NUM_INSTANCES
#include <xlinear_core.h>
#define LINEAR_CORES_NUM XPAR_XLINEAR_CORE_NUM_INSTANCES
Core *Linear_core_list[LINEAR_CORES_NUM];
#endif

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
