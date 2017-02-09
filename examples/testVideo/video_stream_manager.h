#ifndef __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_STREAM_MANAGER_HEADER__
#define __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_STREAM_MANAGER_HEADER__

#include <list>

#include <omicron/otypes.h>
#include <omicron/Thread.h>

#include "video_stream_port.h"

class VideoStreamManager
{
public:
    static VideoStreamManager* instance();

    bool init();
    void release();

    ~VideoStreamManager( void );

    VideoStreamPort* getVideoStream( );
    void delVideoStream( VideoStreamPort* video_stream );

    void updateAllStreams();

private:
    VideoStreamManager( void );

private:
    std::list< VideoStreamPort* > _videoStreams;
	fast_mutex _mutex;
};

#endif
