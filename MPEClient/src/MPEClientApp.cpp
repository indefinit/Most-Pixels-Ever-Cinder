//
//  MPEClientApp.cpp
//  Unknown Project
//
//  Copyright (c) 2013 William Lindmeier. All rights reserved.
//

#include "Ball.hpp"
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "ClientSettings.h"
#include "MPEClient.h"

using namespace ci;
using namespace ci::app;
//using namespace std;
using std::string;
using namespace mpe;

class MPEClientApp : public AppNative
{

    public:

    void        prepareSettings( Settings *settings );
    void        setup();
    void        shutdown();
    void        mouseDown( MouseEvent event );
    void        mouseDrag( MouseEvent event );
    void        sendMousePosition();
    void        update();
    void        draw();
    void        drawViewport(bool dra);
    void        clientUpdate();
    void        clientDraw();

    private:

    MPEClient   mClient;
    Rand        mRand;
    Ball        mBall;
    /*
    // For demonstration purposes:
    // Drag the window around the screen to change the
    // position of the client.
    Vec2i       mScreenSize;
    Vec2i       mScreenPos;
    */
};

void MPEClientApp::prepareSettings( Settings *settings )
{
    // NOTE: Initially making the window small to prove that
    // the settings.xml forces a resize.
    settings->setWindowSize( 100, 100 );
}

void MPEClientApp::setup()
{
    console() << "Loading settings from " << SettingsFileName << "\n";

    mClient = MPEClient(SettingsFileName);

    // The same as the processing sketch.
    // Does Processing Rand work the same as Cinder Rand as OF Rand?
    mRand.seed(1);

    Vec2i sizeMaster = mClient.getMasterSize();
    Vec2f posBall = Vec2f(mRand.nextFloat(sizeMaster.x), mRand.nextFloat(sizeMaster.y));
    Vec2f velBall = Vec2f(mRand.nextFloat(-5,5), mRand.nextFloat(-5,5));

    console() << "Creating ball with master size: " << sizeMaster << "\n";
    mBall = Ball(posBall, velBall, sizeMaster);

    mClient.start();
}

void MPEClientApp::shutdown()
{
    mClient.stop();
}

void MPEClientApp::mouseDown( MouseEvent event )
{
    sendMousePosition();
}

void MPEClientApp::mouseDrag( MouseEvent event )
{
    sendMousePosition();
}

void MPEClientApp::sendMousePosition()
{
    if (mClient.isConnected())
    {
        Vec2i pos = getMousePos();
        mClient.broadcast(std::to_string(pos.x) + "," + std::to_string(pos.y));
    }
}

#pragma mark - Loop

void MPEClientApp::update()
{
    int frameCount = getElapsedFrames();

    if (mClient.isConnected())
    {
        // It will just stall until it's ready to draw
        bool isNewDataAvailable = mClient.update();

        if (isNewDataAvailable)
        {
            mBall.calc();            
        }

        /*
        Vec2i size = getWindowSize();
        Vec2i pos = getWindowPos();

        if (mScreenSize != size || mScreenPos != pos)
        {
            // The position has changed.
            // Update the renderable area.
            mClient.setVisibleRect(ci::Rectf(pos.x, pos.y, pos.x + size.x, pos.y + size.y));
            console() << "Visible Rect: " << mClient.getVisibleRect() << "\n";
            mScreenSize = size;
            mScreenPos = pos;
        }
        */

    }
    else
    {
        // Attempt to reconnect every 60 frames
        if (frameCount % 60 == 0)
        {
            mClient.start();
        }
    }
}

void MPEClientApp::draw()
{
    // App drawing should be done in frameEvent.
    mClient.draw(boost::bind(&MPEClientApp::drawViewport, this, _1));
}

void MPEClientApp::drawViewport(bool isNewFrame)
{
    if (isNewFrame)
    {
        gl::clear(Color( 1, 0, 0 ));
    }
    else
    {
        gl::clear(Color( 1, 0, 1 ));
    }
    
    gl::color(0,0,0);
    gl::drawString(std::to_string(getElapsedFrames()), Vec2f(100, 100));
    mBall.draw();
}

CINDER_APP_NATIVE( MPEClientApp, RendererGl )
