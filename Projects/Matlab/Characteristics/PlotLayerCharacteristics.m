function PlotLayerCharacteristics(din, dout, ic, oc, k, s, p, order, layerName)
	figure('Name', layerName, 'units','normalized','outerposition',[0 0 1 1])
	arr = OutputAddressGenerator(dout, oc, order);
	PrintAddressNew(arr);
	PrintMemoryLocality(arr, din, k, s, p, ic)
end
