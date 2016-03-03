#include <omega.h>

#include <thread>

#include "libstreamport.h"

class streamport
{
public:
    streamport()
    {
        _color = n;
        n += 10000;
        _w = 1920;
        _h = 1080;
        int size = _w * _h * 3 / 2;
        _buf = new char[ size ];
        memset( _buf, _color, size );
    }
    ~streamport()
    {

    }
    void thread_task( streamport* sp )
    {
        while( !sp->_stop )
        {
            if( _handler )
            {
                int size = _w * _h * 3 / 2;
                memset( _buf, _color++, size );
                _handler->on_video_frame_got( ( unsigned char* )_buf, _w, _h );
            }

            omega::osleep( 16 );	//for 60fps
        }
    }

    bool start()
    {
        _stop = false;
        std::thread t( &streamport::thread_task, this, this );
        t.detach();
        return true;
    }
    bool stop()
    {
        _stop = true;
        return true;
    }

    libstreamport_handler *_handler;
    bool _stop;
    int _color;
    void* _buf;
    int _w;
    int _h;

    static int n;
};

int streamport::n = 0;

libstreamport::libstreamport()
    : sp( NULL )
{
    sp = new streamport;
}

libstreamport::libstreamport( libstreamport_handler *_handler, int _output_format )
    : sp( NULL )
{
    sp = new streamport;
    sp->_handler = _handler;
}

libstreamport::~libstreamport()
{
}

void libstreamport::set_prama( libstreamport_handler *_handler, int _output_format )
{
    sp->_handler = _handler;
}

bool libstreamport::start_video( )
{
    return sp->start();
}

bool libstreamport::stop_video()
{
    return sp->stop();
}
