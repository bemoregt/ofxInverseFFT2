#pragma once

#include "ofMain.h"
#include "ofxInverseFFT2.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void dragEvent(ofDragInfo dragInfo);
    
private:
    ofxInverseFFT2 fftProcessor;
    
    // GUI 요소
    ofxPanel gui;
    ofxToggle useLogScale;
    ofxToggle useShift;
    ofxToggle useNormalize;
    ofxButton resetButton;
    ofxButton saveResultButton;
    
    // 마스킹 관련 변수
    bool isDrawingMask;
    float maskX, maskY;
    float maskRadius;
    
    void saveResult();
    void resetMask();
};