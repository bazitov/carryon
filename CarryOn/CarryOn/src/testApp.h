#pragma once

#include "ofMain.h"
#include "FreeImage.h"  
#include "ofxOpenCv.h"
#include "ofxControlPanel.h"
#include "ofEvents.h"

// include these 2 lines for median-of-5 code:
#include <algorithm>
using std::swap;

class testApp : public ofBaseApp{
	
	public:
		
		void setup();
		void update();
		void draw();
  		void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);	
	
        //glich
        void glitchThis(unsigned char * pixels, int width, int height, ofImageType colorMode);

      		
		ofVideoGrabber 		vidGrabber;
        ofVideoPlayer 		shareMovie;
        bool                isGlitched;
        bool                drawGui;
        ofImage             videoColorImg;
        ofImage         *   glitchImg;
        ofImage         *   bufferImg;
        int                 glitchValue;
        ofImage             shareLogo;
        ofxCvColorImage		colorImg;
        ofxCvGrayscaleImage grayImage;
        ofxCvGrayscaleImage grayPast;
        ofxCvGrayscaleImage grayDiff;
        float               movementLevel;
        float               movementLevelMax;
        ofTrueTypeFont      myFont;
		int 				camWidth;
		int 				camHeight;
        int 				calcWidth;
        int 				calcHeight;
        int 				videoWidth;
        int 				videoHeight;
    
        // simple motion detection
        float                       alpha;
        float                       sum;
        vector<ofxCvGrayscaleImage> previousImages;

        bool                doDraw;
        bool                newFrame;
        int                 counter;

    
    //filters
    ofxControlPanel			gui;
	
    
	//--------------------------------------------	
	int                     gW;
	int                     gH;
	int                     gM;
};