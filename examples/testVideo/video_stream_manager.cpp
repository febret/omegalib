#include "video_stream_manager.h"

#include "video_stream_port.h"

VideoStreamManager* VideoStreamManager::instance()
{
    static VideoStreamManager mgr;
    return &mgr;
}

VideoStreamManager::VideoStreamManager( void )
{
}

VideoStreamManager::~VideoStreamManager( void )
{
}

VideoStreamPort* VideoStreamManager::getVideoStream( )
{
    omicron::AutoLock autolock( _mutex );
    VideoStreamPort* vsd = new VideoStreamPort;
    vsd->init( );
    _videoStreams.push_back( vsd );
    return vsd;
}

void VideoStreamManager::delVideoStream( VideoStreamPort* video_stream )
{
    omicron::AutoLock autolock( _mutex );
    for( std::list< VideoStreamPort* >::iterator it = _videoStreams.begin();
         it != _videoStreams.end(); ++it )
    {
        VideoStreamPort* vs = *it;
        if( vs == video_stream )
        {
            vs->release();
            delete vs;
            _videoStreams.erase( it );
            return;
        }
    }
}

void VideoStreamManager::updateAllStreams()
{
    foreach( VideoStreamPort* vs, _videoStreams )
    {
        vs->updateFrame();
    }
}

bool VideoStreamManager::init()
{
    return true;
}

void VideoStreamManager::release()
{
    omicron::AutoLock autolock( _mutex );
    for( std::list< VideoStreamPort* >::iterator it = _videoStreams.begin();
         it != _videoStreams.end(); ++it )
    {
        VideoStreamPort* vs = *it;
        vs->release();
        delete vs;
    }
    _videoStreams.clear();
}
