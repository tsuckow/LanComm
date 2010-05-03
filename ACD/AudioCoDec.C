#include "AudioCoDec.H"

int acdInitialize() {
	int failCode;
	acdReset();//Reset just for good measure.
	failCode=acdInitInterface();
	if (failCode) return failCode;
//	acdTestCommunication();
	return acdInitDevice();
}
int acdInitInterface() {
	return spiInitSPI1();
}
int acdInitDevice() {
	return 0;//Return no errors.
}

