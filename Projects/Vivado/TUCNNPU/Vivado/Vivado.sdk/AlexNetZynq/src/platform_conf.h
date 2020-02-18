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
	unsigned int InstanceId;
	void *InstancePtr;
} Core;

/** Contains all necessary data for an image to be stored in memory. */
typedef struct {
	/** Image's height */
	unsigned int height;
	/** Image's width */
	unsigned int width;
	/** Image's color profile (RGB -> 3, BW -> 1, CMYK -> 4, etc.) */
	unsigned int depth;
	/**
	 * Holds RGB data after jpeg decompression, one color in each cell,
	 * flattened row by row
	 */
	unsigned char *bmp_buffer;
	/** Size of bmp buffer in cells */
	unsigned long bmp_size;
	/** Image's path */
	char *path;
	/**
	 * A 3-dimensional representation of an image.
	 * First dimension: Color channel
	 * Second dimension: Image's rows in pixels
	 * Third dimension: Image's columns in pixels
	 *
	 * Valid pixel values: unsigned integers in range (0, 255).
	 */
	unsigned char ***channels;
} Image;

/** Structure to contain file paths */
typedef struct {
	/** Length of the list (number of paths stored) */
	unsigned int length;
	/** List of strings to store filepaths */
	char **list;
} Filelist;

/**
 * Struct to store a multi-dimensional matrix of type matrix_t in a buffer-like
 * representation.
 */
typedef struct {
	unsigned int dimsNum; /**< Size of dims array */
	unsigned int *dims;   /**< Stores the size of each dimension */
	/**
	 * Stores the matrix_t data as a 1D buffer by flattening every dimension.
	 *
	 * Its size is defined by:
	 * dims[0] * dims[1] * dims[2] * ... * dims[dimsNum -1] * sizeof(matrix_t)
	 *
	 * To access data from the i-th plain, j-th row, k-th column calculate an
	 * index as follows:
	 * index = k + j * dims[2] + i * dims[1] * dims[2]
	 *
	 * For ease of calculating indices use calc3DIndex or calc4DIndex.
	 */
	matrix_t *matrix;
} FloatMatrix;

/** Structure to store the network's parameters */
typedef struct {
	unsigned int len;	 /** The number of parameter matrices */
	FloatMatrix **matrix; /** The 1D array of FloatMartix cells to store the
							 parameters */
} Params;

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

typedef struct {
	unsigned int layersNum;
	LayerConf *layersConf;
	Params *params;
	char **labels;
	Filelist *imagesPaths;
} NetConf;

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
