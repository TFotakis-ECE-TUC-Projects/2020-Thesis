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
	MaxPool,
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

XConv_core Conv_core_list[CONV_CORES_NUM];

XMaxpool_core Maxpool_core_list[MAXPOOL_CORES_NUM];

XLinear_core Linear_core_list[LINEAR_CORES_NUM];

#endif /* SRC_PLATFORM_CONF_H_ */
