#include "ImgProcess.h"
#include "FnCore/Log_xx.h"
#include "omp.h"

using namespace std;
namespace IMat
{
	//加速版1.4  保证进来的数据都是整型,利用查表计算exp,可进行原位操作
#define v_1_6
#ifdef v_1_4
	extern "C" FNNOEXPORT int FastNLM(uint8_t * psrc, uint8_t * pdst, int img_w, int img_h, float h, int rd, int rD)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		bool isSituOperation = false;
		if (pdst == psrc)
		{
			pdst = new uchar[(uint64)img_w * img_h];
			isSituOperation = true;
		}
		int expand = (rD + rd);
		int d = rd * 2 + 1;
		//总的搜索框，需要进行边界拓展
		int temp_h = img_h + expand * 2;
		int temp_w = img_w + expand * 2;
		uchar* ptemp = new uchar[(uint64)temp_h * temp_w];
		copyBorder(psrc, ptemp, img_h, img_w, expand, expand, expand, expand);
		uchar* ptotal = ptemp + (uint64)temp_w * rD + rD;//p1指向第rD行，第rD列
		//这是原图加搜索范围rd的模板窗口,再大图temp里面进行逐个点差，以便与积分图的计算
		int paddrd_w = img_w + rd * 2;
		int paddrd_h = img_h + rd * 2;
		uchar* paddrd = new uchar[(uint64)paddrd_w * paddrd_h];
		copyBorder(psrc, paddrd, img_h, img_w, rd, rd, rd, rd);
		///////用于计算积分图的框
		int difsum_w = paddrd_w + 1;
		int difsum_h = paddrd_h + 1;
		int64* pdifsum = new int64[(uint64)difsum_h * difsum_w];
		memset(pdifsum, 0, sizeof(int64) * (difsum_w));
		for (int y = 0; y < difsum_h; ++y)
			pdifsum[y * difsum_w] = 0;
		//记录每个坐标的权重和，用于归一化
		float* psweight = new float[(uint64)img_h * img_w];
		memset(psweight, 0, sizeof(float) * img_h * img_w);
		//记录每个坐标权重乘以像素值的和
		float* paverage = new float[(uint64)img_h * img_w];
		memset(paverage, 0, sizeof(float) * img_h * img_w);
		//搜索框
		int paddrD_w = img_w + rD * 2;
		int paddrD_h = img_h + rD * 2;
		uchar* paddrD = new uchar[(uint64)paddrD_h * paddrD_w];
		copyBorder(psrc, paddrD, img_h, img_w, rD, rD, rD, rD);
		uchar* paddvDp = paddrD + (int64)rD * paddrD_w + rD;  //

		float ratio = 1.0f / ((h * h) * (d * d));
		int exptabnum = 256 * 256;
		float* exptab = new float[exptabnum];
		for (int i = 0; i < exptabnum; ++i)
			exptab[i] = exp(-i * ratio);

		for (int sy = -rD; sy <= rD; ++sy)
		{
			for (int sx = -rD; sx <= rD; ++sx)
			{
				//获得偏差值,计算积分图
				int xx = 0;
				for (int py = 0; py < paddrd_h; ++py)
				{
					int64* pdifsum_uprow = pdifsum + (uint64)difsum_w * (py);
					int64* pdifsum_row = pdifsum + (uint64)difsum_w * (uint64)(py + 1);
					int64 res = 0;
					for (int px = 0; px < paddrd_w; ++px)
					{
						int index1 = paddrd_w * py + px;
						int index2 = temp_w * (py + sy) + px + sx;
						int64 val1 = (int64)paddrd[index1];
						int64 val2 = (int64)ptotal[index2];
						int64 dif = val1 - val2;
						res += (dif * dif);
						pdifsum_row[px + 1] = pdifsum_uprow[px + 1] + res;
					}
				}
				uchar* paddvDt = paddvDp + (uint64)sy * paddrD_w + sx;
				for (int y = 0; y < img_h; ++y)
				{
					int64* up_row = pdifsum + y * (difsum_w);
					int64* down_row = pdifsum + (y + d) * (difsum_w);
					for (int x = 0; x < img_w; ++x)
					{
						int64 dist2 = (up_row[x] - up_row[x + d] - down_row[x] + down_row[x + d]);
						float weight = dist2 < exptabnum ? exptab[dist2] : exp(-dist2 * ratio);
						psweight[y * img_w + x] += weight;
						paverage[y * img_w + x] += paddvDt[y * paddrD_w + x] * weight;
					}
				}
				int yy = 0;
			}
		}
		int imgSize = img_h * img_w;
		for (int i = 0; i < imgSize; ++i)
		{
			float val = paverage[i] / psweight[i] + 0.5f;
			val = min(val, 255.0f);
			val = max(val, 0.0f);
			pdst[i] = val;
		}
		delete[]ptemp;
		delete[]paddrd;
		delete[]pdifsum;
		delete[]psweight;
		delete[]paverage;
		delete[]paddrD;
		delete[]exptab;
		if (isSituOperation == true)
		{
			memcpy(psrc, pdst, (uint64)img_h * img_w * sizeof(uchar));
			delete[]pdst;
			pdst = psrc;
		}
		return 1;
	}
#endif

#ifdef v_1_5
	extern "C" FNNOEXPORT int FastNLM(uint8_t * psrc, uint8_t * pdst, int img_w, int img_h, float h, int rd, int rD)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		bool isSituOperation = false;
		if (pdst == psrc)
		{
			pdst = new uchar[(uint64)img_w * img_h];
			isSituOperation = true;
		}
		int expand = (rD + rd);
		int d = rd * 2 + 1;
		//总的搜索框，需要进行边界拓展
		int temp_h = img_h + expand * 2;
		int temp_w = img_w + expand * 2;
		uchar* ptemp = new uchar[(uint64)temp_h * temp_w];
		copyBorder(psrc, ptemp, img_h, img_w, expand, expand, expand, expand);
		uchar* ptotal = ptemp + (uint64)temp_w * rD + rD;//p1指向第rD行，第rD列
		//这是原图加搜索范围rd的模板窗口,再大图temp里面进行逐个点差，以便与积分图的计算
		int paddrd_w = img_w + rd * 2;
		int paddrd_h = img_h + rd * 2;
		uchar* paddrd = new uchar[(uint64)paddrd_w * paddrd_h];
		copyBorder(psrc, paddrd, img_h, img_w, rd, rd, rd, rd);
		///////用于计算积分图的框
		int difsum_w = paddrd_w + 1;
		int difsum_h = paddrd_h + 1;
		int64* pdifsum = new int64[(uint64)difsum_h * difsum_w];
		memset(pdifsum, 0, sizeof(int64) * (difsum_w));
		for (int y = 0; y < difsum_h; ++y)
			pdifsum[y * difsum_w] = 0;
		//记录每个坐标的权重和，用于归一化
		float* psweight = new float[(uint64)img_h * img_w];
		memset(psweight, 0, sizeof(float) * img_h * img_w);
		//记录每个坐标权重乘以像素值的和
		float* paverage = new float[(uint64)img_h * img_w];
		memset(paverage, 0, sizeof(float) * img_h * img_w);
		//搜索框
		int paddrD_w = img_w + rD * 2;
		int paddrD_h = img_h + rD * 2;
		uchar* paddrD = new uchar[(uint64)paddrD_h * paddrD_w];
		copyBorder(psrc, paddrD, img_h, img_w, rD, rD, rD, rD);
		uchar* paddvDp = paddrD + (int64)rD * paddrD_w + rD;  //

		float ratio = 1.0f / ((h * h) * (d * d));
		int exptabnum = 65536;
		float* exptab = new float[exptabnum];
		for (int i = 0; i < exptabnum; ++i)
			exptab[i] = exp(-i * ratio);
		for (int sy = -rD; sy <= rD; ++sy)
		{
			for (int sx = -rD; sx <= rD; ++sx)
			{
				//获得偏差值,计算积分图
				int xx = 0;
				for (int py = 0; py < paddrd_h; ++py)
				{
					int64* pdifsum_uprow = pdifsum + (uint64)difsum_w * (py);
					int64* pdifsum_row = pdifsum + (uint64)difsum_w * (uint64)(py + 1);
					int64 res = 0;
					for (int px = 0; px < paddrd_w; ++px)
					{
						int dif = (int)paddrd[paddrd_w * py + px] - (int)ptotal[temp_w * (py + sy) + px + sx];
						res += (dif * dif);
						pdifsum_row[px + 1] = pdifsum_uprow[px + 1] + res;
					}
				}
				uchar* paddvDt = paddvDp + (uint64)sy * paddrD_w + sx;
				for (int y = 0; y < img_h; ++y)
				{
					int64* up_row = pdifsum + y * (difsum_w);
					int64* down_row = pdifsum + (y + d) * (difsum_w);
					for (int x = 0; x < img_w; ++x)
					{
						int64 dist2 = (up_row[x] - up_row[x + d] - down_row[x] + down_row[x + d]);
						float weight = dist2 < exptabnum ? exptab[dist2] : exp(-dist2 * ratio);
						psweight[y * img_w + x] += weight;
						paverage[y * img_w + x] += paddvDt[y * paddrD_w + x] * weight;
					}
				}
				int yy = 0;
			}
		}
		int imgSize = img_h * img_w;
		for (int i = 0; i < imgSize; ++i)
		{
			float val = paverage[i] / psweight[i] + 0.5f;
			val = min(val, 255.0f);
			val = max(val, 0.0f);
			pdst[i] = val;
		}
		delete[]ptemp;
		delete[]paddrd;
		delete[]pdifsum;
		delete[]psweight;
		delete[]paverage;
		delete[]paddrD;
		delete[]exptab;
		if (isSituOperation == true)
		{
			memcpy(psrc, pdst, (uint64)img_h * img_w * sizeof(uchar));
			delete[]pdst;
			pdst = psrc;
		}
		return 1;
	}
#endif


	//#ifdef v_1_6
	//extern "C" IMGPROCESS_API int FastNLM(uint8_t* psrc, uint8_t* pdst, int img_w, int img_h, float h, int rd, int rD)
	//{
	//	if (psrc == NULL) return -1;
	//	if (pdst == NULL) return -2;
	//	const int thread_num = 8;
	//
	//
	//	bool isSituOperation = false;
	//	if (pdst == psrc)
	//	{
	//		pdst = new uchar[(uint64)img_w * img_h];
	//		isSituOperation = true;
	//	}
	//	int expand = (rD + rd);
	//	int d = rd * 2 + 1;
	//	//总的搜索框，需要进行边界拓展
	//	int temp_h = img_h + expand * 2;
	//	int temp_w = img_w + expand * 2;
	//	uchar* ptemp = new uchar[(uint64)temp_h * temp_w];
	//	copyBorder(psrc, ptemp, img_h, img_w, expand, expand, expand, expand);
	//	uchar* ptotal = ptemp + (uint64)temp_w * rD + rD;//p1指向第rD行，第rD列
	//	//这是原图加搜索范围rd的模板窗口,再大图temp里面进行逐个点差，以便与积分图的计算
	//	int paddrd_w = img_w + rd * 2;
	//	int paddrd_h = img_h + rd * 2;
	//	uchar* paddrd = new uchar[(uint64)paddrd_w * paddrd_h];
	//	copyBorder(psrc, paddrd, img_h, img_w, rd, rd, rd, rd);
	//	///////用于计算积分图的框
	//	int difsum_w = paddrd_w + 1;
	//	int difsum_h = paddrd_h + 1;
	//	int64* pdifsum = new int64[(uint64)difsum_h * difsum_w];
	//	memset(pdifsum, 0, sizeof(int64) * (difsum_w));
	//	for (int y = 0; y < difsum_h; ++y)
	//		pdifsum[y * difsum_w] = 0;
	//	//记录每个坐标的权重和，用于归一化
	//	float* psweight = new float[(uint64)img_h * img_w]();
	//	//记录每个坐标权重乘以像素值的和
	//	float* paverage = new float[(uint64)img_h * img_w]();
	//	//搜索框
	//	int paddrD_w = img_w + rD * 2;
	//	int paddrD_h = img_h + rD * 2;
	//	uchar* paddrD = new uchar[(uint64)paddrD_h * paddrD_w];
	//	copyBorder(psrc, paddrD, img_h, img_w, rD, rD, rD, rD);
	//	uchar* paddvDp = paddrD + (int64)rD * paddrD_w + rD;  //
	//
	//	float ratio = 1.0f / ((h * h) * (d * d));
	//	int exptabnum = 256 * 256;
	//	float* exptab = new float[exptabnum];
	//	for (int i = 0; i < exptabnum; ++i)
	//		exptab[i] = exp(-i * ratio);
	//	int DD = 2 * rD + 1;
	//	int _sum = DD * DD;
	//
	//#pragma omp parallel num_threads(thread_num)
	//	for (int ID=0;ID< _sum;++ID)
	//	{
	//		int thread_id = omp_get_thread_num();
	//		int sy = ID / DD - rD;
	//		int sx = ID % DD - rD;
	//		FastNLMKer(sy,sx,paddrd_h,paddrd_w,pdifsum,difsum_w,difsum_h,temp_w,paddrd,ptotal,
	//			paddvDp,paddrD_w,img_w,img_h,d,exptab,exptabnum,ratio,psweight,paverage);
	//
	//	}
	//	int imgSize = img_h * img_w;
	//	for (int i = 0; i < imgSize; ++i)
	//	{
	//		float val = paverage[i] / psweight[i] + 0.5f;
	//		val = min(val, 255.0f);
	//		val = max(val, 0.0f);
	//		pdst[i] = val;
	//	}
	//	delete[]ptemp;
	//	delete[]paddrd;
	//	delete[]pdifsum;
	//	delete[]psweight;
	//	delete[]paverage;
	//	delete[]paddrD;
	//	delete[]exptab;
	//	if (isSituOperation == true)
	//	{
	//		memcpy(psrc, pdst, (uint64)img_h * img_w * sizeof(uchar));
	//		delete[]pdst;
	//		pdst = psrc;
	//	}
	//	return 1;
	//}
	//#endif

	void FastNLMKer(int sy, int sx, int paddrd_h, int paddrd_w, int64_t* pdifsum, int difsum_w, int difsum_h, int temp_w, uint8_t* paddrd, uint8_t* ptotal,
		uint8_t* paddvDp, int paddrD_w, int img_w, int img_h, int d, float* exptab, int exptabnum, float ratio, float* psweight, float* paverage)
	{
		for (int py = 0; py < paddrd_h; ++py)
		{
			int64_t* pdifsum_uprow = pdifsum + (uint64_t)difsum_w * (py);
			int64_t* pdifsum_row = pdifsum + (uint64_t)difsum_w * (uint64_t)(py + (uint64_t)1);
			int64_t res = 0;
			for (int px = 0; px < paddrd_w; ++px)
			{
				int index1 = paddrd_w * py + px;
				int index2 = temp_w * (py + sy) + px + sx;
				int64_t val1 = (int64_t)paddrd[index1];
				int64_t val2 = (int64_t)ptotal[index2];
				int64_t dif = val1 - val2;
				res += (dif * dif);
				pdifsum_row[px + 1] = pdifsum_uprow[px + 1] + res;
			}
		}

		uint8_t* paddvDt = paddvDp + (uint64_t)sy * paddrD_w + sx;
		for (int y = 0; y < img_h; ++y)
		{
			int64_t* up_row = pdifsum + y * (difsum_w);
			int64_t* down_row = pdifsum + (y + d) * (difsum_w);
			for (int x = 0; x < img_w; ++x)
			{
				int64_t dist2 = (up_row[x] - up_row[x + d] - down_row[x] + down_row[x + d]);
				float weight = dist2 < exptabnum ? exptab[dist2] : exp(-dist2 * ratio);
				psweight[y * img_w + x] += weight;
				paverage[y * img_w + x] += paddvDt[y * paddrD_w + x] * weight;
			}
		}
	}
#ifdef v_1_6
	extern "C" FNNOEXPORT int FastNLM(uint8_t * psrc, uint8_t * pdst, int img_w, int img_h, float h, int rd, int rD)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		const int nt = 4;
		int expand = (rD + rd);
		int d = rd * 2 + 1;
		//总的搜索框，需要进行边界拓展
		int temp_h = img_h + expand * 2;
		int temp_w = img_w + expand * 2;
		uint8_t* ptemps = new uint8_t[(uint64_t)temp_h * temp_w * nt];
		copyBorder(psrc, ptemps, img_h, img_w, expand, expand, expand, expand);
		uint8_t* ptotal = ptemps + (uint64_t)temp_w * rD + rD;//p1指向第rD行，第rD列
		//这是原图加搜索范围rd的模板窗口,再大图temp里面进行逐个点差，以便与积分图的计算
		int paddrd_w = img_w + rd * 2;
		int paddrd_h = img_h + rd * 2;
		uint8_t* paddrds = new uint8_t[(uint64_t)paddrd_w * paddrd_h * nt];
		copyBorder(psrc, paddrds, img_h, img_w, rd, rd, rd, rd);

		///////用于计算积分图的框
		int difsum_w = paddrd_w + 1;
		int difsum_h = paddrd_h + 1;
		int64_t* pdifsums = new int64_t[(uint64_t)difsum_h * difsum_w * nt]();
		//记录每个坐标的权重和，用于归一化
		float* psweights = new float[(uint64_t)img_h * img_w * nt]();
		//记录每个坐标权重乘以像素值的和
		float* paverages = new float[(uint64_t)img_h * img_w * nt]();
		//搜索框
		int paddrD_w = img_w + rD * 2;
		int paddrD_h = img_h + rD * 2;
		uint8_t* paddrDs = new uint8_t[(uint64_t)paddrD_h * paddrD_w * nt];

		copyBorder(psrc, paddrDs, img_h, img_w, rD, rD, rD, rD);
		uint8_t* paddvDp = paddrDs + (int64_t)rD * paddrD_w + rD;  //

		float ratio = 1.0f / ((h * h) * (d * d));
		int exptabnum = 256 * 256;
		float* exptab = new float[exptabnum];
		for (int i = 0; i < exptabnum; ++i)
			exptab[i] = exp(-i * ratio);
		int DD = 2 * rD + 1;
		int _sum = DD * DD;
		for (int i = 1; i < nt; ++i)
		{
			memcpy(ptemps + (uint64_t)i * temp_h * temp_w, ptemps, sizeof(uint8_t) * temp_h * temp_w);
			memcpy(paddrds + (uint64_t)i * paddrd_w * paddrd_h, paddrds, sizeof(uint8_t) * paddrd_w * paddrd_h);
			memcpy(paddrDs + (uint64_t)i * paddrD_h * paddrD_w, paddrDs, sizeof(uint8_t) * paddrD_h * paddrD_w);
		}

		omp_set_num_threads(nt);
#pragma omp parallel for
		for (int ID = 0; ID < _sum; ++ID)
		{
			int thread_id = omp_get_thread_num();
			int sy = ID / DD - rD;
			int sx = ID % DD - rD;
			int64_t* pdifsum = pdifsums + (int64_t)difsum_h * difsum_w * thread_id; //积分图
			uint8_t* paddrd = paddrds + (int64_t)paddrd_w * paddrd_h * thread_id;
			float* psweight = psweights + (int64_t)img_h * img_w * thread_id;
			float* paverage = paverages + (int64_t)img_h * img_w * thread_id;
			uint8_t* paddvDp = paddrDs + ((int64_t)paddrD_h * paddrD_w * thread_id) + (int64_t)rD * paddrD_w + rD;  //
			FastNLMKer(sy, sx, paddrd_h, paddrd_w, pdifsum, difsum_w, difsum_h, temp_w, paddrd, ptotal,
				paddvDp, paddrD_w, img_w, img_h, d, exptab, exptabnum, ratio, psweight, paverage);

		}
		int imgSize = img_h * img_w;
		for (int k = 1; k < nt; ++k)
		{
			for (int i = 0; i < imgSize; ++i)
			{
				psweights[i] += psweights[i + k * imgSize];
				paverages[i] += paverages[i + k * imgSize];
			}
		}

		for (int i = 0; i < imgSize; ++i)
		{
			float val = paverages[i] / psweights[i] + 0.5f;
			val = min(val, 255.0f);
			val = max(val, 0.0f);
			pdst[i] = (uint8_t)val;
		}

		delete[]ptemps;
		delete[]paddrds;
		delete[]pdifsums;
		delete[]psweights;
		delete[]paverages;
		delete[]paddrDs;
		delete[]exptab;
		return 1;
	}
#endif
	extern "C" FNNOEXPORT int FastNLM16(uint16_t * psrc, uint16_t * pdst, int img_w, int img_h, float h, int rd, int rD)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		uint32_t pixelcount = img_w * img_h;
		uint8_t* psrc8 = new uint8_t[pixelcount];
		uint8_t* pdst8 = new uint8_t[pixelcount];

		for (uint32_t i = 0; i < pixelcount; ++i)
		{
			int val = (int)(psrc[i] / 257.0f + 0.5f);
			val = max(min(val, 255), 0);
			psrc8[i] = (uint8_t)val;
		}
		FastNLM(psrc8, pdst8, img_w, img_h, h, rd, rD);
		int dif, val;
		for (uint32_t i = 0; i < pixelcount; ++i)
		{
			dif = (int)pdst8[i] - (int)psrc8[i];
			val = dif * 257 + (int)psrc[i];
			val = max(min(val, 65535), 0);
			pdst[i] = (uint16_t)val;
		}
		delete[]psrc8;
		delete[]pdst8;
		return 1;
	}

	namespace IMat
	{
		int FastNLMean(Mat& src, Mat& dst, float h, int rd, int rD)
		{
			if (src.data == nullptr)
			{
				PrintInfo("FastNLM: src is nullptr");
				return -1;
			}
			PixelForm m_format = src.m_Format;
			if (m_format == PixelForm::U8)
			{
				return FastNLM(src.data, dst.data, src.cols, src.rows, h, rd, rD);
			}
			else if (m_format == PixelForm::U16)
			{
				return FastNLM16((uint16_t*)src.data, (uint16_t*)dst.data, src.cols, src.rows, h, rd, rD);
			}
			return 1;
		}
	};
};