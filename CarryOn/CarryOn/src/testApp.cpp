#include "testApp.h"

using namespace cv;

//--------------------------------------------------------------
void testApp::setup(){
    ofSetDataPathRoot("../Resources/");
    //loadvideo
    shareMovie.loadMovie("ShareConference.m4v");
	shareMovie.play();
    
    newFrame = false;
    counter = 0;
    movementLevelMax = 0;
    isGlitched = false;
    drawGui = false;

	camWidth 		= 320;	// try to grab at this size. 
	camHeight 		= 240;
    ofSetVerticalSync(true);
	ofEnableAlphaBlending();
    calcWidth       = camWidth;
    calcHeight      = camHeight;
    		
    // setup video grabber & saver
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(calcWidth,camHeight);

    shareLogo.loadImage("share3.png");
    
    bufferImg = new ofImage();
	glitchImg = new ofImage();
    glitchValue = 0;
    videoWidth 	= 960;
	videoHeight = 540;
    
    //draw setup
    myFont.loadFont("Cousine-Regular-Latin.ttf", 16);
    
    //setup OpenCV & Optical flow
    colorImg.allocate(calcWidth,calcHeight); //<-- CAM WEIGHT!
	grayImage.allocate(calcWidth,calcHeight);
	grayPast.allocate(calcWidth,calcHeight);
	grayDiff.allocate(calcWidth,calcHeight);
    previousImages.push_back(grayPast);
    previousImages.push_back(grayPast);
    previousImages.push_back(grayPast);
    alpha = 0.5;
    
 
    //GUI
    gW = 256;
	gH = (gW*3)/4;
	gM = 8; 
    
    gui.setup("App Controls", 640, 15, 320, 360);
	gui.addPanel(" Main Controls", 1, false);
	
	//--------- PANEL 1
	gui.setWhichPanel(0);
	gui.setWhichColumn(0);
    
    gui.addSlider("Initial Glitch",   "GLITCH_INITIAL", 10, 0, 15, false);
    gui.addSlider("Glitch coefficient",   "GLITCH_COEFFICIENT", 1, 0, 10, false);
    gui.addSlider("Movement level",    "RECORD_LEVEL_SIMPLE", 0.3, 0, 10.0, false);
	
	gui.addToggle("Draw Debug?", "DO_DRAW", 1);
	
	gui.loadSettings("controlPanelSettings.xml");
}

//--------------------
void testApp::glitchThis(unsigned char * pixels, int width, int height, ofImageType colorMode){
    
	// if no pixels - skip
	if(!pixels) return;
    
    
	// thanks to the original example by Kyle McDonald
	// implementation is quite similar, no surprises in there
    
    
	// we clear memory first 
    
	bufferImg->clear();
    
	// set pixels and be sure we're converting it to RGB, we'll need this further for image recording
	// also alpha is needed for desktop mode 
    
	bufferImg->setFromPixels(pixels, width, height, colorMode, true);
	bufferImg->setImageType(OF_IMAGE_COLOR);
    
	// open buffer and save image to memory
    
	ofBuffer  * buffer = new ofBuffer(); 
	ofSaveImage(bufferImg->getPixelsRef(), *buffer, OF_IMAGE_FORMAT_JPEG, OF_IMAGE_QUALITY_BEST);
    
	// send this data to glitch it into memory!
    // send this data to glitch it!
    char * bffr = buffer->getBinaryBuffer();
    
    
    int scrambleStart = 617;
    int scrambleEnd = buffer->size();
    
    int nbOfReplacements =  glitchValue;                                  
    if (nbOfReplacements < 0) nbOfReplacements = 0;
    
    for(int i = 0; i < nbOfReplacements; i++) {
        
        int PosA = (int) (ofRandom (scrambleStart, scrambleEnd - scrambleStart));
        int PosB = (int) (ofRandom (scrambleStart, scrambleEnd - scrambleStart));
        
        char tmp	 =  bffr[PosA];
        bffr[PosA] =	bffr[PosB];
        bffr[PosB] =	tmp;
        
    }
    
	buffer->resetLineReader();
    
	// and then we try to load this image again
	// glitch can be randomly corrupted, so we have to be careful on this 
    
	//bHasLoaded = true;
	if(!glitchImg->loadImage(*buffer)) {
		//bHasLoaded = false;
        
	}
    
	// force to RGB again
	glitchImg->setImageType(OF_IMAGE_COLOR);
    
	// clear mem
	buffer->clear();
	delete buffer;
}
//--------------------------------------------------------------
void testApp::update(){
    
    //movie
    shareMovie.idleMovie();
    
    gui.update();
    vidGrabber.grabFrame();
    if (vidGrabber.isFrameNew()) {
        newFrame = true;
        colorImg.setFromPixels(vidGrabber.getPixels(), calcWidth, calcHeight);	
        grayImage = colorImg;
    }
    
    if (newFrame){
        float treshod;
            grayDiff = grayImage;
            grayDiff.absDiff(previousImages[0]);
            Mat diffMat(grayDiff.getCvImage());
            Scalar s1 = mean(diffMat);
            
            // store the current grayscale image for the next iteration of update()
            previousImages.push_back(grayImage);
            if (previousImages.size() > 10) {
                previousImages.erase(previousImages.begin());
            }
            // let's threshold the difference image,
            // all values less than 10 are 0, all values above 10 become 255
            //gray_diff.threshold(10);
            
            // here we will find the sum and then average of all the pixels in the difference image
            // this will be used for a simple measure of "motion"
            // we use a low-pass filter, a first order filter which combines the current
            // value with the previous one, using a linear weighting.
            sum = (alpha) * sum + (1 - alpha) * s1[0] / 10.0f; //cvSum(gray_diff.getCvImage()).val[0] / (float)img_width / (float)img_height / 10.0;
            movementLevel = sum;
            treshod = gui.getValueF("RECORD_LEVEL_SIMPLE");
       
        if (movementLevelMax < movementLevel) movementLevelMax = movementLevel; 
        if (movementLevel < treshod) {
            isGlitched = true;
            
            //do the glitch
            
            glitchValue =  (int)(gui.getValueF("GLITCH_INITIAL")-movementLevel*gui.getValueF("GLITCH_COEFFICIENT"));
            
            unsigned char *		pixels = shareMovie.getPixels();
            glitchThis(pixels, videoWidth, videoHeight, OF_IMAGE_COLOR);

        } else {
            isGlitched = false;
        }
        newFrame = false;
    }
}

//--------------------------------------------------------------
void testApp::draw(){ 
    char reportStr[1024];
    
    if (isGlitched) {
        glitchImg->draw(0,0);
        sprintf(reportStr, "Dance in front of the camera!");
        myFont.drawString (reportStr, 555, 50);
    } else {
        shareMovie.draw(0, 0, videoWidth, videoHeight);
    }
    
    if (gui.getValueI("DO_DRAW")) {
    
        ofSetHexColor(0xffffff);
        colorImg.draw (800, 420, 160, 120);
        
//        sprintf(reportStr, "vec length:%f, max length:%f", movementLevel, movementLevelMax);
//        ofDrawBitmapString(reportStr, 20, 10);
    }
    if (drawGui) {
        gui.draw();
    }
    shareLogo.draw(15, 15);
    sprintf(reportStr, "We are SHARING %.0f seconds with you!", shareMovie.getPosition()*100);
    ofDrawBitmapString(reportStr, 20, 530);
    
}

//--------------------------------------------------------------
void testApp::exit(){ 
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
    if ( key == 'r') {
        movementLevelMax = 0;
    }
    if ( key == 'g') {
        drawGui = !drawGui;
    }
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    gui.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    gui.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	gui.mouseReleased();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
