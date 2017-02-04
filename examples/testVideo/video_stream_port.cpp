#include "video_stream_port.h"

VideoStreamPort::VideoStreamPort( void )
    : _videoData( NULL )
{
}

VideoStreamPort::~VideoStreamPort( void )
{
}

bool VideoStreamPort::init()
{
    if( !_pool.init( 10 ) )
    {
        return false;
    }

    _videoSrc.set_prama( this, OUTPUT_FMT_YUV );

    return _videoSrc.start_video();
}

void VideoStreamPort::release()
{
    _videoSrc.stop_video();

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

void VideoStreamPort::updateFrame()
{
    omicron::AutoLock lock( _videoDataMutex );
    if( _videoData )
    {
        _pool.free( _videoData );
        _videoData = NULL;
    }
    if( _videoDataBuf.size() == 0 )
    {
        return;
    }
    _videoData = _videoDataBuf.front();
    _videoDataBuf.pop();
}

void VideoStreamPort::on_video_frame_got( unsigned char* buf, int w, int h )
{
    if( _pool.getFreeCount() == 0 )
    {
        omicron::AutoLock lock( _videoDataMutex );
        VideoData* videoData = _videoDataBuf.front();
        _videoDataBuf.pop();
        _pool.free( videoData );
    }
    int size = ( int )( w * h * 1.5 );
    VideoData* videoData = _pool.malloc( w, h, size );
    if( !videoData )
    {
        //DebugBreak();
    }
    else
    {
        memcpy( videoData->data, buf, size );
        videoData->w = w;
        videoData->h = h;
        omicron::AutoLock lock( _videoDataMutex );
        _videoDataBuf.push( videoData );
    }
}

VideoData* VideoStreamPort::getVideoData()
{
    return _videoData;
}
