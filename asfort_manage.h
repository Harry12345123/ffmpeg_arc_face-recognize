#ifndef _ASFORT_MANAGE_H
#define _ASFORT_MANAGE_H

#include <opencv2/dnn.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "arcsoft_face_sdk.h"
#include "amcomdef.h"
#include "asvloffscreen.h"
#include "merror.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define APPID "4CP3UGEdWtsP8AAHLqfMzE2rtQQiwSoXR11PY1GUN68u"
#define SDKKEY "3e2FJNHbrUt1QsxdVT25SCdUJEk8z7XRKt8z87xnThLR"

#define APPID_02 "4CP3UGEdWtsP8AAHLqfMzE37DCw3Fi5Dbo4uaKBCYQcp"
#define SDKKEY_02 "6kGH8dkMxBVngU4vDK1giw4RHb4rgae9jsAwon17aUSW"

#define NSCALE 16 
#define FACENUM	5

void CutIplImage(IplImage *src, IplImage *dst, int x, int y);

int ColorSpaceConversion(IplImage* image, MInt32 format, ASVLOFFSCREEN& offscreen);

int ColorSpaceConversion(MInt32 width, MInt32 height, MInt32 format, MUInt8* imgData, ASVLOFFSCREEN& offscreen);

int init_asfort_device( MPChar appid, MPChar sdkkey);


#endif