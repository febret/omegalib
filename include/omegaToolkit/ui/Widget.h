/******************************************************************************
 * THE OMEGA LIB PROJECT
 *-----------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, 
 *							University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-----------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory,  
 * University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this 
 * list of conditions and the following disclaimer. Redistributions in binary 
 * form must reproduce the above copyright notice, this list of conditions and 
 * the following disclaimer in the documentation and/or other materials provided 
 * with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-----------------------------------------------------------------------------
 * What's in this file:
 *	Widget is the base class of the user interface utility toolkit.
 ******************************************************************************/
#ifndef __WIDGET_H__
#define __WIDGET_H__

#include "omega/osystem.h"
#include "omegaToolkit/omegaToolkitConfig.h"
#include "omega/DrawInterface.h"
#include "omega/Renderable.h"

// Needed to support python callbacks
// NOTE: don't move this header before others. omegalib will explode on exit
// on OSX with an Illegal Instruction: 4 error.
#include "omega/PythonInterpreterWrapper.h"

namespace omegaToolkit { 
    class UiScriptCommand;
    namespace ui {
    class Container;
    class WidgetFactory;
    ///////////////////////////////////////////////////////////////////////////
    class OTK_API Widget: public RenderableFactory, IEventListener
    {
    friend class UiManager;
    friend class UiRenderPass;
    friend class Container;
    friend class WidgetRenderable;
    public:
        enum Layer { Back, Middle, Front, NumLayers };
        enum BlendMode { BlendInherit, BlendNormal, BlendAdditive, BlendDisabled };
        static const int MaxWidgets = 16384;

        // Border style data
        struct BorderStyle
        {
            void fromString(const String& str);

            Color color;
            int width;
        };

        static Widget* create(Container* parent);

    public:
        Widget(Engine* server);
        virtual ~Widget();

        virtual Renderable* createRenderable();

        virtual void handleEvent(const Event& evt);
        virtual void update(const omega::UpdateContext& context);

        Container* getContainer() { return myContainer; }

        //! Input and Events
        //@{
        //! Sets the event listener used to handle events generated by this widget.
        void setUIEventHandler(IEventListener* value);
        //! @see setWidgetEventHandler
        IEventListener* getUIEventHandler();
        void setUIEventCommand(const String& command);
        //! Returns true if the event is a button down event happening
        //! on this widget
        bool isButtonDown(const Event& evt, Event::Flags button);
        //! Returns true if the event is a button up event happening
        //! on this widget
        bool isButtonUp(const Event& evt, Event::Flags button);
        //! Sets or gets a script command to be invoked on every update for this
        //! widget
        void setUpdateCommand(const String& cmd) { myUpdateCommand = cmd; }
        String getUpdateCommand() { return myUpdateCommand; }
        //! Sets or gets a script command to be invoked when this widget starts
        //! dragging
        void setDragBeginCommand(const String& cmd) { myDragBeginCommand = cmd; }
        String getDragBeginCommand() { return myDragBeginCommand; }
        //! Sets or gets a script command to be invoked when this widget ends
        //! dragging
        void setDragEndCommand(const String& cmd) { myDragEndCommand = cmd; }
        String getDragEndCommand() { return myDragEndCommand; }
        //! Sets or gets a script command to be invoked when this widget gets
        //! active status
        void setActivateCommand(const String& cmd) { myActivateCommand = cmd; }
        String getActivateCommand() { return myActivateCommand; }
        //! Sets or gets a script command to be invoked when this widget loses
        //! active status
        void setDeactivateCommand(const String& cmd) { myDeactivateCommand = cmd; }
        String getDeactivateCommand() { return myDeactivateCommand; }
        //@}

        //! Returns the widget name.
        const String& getName();
        //! Sets the widget name.
        void setName(const String& name);

        //! Position and rotation
        //@{
        //! Gets the widget position.
        const Vector2f& getPosition() { return myPosition; }
        //! Sets the widget position
        void setPosition(const omega::Vector2f& value);
        void setPosition(int value, int dimension);
        // Convenience method to center the widget around the specified point.
        void setCenter(const omega::Vector2f& value);
        Vector2f getCenter();
        //! Sets the widget rotation
        //! @param value - the widget rotation in degrees
        void setRotation(float value) { myRotation = value; }
        //! Gets the widget position.
        float getRotation() { return myRotation; }
        //@}

        //! Size constraints
        //@{
        //! Gets the widget size
        const Vector2f& getSize();
        //float getSize(Orientation orientation);
        float getWidth();
        float getHeight();
        //! Sets the widget size.
        void setSize(const Vector2f& value);
        void setWidth(float value);
        void setHeight(float value);
        const Vector2f& getMinimumSize();
        const Vector2f& getMaximumSize();
        void setMinimumSize(const Vector2f& value);
        int getMinimumWidth();
        int getMinimumHeight();
        void setMaximumSize(const Vector2f& value);
        int getMaximumWidth();
        int getMaximumHeight();
        void setMinimumWidth(float value);
        void setMinimumHeight(float value);
        void setMaximumWidth(float value);
        void setMaximumHeight(float value);
        void clearSizeConstaints();
        void setAutosize(bool value);
        bool getAutosize();
        //@}

        //! Flags
        //@{
        bool isVisible();
        void setVisible(bool value);
        //! When true, the widget is enabled, i.e. it can receive input events and takes part in navigation (can become active)
        bool isEnabled() { return myEnabled; }
        void setEnabled(bool value) { myEnabled = value; }
        bool isActive() { return myActive; }
        void setActive(bool value);
        //! Returns true if this widget is part of a container that will be drawn
        //! in 3D mode.
        virtual bool isIn3DContainer();
        bool isPointerInside() { return myPointerInside; }
        //@}

        //! Returns the unique Widget id.
        int getId();
        virtual void layout();

        void setStereo(bool value) { myStereo = value; }
        bool isStereo() { return myStereo; }

        //! Navigation
        //@{
        void setHorizontalNextWidget(Widget* value) { myHorizontalNextWidget = value; }
        Widget* getHorizontalNextWidget() { return myHorizontalNextWidget; }
        void setHorizontalPrevWidget(Widget* value) { myHorizontalPrevWidget = value; }
        Widget* getHorizontalPrevWidget() { return myHorizontalPrevWidget; }
        void setVerticalNextWidget(Widget* value) { myVerticalNextWidget = value; }
        Widget* getVerticalNextWidget() { return myVerticalNextWidget; }
        void setVerticalPrevWidget(Widget* value) { myVerticalPrevWidget = value; }
        Widget* getVerticalPrevWidget() { return myVerticalPrevWidget; }
        bool isNavigationEnabled() { return myNavigationEnabled; }
        void setNavigationEnabled(bool value);
        bool isGamepadInteractionEnabled();
        bool isPointerInteractionEnabled();
        //@}

        virtual void autosize() {}
        virtual void updateSize();
        void requestLayoutRefresh();

        //! Appearance
        //@{
        void setStyle(const String& style);
        String getStyleValue(const String& key, const String& defaultValue = "");
        void setStyleValue(const String& key, const String& value);
        void setScale(float value) { myScale = value; }
        void setActiveStyle(const String& value) { myActiveStyle = value;  }
        void setInactiveStyle(const String& value) { myInactiveStyle = value; setStyle(value); }
        String getActiveStyle() { return myActiveStyle; }
        String getInactiveStyle() { return myInactiveStyle; }
        //! Sets the widget scale. Scale controls the visual appearance of a 
        //! widget without changing its actual size or forcing a layout refresh 
        //! of the widget container. Scale is indicated as a proportion of the
        //! current widget size.
        float getScale() { return myScale; }
        void setAlpha(float value) { myAlpha = value; }
        float getAlpha();
        void setBlendMode(BlendMode value) { myBlendMode = value; }
        BlendMode getBlendMode() { return myBlendMode; }
        void setFillColor(const Color& c) { myFillColor = c; }
        void setFillEnabled(bool value) { myFillEnabled = value; }
        //! Gets the style for one of the borders
        //! @remarks valid values for side are:
        //! 0 - for top
        //! 1 - for right
        //! 2 - for bottom
        //! 3 - for left
        BorderStyle& getBorderStyle(int side) { return myBorders[side]; }
        //! Enables or disables shaders for this widget. Shaders are enabled
        //! by default and are required to correctly render some widget features
        //! like correct transparency. The shader used by the widget can be 
        //! replaced using the setShaderName method.
        void setShaderEnabled(bool value) { myShaderEnabled = value; }
        bool isShaderEnabled() { return myShaderEnabled; }
        //! Sets the name of the shader used by this widget. The widget will look
        //! for a vertex and a fragment shader with this name. By default, widgets
        //! use a shader named 'ui/widget-shader'. The default shaders will be
        //! 'ui/widget-shader.vert' and 'ui/widget-shader.frag'. The shader
        //! sources can be found in the omegalib data directory.
        void setShaderName(const String& name);
        const String& getShaderName() { return myShaderName; }
        //@}

        Layer getLayer() { return myLayer; }
        void setLayer(Layer layer) { myLayer = layer; }

        //! Returns true if the point is within this widget's bounding box.
        bool hitTest(const Vector2f& point);
        //! Transforms a 2D point from screen space to this widget's reference frame.
        Vector2f transformPoint(const omega::Vector2f& point);

        template<typename W> static W* getSource(const Event& evt);

        void setUserData(void* data) { myUserData = data; }
        void* getUserData() { return myUserData; }

        bool isDraggable() { return myDraggable; }
        void setDraggable(bool value) { myDraggable = value; }
        bool isDragging() { return myDragging; }
        //! When a widget is pinned, its position will remain fixed with 
        //! respect to its container. Pinning widgets is useful to make 
        //! container draggable only on a sub-section represented by the 
        //! pinned widget.
        bool isPinned() { return myPinned; }
        void setPinned(bool value) { myPinned = value; }

        void setSizeAnchorEnabled(bool enabled) { mySizeAnchorEnabled = enabled; }
        bool isSizeAnchorEnabled() { return mySizeAnchorEnabled; }
        void setSizeAnchor(const Vector2f& value) { mySizeAnchor = value; }
        const Vector2f getSizeAnchor() { return mySizeAnchor; }

        WidgetFactory* getFactory();

        //! Debug mode
        //@{
        //! Gets the color used when widget debug mode is enabled.
        Color getDebugColor() { return myDebugModeColor; }
        //! Sets the color used when widget debug mode is enabled.
        void setDebugColor( omega::Color value ) { myDebugModeColor = value; }
        //! Returns true if debug mode is enabled for this widget.
        bool isDebugModeEnabled() { return myDebugModeEnabled; }
        //! Enabled or disabled debug mode for this widget.
        //! When debug mode is enabled, the widget bounding box will be displayed.
        void setDebugModeEnabled(bool value) { myDebugModeEnabled = value; }
        //@}

        //! Scriptable draw callbacks
        //@{
        //! Sets a python function to be called before rendering of this widget
        //! begins. Pointer must be to a callable PyObject.
        void setPreDrawCallback(PyObject* predcb);
        //! Sets a python function to be called right before rendering of this widget
        //! terminates. Pointer must be to a callable PyObject.
        void setPostDrawCallback(PyObject* postdcb);
        //@}
    protected:
        bool simpleHitTest(const omega::Vector2f& point);
        static bool simpleHitTest(const omega::Vector2f& point, const omega::Vector2f& pos, const omega::Vector2f& size);

        //! Called when this widget becomes the active widget. 
        //! Derivec classes can implement this method to specify a custom 
        //! activation behavior
        virtual void activate() {}
        //! Called when this widget is not the active widget anymore. 
        //! Derivec classes can implement this method to specify a custom 
        //! deactivation behavior
        virtual void deactivate() {}
        virtual void updateStyle();

        //! internal layout management
        //@{
        bool needLayoutRefresh();
        void setActualSize(int value, Orientation orientation, bool force = false);
        //@}

        void setContainer(Container* value);
        void dispatchUIEvent(const Event& evt);

        // Menu Widget Sounds
        void playMenuScrollSound();

    protected:
        omega::Vector2f myPosition;
        omega::Vector2f mySize;
        float myRotation;
        //float myScale;

    private:
        Layer myLayer;

        bool myStereo;
        bool myInitialized;

        Engine* myServer;

        static NameGenerator mysNameGenerator;

        unsigned int myId;
        // Widget links for button-based navigation
        Widget* myHorizontalNextWidget;
        Widget* myHorizontalPrevWidget;
        Widget* myVerticalPrevWidget;
        Widget* myVerticalNextWidget;

        // User data.
        void* myUserData;

        omega::String myName;

        // We use a raw pointer to our parent in order to avoid reference loops.
        Container* myContainer;

        IEventListener* myEventHandler;
        Ref<UiScriptCommand> myUiEventCommand;

        bool myNeedLayoutRefresh;

        // Debug mode.
        bool myDebugModeEnabled;
        omega::Color myDebugModeColor;

        bool myPinned;

        bool mySizeAnchorEnabled;
        omega::Vector2f mySizeAnchor;

        bool myDraggable;
        bool myDragging;
        omega::Vector2f myUserMovePosition;

        // When true, the widget is visible.
        bool myVisible;
        // When true, the widget is enabled, i.e. it can receive input events.
        bool myEnabled;
        // When true, the widget has active focus, i.e. it has processing priority over input events.
        bool myActive;
        // When true, the widget takes part in navigation
        bool myNavigationEnabled;
        // When true, a pointer is over this widget area.
        bool myPointerInside;

        // Size constraints.
        omega::Vector2f myMinimumSize;
        omega::Vector2f myMaximumSize;
        bool myAutosize;

        // Blend mode
        BlendMode myBlendMode;
        float myAlpha;
        float myScale;

        // Style data
        Dictionary<String, String> myStyleDictionary;
        String myActiveStyle;
        String myInactiveStyle;

        // Fill style data
        bool myFillEnabled;
        Color myFillColor;

        // Shader data
        bool myShaderEnabled;
        String myShaderName;

        String myUpdateCommand;
        String myDragBeginCommand;
        String myDragEndCommand;
        String myActivateCommand;
        String myDeactivateCommand;

        BorderStyle myBorders[4];

        // Draw pyhon callbacks
        PyObject* myPreDrawCallback;
        PyObject* myPostDrawCallback;

        static Dictionary<int, ui::Widget*> mysWidgets;
        static Lock mysWidgetsMutex;  //mutex for Dictionary above
    };

    ///////////////////////////////////////////////////////////////////////////
    class OTK_API WidgetRenderable: public Renderable
    {
    public:
        WidgetRenderable(Widget* owner): 
          myOwner(owner), 
              myShaderProgram(0), myCurrentContext(NULL) {}

        virtual void draw(const DrawContext& context);
        virtual void drawContent(const DrawContext& context);
        virtual void refresh();
    protected:
        virtual void preDraw();
        virtual void postDraw();
        void pushDrawAttributes();
        void popDrawAttributes();

        //! Gets the current renderstate (accessible inside drawContent)
        //RenderState* getRenderState() { return myRenderState; }

        GLuint myShaderProgram;
        GLuint myAlphaUniform;
        const DrawContext* myCurrentContext;

    private:
        Widget* myOwner;
        //RenderState* myRenderState;
    };

    ///////////////////////////////////////////////////////////////////////////
    inline const String& Widget::getName() 
    { return myName; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setName(const String& name)
    { myName = name; }

    ///////////////////////////////////////////////////////////////////////////
    inline IEventListener* Widget::getUIEventHandler() 
    { return myEventHandler; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setUIEventHandler(IEventListener* value)
    { myEventHandler = value; }

    ///////////////////////////////////////////////////////////////////////////
    inline bool Widget::isButtonDown(const Event& evt, Event::Flags button)
    {
        return evt.isFrom(Service::Ui, myId) && evt.isButtonDown(button);
    }

    ///////////////////////////////////////////////////////////////////////////
    inline bool Widget::isButtonUp(const Event& evt, Event::Flags button)
    {
        return evt.isFrom(Service::Ui, myId) && evt.isButtonUp(button);
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setAutosize(bool value)
    { 
        myAutosize = value; 
        requestLayoutRefresh();
    }

    ///////////////////////////////////////////////////////////////////////////
    inline bool Widget::getAutosize()
    { return myAutosize; }

    ///////////////////////////////////////////////////////////////////////////
    inline const Vector2f& Widget::getSize() 
    { return mySize; }

    ///////////////////////////////////////////////////////////////////////////
    //inline float Widget::getSize(Orientation orientation)
    //{
    //    return mySize[orientation];
    //}

    ///////////////////////////////////////////////////////////////////////////
    inline float Widget::getWidth()
    { return mySize[0]; }

    ///////////////////////////////////////////////////////////////////////////
    inline float Widget::getHeight() 
    { return mySize[1]; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setSize(const omega::Vector2f& value) 
    { 
        if(value != mySize)
        {
            requestLayoutRefresh(); 
            mySize = value; 
            myMinimumSize = value; 
            myMaximumSize = value; 
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setWidth(float value) 
    { 
        requestLayoutRefresh(); 
        mySize[0] = value; 
        myMinimumSize[0] = value; 
        myMaximumSize[0] = value; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setHeight(float value) 
    { 
        requestLayoutRefresh(); 
        mySize[1] = value; 
        myMinimumSize[1] = value; 
        myMaximumSize[1] = value; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline const Vector2f& Widget::getMinimumSize() 
    { return myMinimumSize; }

    ///////////////////////////////////////////////////////////////////////////
    inline const Vector2f& Widget::getMaximumSize() 
    { return myMaximumSize; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setMinimumSize(const Vector2f& value) 
    {
        requestLayoutRefresh(); 
        myMinimumSize = value; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline int Widget::getMinimumWidth() 
    { return myMinimumSize[0]; }

    ///////////////////////////////////////////////////////////////////////////
    inline int Widget::getMinimumHeight() 
    { return myMinimumSize[1]; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setMaximumSize(const Vector2f& value) 
    {
        requestLayoutRefresh(); 
        myMaximumSize = value; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline int Widget::getMaximumWidth() 
    { return myMaximumSize[0]; }

    ///////////////////////////////////////////////////////////////////////////
    inline int Widget::getMaximumHeight() 
    { return myMaximumSize[1]; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setMinimumWidth(float value) 
    { 
        requestLayoutRefresh(); 
        myMinimumSize[0] = value; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setMinimumHeight(float value) 
    {
        requestLayoutRefresh(); 
        myMinimumSize[1] = value; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setMaximumWidth(float value) 
    { 
        requestLayoutRefresh(); 
        myMaximumSize[0] = value; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setMaximumHeight(float value) 
    { 
        requestLayoutRefresh(); 
        myMaximumSize[1] = value; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline bool Widget::isVisible() 
    { return myVisible; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setVisible(bool value) 
    { myVisible = value; }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setPosition(const omega::Vector2f& value) 
    { 
        if(myPinned)
        {
            if(myContainer != NULL)
            {
                Vector2f pos = ((Widget*)myContainer)->getPosition() +
                    value - myPosition;

                // se the alternative setPosition method because we do not
                // want the layout to be refreshed upon this call.
                ((Widget*)myContainer)->setPosition(pos[0], Horizontal);
                ((Widget*)myContainer)->setPosition(pos[1], Vertical);
            }
        }
        else 
        {
            myPosition = value; 
            requestLayoutRefresh();
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setPosition(int value, int dimension) 
    { 
        if(myPinned)
        {
            if(myContainer != NULL)
            {
                if(myContainer != NULL)
                {
                    ((Widget*)myContainer)->setPosition(
                        ((Widget*)myContainer)->getPosition()[dimension] + 
                        value - myPosition[dimension], dimension);
                }
            }
        }
        else
        {
            myPosition[dimension] = value; 
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setShaderName(const String& name)
    {
        myShaderName = name;
        // Refresh the widget, so its renderables will load the new shader.
        refresh();
    }

    ///////////////////////////////////////////////////////////////////////////
    // NOTE: Widget::getSource is only used by UiScriptCommand. See if there is a way to get rid of this.
    template<typename W> 
    inline W* Widget::getSource(const Event& evt)
    {
        if(evt.getServiceType() == Service::Ui)
        {
            W* w = dynamic_cast<W*>(mysWidgets[evt.getSourceId()]);
            return w;
        }
        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setPreDrawCallback(PyObject* predcb)
    {
        myPreDrawCallback = predcb; 
    }

    ///////////////////////////////////////////////////////////////////////////
    inline void Widget::setPostDrawCallback(PyObject* postdcb)
    {
        myPostDrawCallback = postdcb;
    }
};
}; // namespace omegaToolkit

#endif