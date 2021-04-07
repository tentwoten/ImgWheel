#ifndef _POST_PROCESS_CHAIN_H_
#define _POST_PROCESS_CHAIN_H_
#include "ImgProcess.h"
extern "C" 
namespace IMat
{
	FNNOEXPORT int Img_Post_Process_Chain(uint16_t* psrc, uint16_t* pdst, int w, int h, const char* filePath, std::string& PostInfo);

}
#endif