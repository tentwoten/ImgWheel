#include "ImgProcess.h"
namespace IMat
{
	extern "C" FNNOEXPORT int Image_Log(float* psrc, float* pdst, int w, int h, int isNegative)
	{
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		int imgSize = w * h;
		if (isNegative == 1)
		{
			for (int i = 0; i < imgSize; ++i)
			{
				float val = psrc[i];
				val = val > 20 ? val : 20;
				pdst[i] = -log(val);
			}
		}
		else
		{
			for (int i = 0; i < imgSize; ++i)
			{
				float val = psrc[i];
				val = val > 20 ? val : 20;
				pdst[i] = log(val);
			}
		}
		return 1;
	}
};