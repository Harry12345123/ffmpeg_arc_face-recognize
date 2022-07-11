#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "map_manage.h"

S_THREAD_MAP g_thread_maps[MAX_MAP_NUM];
pthread_mutex_t g_thread_maps_mutex;

#if 1
int init_map_manage_function()
{
    pthread_mutex_init(&g_thread_maps_mutex, NULL);
    memset(g_thread_maps, 0, sizeof(g_thread_maps));

    return 0;
}

unsigned int get_decode_ffmpeg_stream_task_id(unsigned int id)
{
    unsigned int task_id = 0;

    pthread_mutex_lock(&g_thread_maps_mutex);
    task_id = g_thread_maps[id].map_id;
    pthread_mutex_unlock(&g_thread_maps_mutex);
    
    return task_id;
}

int set_map_id(unsigned int map_id, unsigned int id)
{
    pthread_mutex_lock(&g_thread_maps_mutex);
    g_thread_maps[id].map_id = map_id;
    pthread_mutex_unlock(&g_thread_maps_mutex);
    
    return 0;
}
  
int set_thread_map(unsigned int map_id, S_THREAD_MAP *map)
{
    pthread_mutex_lock(&g_thread_maps_mutex);
    g_thread_maps[map_id] = *map;
    pthread_mutex_unlock(&g_thread_maps_mutex);
    return 0;
}

unsigned int get_thread_map(unsigned int map_id, S_THREAD_MAP *map)
{
    pthread_mutex_lock(&g_thread_maps_mutex);
    *map = g_thread_maps[map_id];
    pthread_mutex_unlock(&g_thread_maps_mutex);
    return 0;
}

#endif
