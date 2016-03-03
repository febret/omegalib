/**********************************************************************************************************************
 * THE OMEGA LIB PROJECT
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright 2010								Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:
 *  Alessandro Febretti							febret@gmail.com
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright (c) 2010, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------------------------------------------------
 *	ohello
 *		Implements the simplest possible omegalib application.  Just a synchronized renderer that performs some
 *		opengl drawing on one or multiple nodes.
 *********************************************************************************************************************/
#include <omega.h>
#include <omegaToolkit.h>
#include <omegaGl.h>

using namespace omega;
using namespace omegaToolkit;
using namespace omegaToolkit::ui;

#include "video_ctrl.h"
#include "video_stream_manager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class VideoApplication: public EngineModule
{
public:
    VideoApplication(): EngineModule( "VideoApplication" ) {}
    virtual void initialize()
    {
        EngineModule::initialize();

        omega::Console* console = getEngine()->getConsole();
        console->setDrawFlags( omega::Console::DrawStats );

        myUiModule = UiModule::createAndInitialize();
        Ref<Container> myUi = myUiModule->getUi();

        VideoCtrl* v1 = VideoCtrl::create( myUi );
        v1->startVideo();
        v1->setPosition( omega::Vector2f( 0, 0 ) );
        v1->setSize( omega::Vector2f( 3840, 2160 ) );

        VideoCtrl* v2 = VideoCtrl::create( myUi );
        v2->startVideo( );
        v2->setPosition( omega::Vector2f( 3840, 0 ) );
        v2->setSize( omega::Vector2f( 3840, 2160 ) );

        VideoCtrl* v3 = VideoCtrl::create( myUi );
        v3->startVideo(  );
        v3->setPosition( omega::Vector2f( 7680, 0 ) );
        v3->setSize( omega::Vector2f( 3840, 2160 ) );

        VideoCtrl* v4 = VideoCtrl::create( myUi );
        v4->startVideo( );
        v4->setPosition( omega::Vector2f( 0, 2160 ) );
        v4->setSize( omega::Vector2f( 3840, 2160 ) );

        VideoCtrl* v5 = VideoCtrl::create( myUi );
        v5->startVideo( );
        v5->setPosition( omega::Vector2f( 3840, 2160 ) );
        v5->setSize( omega::Vector2f( 3840, 2160 ) );

        VideoCtrl* v6 = VideoCtrl::create( myUi );
        v6->startVideo( );
        v6->setPosition( omega::Vector2f( 7680, 2160 ) );
        v6->setSize( omega::Vector2f( 3840, 2160 ) );
    }

    void update( const UpdateContext & context )
    {
        //update all video streams to next frame
        VideoStreamManager::instance()->updateAllStreams();
    }

private:
    // The ui manager
    Ref<UiModule> myUiModule;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ApplicationBase entry point
int main( int argc, char** argv )
{
    Application<VideoApplication> app( "testVideo" );
    return omain( app, argc, argv );
}
