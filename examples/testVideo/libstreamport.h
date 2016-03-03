#pragma once

#include <string>

class streamport;

#define OUTPUT_FMT_YUV 0
#define OUTPUT_FMT_RGB 1

class libstreamport_handler
{
public:
    virtual void on_video_frame_got( unsigned char *buf, int width, int height ) = 0;
};

class libstreamport
{
public:
    libstreamport();
    libstreamport( libstreamport_handler *_handler, int _output_format = OUTPUT_FMT_YUV );
    virtual ~libstreamport();

    void set_prama( libstreamport_handler *_handler, int _output_format = OUTPUT_FMT_YUV );

    bool start_video( );
    bool stop_video();

protected:
    streamport *sp;
};