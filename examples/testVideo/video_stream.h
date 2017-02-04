#ifndef __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_STREAM_HEADER__
#define __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_STREAM_HEADER__

#include <stddef.h>

#include <string>
#include <queue>

#include <omicron/otypes.h>
#include <omicron/Thread.h>

#include "video_buf_pool.h"

namespace SUYUN_DISPLAY_SYSTEM
{
namespace RENDER
{
class VideoStream : public omicron::ReferenceType
{
public:
    enum VideoStreamType
    {
        VIDEO_STREAM_NONE,
        VIDEO_STREAM_TERMINAL,
        VIDEO_STREAM_PORT,
        VIDEO_STREAM_FFMPEG
    };
public:
    VideoStream( void );
    virtual ~VideoStream( void );

    bool init();
    virtual void release();

    virtual void updateFrame();
    VideoData* getVideoData();

    virtual VideoStreamType type() const = 0;

protected:
    VideoData* _videoData;
    std::queue<VideoData*> _videoDataBuf;
    omicron::Lock _videoDataMutex;

	VideoBufPool _pool;
};
}
}

#endif
