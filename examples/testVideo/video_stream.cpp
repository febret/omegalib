#include "video_stream.h"

namespace SUYUN_DISPLAY_SYSTEM
{
namespace RENDER
{
VideoStream::VideoStream( void )
    : _videoData( NULL )
{
    omicron::AutoLock autolock( _videoDataMutex );
    while( _videoDataBuf.size() > 0 )
    {
        VideoData* video_data = _videoDataBuf.front();
        _videoDataBuf.pop();

        delete video_data;
    }
}

VideoStream::~VideoStream( void )
{
	release();
}

bool VideoStream::init()
{
	if( !_pool.init( 10 ) )
	{
		return false;
	}

    return true;
}

void VideoStream::release()
{
    omicron::AutoLock autolock( _videoDataMutex );
    while( _videoDataBuf.size() > 0 )
    {
        VideoData* video_data = _videoDataBuf.front();
        _videoDataBuf.pop();

        delete video_data;
    }
    if( _videoData )
    {
        delete _videoData;
        _videoData = NULL;
    }
	_pool.release();
}

VideoData* VideoStream::getVideoData()
{
    return _videoData;
}

void VideoStream::updateFrame()
{
    if( _videoData )
    {
        delete _videoData;
        _videoData = NULL;
    }
    omicron::AutoLock lock( _videoDataMutex );
    if( _videoDataBuf.size() == 0 )
    {
        return;
    }
    _videoData = _videoDataBuf.front();
    _videoDataBuf.pop();
}
}
}
