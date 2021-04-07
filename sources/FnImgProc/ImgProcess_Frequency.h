#ifndef IMGPROCESS_FREQUENCY_H_
#define IMGPROCESS_FREQUENCY_H_
#include "FnCore/FnGlobalEnum.h"
#include "FnCore/FnGlobalHead.h"

namespace IMat
{
	extern"C" FNNOEXPORT int Dft1D(float* psrc, float* real, float* imag, int w);

	extern"C" FNNOEXPORT int Dft2D(float* psrc, float* real, float* imag, int w, int h);

	extern"C" FNNOEXPORT int Idft1D(float* psrc, float* real, float* imag, int w);

	extern"C" FNNOEXPORT int Idft2D(float* psrc, float* real, float* imag, int w, int h);
}
#endif // !IMGPROCESS_FREQUENCY_H_