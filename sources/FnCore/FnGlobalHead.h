#ifndef _FNGLOBALHEAD_H_
#define _FNGLOBALHEAD_H_

#if defined(_WIN32)
#ifndef FNNOEXPORT
#define FNNOEXPORT __declspec(dllexport)
#endif // FNEXPORT
#else
#ifndef FNNOEXPORT
#include "math.h"
#include "float.h"
#include <climits>
#include "string.h"
#include "stdio.h"
#define FNNOEXPORT
#endif // FNEXPORT
#endif




#endif
