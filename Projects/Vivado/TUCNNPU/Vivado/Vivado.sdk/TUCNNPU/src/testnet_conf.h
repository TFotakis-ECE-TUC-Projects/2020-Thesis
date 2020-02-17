#ifndef SRC_ALEXNET_CONF_H_
#define SRC_ALEXNET_CONF_H_

#define LAYERS_NUMBER 11

LayerConf LAYERS_CONF_DEF[] = {
	{
		.layerType = CONV_LAYER_TYPE,
		.kernelSize = 11,
		.stride = 4,
		.padding = 2,
		.din = 3,
		.hin = 224,
		.win = 224,
		.dout = 64,
	},
	{
		.layerType = MAXPOOL_LAYER_TYPE,
		.kernelSize = 3,
		.stride = 2,
	},
	{
		.layerType = CONV_LAYER_TYPE,
		.kernelSize = 5,
		.stride = 1,
		.padding = 2,
		.dout = 192,
	},
	{
		.layerType = MAXPOOL_LAYER_TYPE,
		.kernelSize = 3,
		.stride = 2,
	},
	{
		.layerType = CONV_LAYER_TYPE,
		.kernelSize = 3,
		.stride = 1,
		.padding = 1,
		.dout = 384,
	},
	{
		.layerType = CONV_LAYER_TYPE,
		.kernelSize = 3,
		.stride = 1,
		.padding = 1,
		.dout = 256,
	},
	{
		.layerType = CONV_LAYER_TYPE,
		.kernelSize = 3,
		.stride = 1,
		.padding = 1,
		.dout = 256,
	},
	{
		.layerType = MAXPOOL_LAYER_TYPE,
		.kernelSize = 3,
		.stride = 2,
	},
	{
		.layerType = LINEAR_RELU_LAYER_TYPE,
		.outFeatures = 4096,
	},
	{
		.layerType = LINEAR_RELU_LAYER_TYPE,
		.outFeatures = 4096,
	},
	{
		.layerType = LINEAR_LAYER_TYPE,
		.outFeatures = 1000,
	},
};

#endif /* SRC_ALEXNET_CONF_H_ */
