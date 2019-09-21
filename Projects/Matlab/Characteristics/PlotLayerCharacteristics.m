function PlotLayerCharacteristics(din, dout, ic, oc, k, s, p, order, layerName)
	global SAVE_PLOTS USE_SUBPLOTS
	if USE_SUBPLOTS
		figure('Name', layerName, 'units','normalized','outerposition',[0 0 0.5 1])
	end
	
	arr = OutputAddressGenerator(dout, oc, order);
	PrintAddressNew(arr, layerName);
	PrintMemoryLocality(arr, din, k, s, p, ic, layerName)
	
	if USE_SUBPLOTS && SAVE_PLOTS
		saveas(gcf, strcat('output/withSubplots/AlexNet-Characteristics-', layerName, '.png'));
	end
end
