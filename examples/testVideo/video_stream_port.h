#ifndef __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_STREAM_PORT_HEADER__
#define __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_STREAM_PORT_HEADER__

#include <queue>

#include "libstreamport.h"
#include "video_buf_pool.h"

class VideoStreamPort
    : public libstreamport_handler
{
public:
    VideoStreamPort( void );
    ~VideoStreamPort( void );

    bool init();
    void release();

    void updateFrame();
    VideoData* getVideoData();

    virtual void on_video_frame_got( unsigned char* buf, int w, int h );

private:
    VideoData* _videoData;
    std::queue<VideoData*> _videoDataBuf;
    fast_mutex _videoDataMutex;

    VideoBufPool _pool;

    libstreamport _videoSrc;
};

#endif
