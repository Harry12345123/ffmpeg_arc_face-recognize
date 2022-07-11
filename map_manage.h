#ifndef _MAP_MANAGE_H
#define _MAP_MANAGE_H

#include <map>
#include <string>
#include <iostream>
#include "asfort_manage.h"

#define MAX_MAP_NUM (3)
using namespace std;


class S_THREAD_MAP{

  public:
     int map_id;
     map<string, ASF_FaceFeature> thread_map;
};




int init_map_manage_function();
int set_map_id(unsigned int task_id);
unsigned int get_map_id();

int set_thread_map(unsigned int task_id, S_THREAD_MAP *stream);
unsigned int get_thread_map(unsigned int task_id, S_THREAD_MAP *stream);


/*int set_decode_ffmpeg_stream(unsigned int task_id, S_DECODE_FFMPEG_STREAM *stream);
unsigned int get_decode_ffmpeg_stream(unsigned int task_id, S_DECODE_FFMPEG_STREAM *stream);
void print_decode_ffmpeg_stream(char *message, S_DECODE_FFMPEG_STREAM *stream);*/

#endif
