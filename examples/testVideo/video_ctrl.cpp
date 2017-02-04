#include "video_ctrl.h"
#include "video_stream.h"
#include "video_stream_manager.h"

using namespace omega;
using namespace omegaToolkit::ui;

VideoCtrl::VideoCtrl( Engine* e )
    : Widget( e )
    , _videoStream( NULL )
    , _subtitleX( 0 )
    , _subtitleY( 0 )
    , _fontSize( 60 )
    , _usePbo( true )
    , _showBorder( true )
    , _borderColor( 1.f, 0.f, 0.f, 1.f )
    , _borderWidth( 5 )
{
    setShaderName( "ui/widget-label" );
}

VideoCtrl::~VideoCtrl( void )
{
    stopVideo();
}

VideoCtrl* VideoCtrl::create( Container* container )
{
    VideoCtrl* video = new VideoCtrl( Engine::instance() );
    container->addChild( video );
    return video;
}

Renderable* VideoCtrl::createRenderable()
{
    return new VideoCtrlRenderable( this );
}

void VideoCtrl::startVideo( )
{
    if( !_videoStream )
    {
        _videoStream = VideoStreamManager::instance()->
                       getVideoStream();
        refresh();
    }
}

void VideoCtrl::stopVideo()
{
    if( _videoStream )
    {
        VideoStreamManager::instance()->delVideoStream( _videoStream );
        _videoStream = NULL;
    }
}

bool VideoCtrl::getShowBorder()
{
    return _showBorder;
}

void VideoCtrl::setShowBorder( bool show )
{
    _showBorder = show;
}

void VideoCtrl::setBorder( const omega::Color& borderColor, int width )
{
    _borderColor = borderColor;
    _borderWidth = width;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

VideoCtrlRenderable::VideoCtrlRenderable( VideoCtrl* video )
    : WidgetRenderable( video )
    , _video( video )
    , _yuvProgram( 0 )
    , _pboIndex( 0 )
    , _videoSrcW( 0 )
    , _videoSrcH( 0 )
    , _font( NULL )
    , _textureUniform( 0 )
{
    memset( _textureIds, 0, sizeof( _textureIds ) );
    memset( _pboIds, 0, sizeof( _pboIds ) );
}


VideoCtrlRenderable::~VideoCtrlRenderable( void )
{
}

void VideoCtrlRenderable::dispose()
{
    if( _textureIds[0] != 0 )
    {
        glDeleteTextures( 3, _textureIds );
        memset( _textureIds, 0, sizeof( _textureIds ) );
    }
    if( _pboIds[ 0 ][ 0 ] != 0 )
    {
        glDeleteBuffersARB( 6, ( GLuint* )_pboIds );
        memset( _pboIds, 0, sizeof( _pboIds ) );
    }
}

void VideoCtrlRenderable::refresh()
{
    WidgetRenderable::refresh();
    //refreshVideo();
    //refreshFont();

    if( _textureIds[0] != 0 )
    {
        glDeleteTextures( 3, _textureIds );
        memset( _textureIds, 0, sizeof( _textureIds ) );
    }
    if( _pboIds[ 0 ][ 0 ] != 0 )
    {
        glDeleteBuffersARB( 6, ( GLuint* )_pboIds );
        memset( _pboIds, 0, sizeof( _pboIds ) );
    }
}

void VideoCtrlRenderable::refreshVideo()
{
    VideoData* video_data = _video->_videoStream->getVideoData();
    if( !video_data )
    {
        return;
    }

    int w = video_data->w;
    int h = video_data->h;

    glGenTextures( 3, _textureIds );
    int ww[] = {w, w / 2, w / 2};
    int hh[] = {h, h / 2, h / 2};

    for( int i = 0; i < 3; ++i )
    {
        glBindTexture( GL_TEXTURE_RECTANGLE, _textureIds[i] );
        //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexImage2D( GL_TEXTURE_RECTANGLE, 0, GL_LUMINANCE, ww[ i ], hh[ i ], 0,
                      GL_LUMINANCE, GL_UNSIGNED_BYTE,  0 );
    }

    if( _video->_usePbo )
    {
        glGenBuffersARB( 6, ( GLuint* )_pboIds );
        for( int i = 0; i < 2; ++i )
        {
            for( int j = 0; j < 3; ++j )
            {
                glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[i][j] );
                glBufferDataARB( GL_PIXEL_UNPACK_BUFFER_ARB, ww[j] * hh[j], 0, GL_STREAM_DRAW_ARB );
            }
        }
        glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
    }

    //_yuv_program = getRenderer()->
    //	getOrCreateProgram("yuv_video", omicron::String("yuv_video.vert"), "yuv_video.frag");
    const char* YUV_SHADER_SRC = "uniform sampler2DRect Ytex;\r\n"
                                 "uniform sampler2DRect Utex,Vtex;\r\n"
                                 "void main(void)\r\n"
                                 "{\r\n"
                                 "float nx,ny,r,g,b,y,u,v;\r\n"
                                 "nx=gl_TexCoord[0].x;\r\n"
                                 "ny=gl_TexCoord[0].y;\r\n"
                                 "y=texture2DRect(Ytex,vec2(nx,ny)).r;\r\n"
                                 "u=texture2DRect(Utex,vec2(nx/2.0,ny/2.0)).r;\r\n"
                                 "v=texture2DRect(Vtex,vec2(nx/2.0,ny/2.0)).r;\r\n"
                                 "y=1.1643*(y-0.0625);\r\n"
                                 "u=u - 0.5;\r\n"
                                 "v=v - 0.5;\r\n"
                                 "r=y+1.5958*v;\r\n"
                                 "g=y-0.39173*u-0.81290*v;\r\n"
                                 "b=y+2.017*u;\r\n"
                                 "gl_FragColor=vec4(r,g,b,1.0);\r\n"
                                 "}\r\n";

    if( _yuvProgram == 0 )
    {
        _yuvProgram = getRenderer()->getOrCreateProgramFromSource( "yuv_video_frag", "", YUV_SHADER_SRC );
        if( oglError )
        {
            _yuvProgram = 0;
            return;
        }
    }
    if( _yuvProgram != 0 )
    {
        glUseProgram( _yuvProgram );
        int tex = 0;
        tex = glGetUniformLocation( _yuvProgram, "Ytex" );
        glUniform1i( tex, 0 );
        tex = glGetUniformLocation( _yuvProgram, "Utex" );
        glUniform1i( tex, 1 );
        tex = glGetUniformLocation( _yuvProgram, "Vtex" );
        glUniform1i( tex, 2 );
        glUseProgram( 0 );
        if( oglError )
        {
            return;
        }
    }
}

void VideoCtrlRenderable::refreshFont()
{
    if( _video->_subtitle.size() == 0 )
    {
        return;
    }

    if( _video->_fontFamily != "" )
    {
        _font = getRenderer()->getFont( _video->_fontFamily, DrawInterface::FTGLPolygon );
    }
    if( _font == NULL )
    {
        _font = getRenderer()->getDefaultFont();
    }
    _textureUniform = glGetUniformLocation( myShaderProgram, "unif_Texture" );
}

void VideoCtrlRenderable::drawContent( const DrawContext& context )
{
    omega::Vector2f tileOffs( context.tile->offset[ 0 ], context.tile->offset[ 1 ] );
    omega::Rectf vpTilePixel( tileOffs.cast< double >(),
                              ( tileOffs + context.tile->pixelSize.cast< double >() ).cast< double >() );

    omega::Rectf vpCtrl( omega::Vector2f( _video->getPosition().x() + 1, _video->getPosition().y() + 1 ),
                         omega::Vector2f( ( _video->getPosition().x() + _video->getSize().x() - 1 ),
                                          ( _video->getPosition().y() + _video->getSize().y() - 1 ) ) );
    if( !vpTilePixel.intersects( vpCtrl ) )
    {
        return;
    }

    // Calling the base WidgetRenderable draw function is not
    // required but is useful to draw widget background and borders.
    glUseProgram( 0 );
    WidgetRenderable::drawContent( context );
	
	//Sleep( 100 );
    renderVideo( context );
    glUseProgram( myShaderProgram );
    renderSubtitle( context );
}

void VideoCtrlRenderable::renderVideo( const DrawContext& context )
{
    VideoStreamPort* vs = _video->_videoStream;
    if( !vs )
    {
        return;
    }

    float display_w = _video->getWidth();
    float display_h = _video->getHeight();
    VideoData* video_data = vs->getVideoData();
    if( video_data )
    {
        _videoSrcW = video_data->w;
        _videoSrcH = video_data->h;

        char* data = video_data->data;
        if( data )
        {
            int x = 0, y = 0;
            if( _textureIds[0] == 0 )
            {
                refreshVideo();
            }

            if( _video->_usePbo )
            {
                _pboIndex = ( _pboIndex + 1 ) % 2;
                int nextPboIndex = ( _pboIndex + 1 ) % 2;

                glBindTexture( GL_TEXTURE_RECTANGLE_NV, _textureIds[0] );
                glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[_pboIndex][0] );
                glTexSubImage2D( GL_TEXTURE_RECTANGLE_NV, 0, x, y,
                                 _videoSrcW, _videoSrcH, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0 );

                glBindTexture( GL_TEXTURE_RECTANGLE_NV, _textureIds[1] );
                glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[_pboIndex][1] );
                glTexSubImage2D( GL_TEXTURE_RECTANGLE_NV, 0, x / 2, y / 2,
                                 _videoSrcW / 2, _videoSrcH / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0 );

                glBindTexture( GL_TEXTURE_RECTANGLE_NV, _textureIds[2] );
                glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[_pboIndex][2] );
                glTexSubImage2D( GL_TEXTURE_RECTANGLE_NV, 0, x / 2, y / 2,
                                 _videoSrcW / 2, _videoSrcH / 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0 );

                const char* dataPtr[] =
                {
                    data,
                    data + _videoSrcW * _videoSrcH,
                    data + _videoSrcW * _videoSrcH * 5 / 4
                };
                int dataSize[] =
                {
                    _videoSrcW * _videoSrcH,
                    _videoSrcW * _videoSrcH / 4,
                    _videoSrcW * _videoSrcH / 4
                };
                for( int i = 0; i < 3; ++i )
                {
                    // bind PBO to update pixel values
                    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, _pboIds[nextPboIndex][i] );

                    // map the buffer object into client's memory
                    // Note that glMapBufferARB() causes sync issue.
                    // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
                    // for GPU to finish its job. To avoid waiting (stall), you can call
                    // first glBufferDataARB() with NULL pointer before glMapBufferARB().
                    // If you do that, the previous data in PBO will be discarded and
                    // glMapBufferARB() returns a new allocated pointer immediately
                    // even if GPU is still working with the previous data.
                    glBufferDataARB( GL_PIXEL_UNPACK_BUFFER_ARB, dataSize[i], 0, GL_STREAM_DRAW_ARB );
                    GLubyte* ptr = ( GLubyte* )glMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
                    if( ptr )
                    {
                        // update data directly on the mapped buffer
                        memcpy( ptr, dataPtr[i], dataSize[i] );
                        glUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB ); // release pointer to mapping buffer
                    }
                }
                glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
            }
            else
            {
                glEnable( GL_TEXTURE_2D );
                glBindTexture( GL_TEXTURE_RECTANGLE, _textureIds[0] );
                glTexSubImage2D( GL_TEXTURE_RECTANGLE, 0, x, y, _videoSrcW, _videoSrcH,
                                 GL_LUMINANCE, GL_UNSIGNED_BYTE, data );

                glBindTexture( GL_TEXTURE_RECTANGLE, _textureIds[1] );
                glTexSubImage2D( GL_TEXTURE_RECTANGLE, 0, x / 2, y / 2, _videoSrcW / 2, _videoSrcH / 2,
                                 GL_LUMINANCE, GL_UNSIGNED_BYTE, data + _videoSrcW * _videoSrcH );

                glBindTexture( GL_TEXTURE_RECTANGLE, _textureIds[2] );
                glTexSubImage2D( GL_TEXTURE_RECTANGLE, 0, x / 2, y / 2, _videoSrcW / 2, _videoSrcH / 2,
                                 GL_LUMINANCE, GL_UNSIGNED_BYTE, data + _videoSrcW * _videoSrcH * 5 / 4 );
            }
        }
    }

    // video border
    if( _textureIds[0] == 0 )
    {
        if( _video->_showBorder )
        {
            omega::Color& color = _video->_borderColor;
            int lineWidth = _video->_borderWidth;
            glLineWidth( lineWidth );
            glColor3f( color.getRed(), color.getGreen(), color.getBlue() );
            glBegin( GL_LINE_LOOP );
            glVertex2i( lineWidth, lineWidth );
            glVertex2i( lineWidth, display_h - lineWidth );
            glVertex2i( display_w, display_h - lineWidth );
            glVertex2i( display_w, lineWidth );
            glVertex2i( lineWidth, lineWidth );
            glEnd();
        }
        return;
    }

    glUseProgram( _yuvProgram );

    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_RECTANGLE, _textureIds[ 0 ] );

    glActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_RECTANGLE, _textureIds[ 1 ] );

    glActiveTexture( GL_TEXTURE2 );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_RECTANGLE, _textureIds[ 2 ] );

    glBegin( GL_QUADS );
    glTexCoord2i( 0, 0 );
    glVertex2i( 0, 0 );
    glTexCoord2i( 0, _videoSrcH );
    glVertex2i( 0, display_h );
    glTexCoord2i( _videoSrcW, _videoSrcH );
    glVertex2i( display_w, display_h );
    glTexCoord2i( _videoSrcW, 0 );
    glVertex2i( display_w, 0 );
    glEnd();

    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
    glDisable( GL_TEXTURE_2D );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
    glDisable( GL_TEXTURE_2D );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_RECTANGLE, 0 );
    glDisable( GL_TEXTURE_2D );

    glUseProgram( 0 );
}

void VideoCtrlRenderable::renderSubtitle( const DrawContext& context )
{
    if( _video->_subtitle.size() == 0 )
    {
        return;
    }

    // If not font has been set, use default ui font.
    if( !_font )
    {
        refreshFont();
    }

    Vector2f text_pos( _video->_subtitleX, _video->_subtitleY );
    getRenderer()->drawWText( _video->_subtitle, _font, text_pos,
                              Font::HALeft | Font::VATop, _video->_fontColor );
}
