#ifndef __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_BUF_POOL_HEADER__
#define __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_BUF_POOL_HEADER__

#include <stack>

#include <omicron/Thread.h>

struct VideoData
{
    VideoData();
    VideoData( int w, int h, int size );
    ~VideoData();
    void resize( int w, int h, int size );
    char* data;
    int w;
    int h;
    int size;
};

class VideoBufPool
{
public:
    VideoBufPool();
    ~VideoBufPool();

    bool init( int maxSize );
    void release();

    int getMaxSize();
    int getFreeCount();

    VideoData* malloc( int w, int h, int size );
    void free( VideoData* buf );

private:
    std::stack< VideoData* > _bufs;
    int _maxSize;

    fast_mutex _mutex;
};

#endif
