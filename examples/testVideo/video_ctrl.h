#ifndef __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_CTRL_HEADER__
#define __SUYUN_DISPLAY_SYSTEM_RENDER_VIDEO_CTRL_HEADER__

#include <omega/Engine.h>
#include <omegaToolkit/ui/Widget.h>
#include <omegaToolkit/ui/Container.h>
#include <omegaGl.h>

class VideoStreamPort;
class VideoCtrl : public omegaToolkit::ui::Widget
{
    friend class VideoCtrlRenderable;
public:
    VideoCtrl( omega::Engine* e );
    ~VideoCtrl( void );

    static VideoCtrl* create( omegaToolkit::ui::Container* container );

    virtual omega::Renderable* createRenderable();

    void startVideo( const std::string& realIp, const std::string& ip,
                     int port, int qosPort, int mediaType, int encrypt );
    void stopVideo();


    bool getShowBorder();
    void setShowBorder( bool show );

    void setBorder( const omega::Color& borderColor, int width );

private:
    VideoStreamPort*    _videoStream;

    std::wstring	_subtitle;
    int				_subtitleX;
    int				_subtitleY;
    omega::Color	_fontColor;
    int				_fontSize;
    std::string		_fontFamily;

    bool			_showBorder;
    omega::Color	_borderColor;
    int				_borderWidth;

    bool _usePbo;
};

class VideoCtrlRenderable
    : public omegaToolkit::ui::WidgetRenderable
{
public:
    VideoCtrlRenderable( VideoCtrl* video );
    ~VideoCtrlRenderable( void );

    virtual void dispose();
    virtual void refresh();
    virtual void drawContent( const omega::DrawContext& context );

private:
    void refreshVideo();
    void refreshFont();
    void renderVideo( const omega::DrawContext& context );
    void renderSubtitle( const omega::DrawContext& context );

private:
    VideoCtrl* _video;

    GLuint _yuvProgram;
    GLuint _textureIds[3];
    GLuint _pboIds[2][3];
    int _pboIndex;

    int _videoSrcW;
    int _videoSrcH;

    omega::Ref< omega::Font > _font;
    GLuint _textureUniform;
};

#endif
