#include "video_buf_pool.h"

VideoData::VideoData()
    : data( NULL )
    , w( 0 )
    , h( 0 )
    , size( 0 )
{
}

VideoData::VideoData( int w, int h, int size )
{
    this->w = w;
    this->h = h;
    this->size = size;
    this->data = new char[ size ];
}

VideoData::~VideoData()
{
    if( data )
    {
        delete[] data;
        data = NULL;
    }
}

void VideoData::resize( int w, int h, int size )
{
    if( data )
    {
        delete[] data;
        data = NULL;
    }

    this->w = w;
    this->h = h;
    this->size = size;
    this->data = new char[ size ];
}

VideoBufPool::VideoBufPool()
    : _maxSize( 0 )
{
}

VideoBufPool::~VideoBufPool()
{
    release();
}

bool VideoBufPool::init( int maxSize )
{
    _maxSize = maxSize;
    for( int i = 0; i < maxSize; ++i )
    {
        _bufs.push( new VideoData() );
    }
    return true;
}

void VideoBufPool::release()
{
    omicron::AutoLock autolock( _mutex );
    while( !_bufs.empty() )
    {
        VideoData* buf = _bufs.top();
        _bufs.pop();
        delete buf;
    }
}

int VideoBufPool::getMaxSize()
{
    omicron::AutoLock autolock( _mutex );
    return _maxSize;
}

int VideoBufPool::getFreeCount()
{
    omicron::AutoLock autolock( _mutex );
    return _bufs.size();
}

VideoData* VideoBufPool::malloc( int w, int h, int size )
{
    omicron::AutoLock autolock( _mutex );
    if( _bufs.empty() )
    {
        return NULL;
    }
    VideoData* buf = _bufs.top();
    _bufs.pop();

    if( buf->size != size )
    {
        buf->resize( w, h, size );
    }
    return buf;
}

void VideoBufPool::free( VideoData* buf )
{
    omicron::AutoLock autolock( _mutex );
    _bufs.push( buf );
}
