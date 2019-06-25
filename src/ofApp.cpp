#include "ofApp.h"

const int width = 1024;
const int height = 768;

//--------------------------------------------------------------
void ofApp::setup()
{
    // Rectangular textures used to be the only way to support
    // non power of 2 sizes in textures
    ofDisableArbTex();

    ofxFBXSceneSettings settings;

    string filename = "humanoid.fbx";

    if (scene.load(filename, settings))
    {
        cout << "ofApp :: loaded the scene OK" << endl;
    }
    else
    {
        cout << "ofApp :: Error loading the scene" << endl;
    }

    cam.lookAt(ofVec3f(0, 0, 0));
    cam.setDistance(550);
    cam.setFarClip(6000);
    cam.setNearClip(.5f);

    fbxMan.setup(&scene);
    fbxMan.setAnimation(0);

    bRenderNormals = false;
    bRenderMeshes = true;
    bDrawBones = false;

    // Syphon setup
    counter = 0;
    ofSetCircleResolution(50);

    bSmooth = false;
    ofSetWindowTitle("OFX: FBX to Syphon");

    mainOutputSyphonServer.setName("Screen Output");
    individualTextureSyphonServer.setName("Texture Output");

    mClient.setup();

    // Using Syphon app Simple Server, found at http://syphon.v002.info/
    mClient.set("", "Simple Server");

    tex.allocate(200, 100, GL_RGBA);

    // If vertical sync is off, we can go a bit fast...
    // This caps the framerate at 60fps.
    ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update()
{

    light.setPosition(0, 200, 200);

    //    light.setPosition(
    //                      cos(ofGetElapsedTimef()) * 20.f,
    //                      sin(ofGetElapsedTimef()*4) * 50 + 10,
    //                      sin(ofGetElapsedTimef()) * 20.f + 100
    //                      );

    fbxMan.update();

    // Perform any bone manipulation here //

    fbxMan.lateUpdate();

    // Syphon update
    counter = counter + 0.033f;
}

//--------------------------------------------------------------
void ofApp::draw()
{

    ofBackground(0, 177, 64);

    ofSetColor(255, 255, 255);

    glEnable(GL_DEPTH_TEST);

    cam.begin();

    ofEnableLighting();
    light.enable();

    if (bRenderMeshes)
    {
        ofSetColor(255, 255, 255);
        fbxMan.draw();
    }

    light.disable();
    ofDisableLighting();

    if (bDrawBones)
    {
        fbxMan.drawSkeletons(6.);
    }

    if (bRenderNormals)
    {
        ofSetColor(255, 0, 255);
        fbxMan.drawMeshNormals(1, false);
    }

    ofSetColor(light.getDiffuseColor());
    // light.draw(); // Draws the position widget of the light

    cam.end();

    glDisable(GL_DEPTH_TEST);

    int numBones = 0;
    vector<shared_ptr<ofxFBXSkeleton>> &skeletons = fbxMan.getSkeletons();
    for (int i = 0; i < skeletons.size(); i++)
    {
        numBones += skeletons[i]->getNumBones();
    }

    // Syphon Stuff

    mClient.draw(50, 50);

    mainOutputSyphonServer.publishScreen();

    individualTextureSyphonServer.publishTexture(&tex);

    ofDrawBitmapString("Note this text is not captured by Syphon since it is drawn after publishing.\nYou can use this to hide your GUI for example.", 15, 15);
}

#ifdef TARGET_OPENGLES
//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs &touch)
{
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs &touch)
{
}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs &touch)
{
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs &touch)
{
    if (scene.getNumAnimations() > 1)
    {
        int newAnimIndex = fbxMan.getCurrentAnimationIndex() + 1;
        if (newAnimIndex > scene.getNumAnimations() - 1)
        {
            newAnimIndex = 0;
        }
        fbxMan.setAnimation(newAnimIndex);
    }
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs &touch)
{
}
#else

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (scene.getNumAnimations() > 1)
    {
        if (key == OF_KEY_DOWN)
        {
            int newAnimIndex = fbxMan.getCurrentAnimationIndex() + 1;
            if (newAnimIndex > scene.getNumAnimations() - 1)
            {
                newAnimIndex = 0;
            }
            fbxMan.setAnimation(newAnimIndex);
        }
        if (key == OF_KEY_UP)
        {
            int newAnimIndex = fbxMan.getCurrentAnimationIndex() - 1;
            if (newAnimIndex < 0)
            {
                newAnimIndex = scene.getNumAnimations() - 1;
            }
            fbxMan.setAnimation(newAnimIndex);
        }
    }

    // Syphon Keypressed
    if (key == 's')
    {
        bSmooth = !bSmooth;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    if (key == ' ')
    {
        fbxMan.getCurrentAnimation().togglePlayPause();
    }
    if (key == 'n')
    {
        bRenderNormals = !bRenderNormals;
    }
    if (key == 'm')
    {
        bRenderMeshes = !bRenderMeshes;
    }
    if (key == 'b')
    {
        bDrawBones = !bDrawBones;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}
#endif
