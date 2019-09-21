clear;
close all;
clc;

global SAVE_PLOTS USE_SUBPLOTS
SAVE_PLOTS = true;
USE_SUBPLOTS = false;

PlotLayerCharacteristics(224, 55, 3, 96, 11, 4, 2, 'FilterFirst', 'Conv1')
PlotLayerCharacteristics(55, 27, 96, 96, 3, 2, 0, 'FilterFirst', 'MaxPool1')
PlotLayerCharacteristics(27, 27, 96, 256, 5, 1, 2, 'FilterFirst', 'Conv2')
PlotLayerCharacteristics(27, 13, 256, 256, 3, 2, 0, 'FilterFirst', 'MaxPool2')
PlotLayerCharacteristics(13, 13, 256, 384, 3, 1, 1, 'FilterFirst', 'Conv3')
PlotLayerCharacteristics(13, 13, 384, 384, 3, 1, 1, 'FilterFirst', 'Conv4')
PlotLayerCharacteristics(13, 13, 384, 256, 3, 1, 1, 'FilterFirst', 'Conv5')
PlotLayerCharacteristics(13, 6, 256, 256, 3, 2, 0, 'FilterFirst', 'MaxPool3')
