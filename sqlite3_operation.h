#ifndef _SQLITE3_OPERATION_H
#define _SQLITE3_OPERATION_H

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "asfort_manage.h"
using namespace std;

int Connection_Sqlite3DataBase();
void insert_face_data_toDataBase(const char * name, MByte * face_feature, MInt32 featureSize);
//map<const unsigned char *, ASF_FaceFeature> QueryFaceFeature();
map<string, ASF_FaceFeature> QueryFaceFeature();
#endif