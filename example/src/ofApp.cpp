#include "ofApp.h"

void ofApp::setup() {
    // FFT 프로세서 초기화
    fftProcessor.setup();
    
    // GUI 설정
    gui.setup("FFT Settings");
    gui.add(useLogScale.setup("Log Scale", true));
    gui.add(useShift.setup("FFT Shift", false));
    gui.add(useNormalize.setup("Normalize", true));
    gui.add(resetButton.setup("Reset Mask"));
    gui.add(saveResultButton.setup("Save Results"));
    
    // 마스킹 변수 초기화
    isDrawingMask = false;
    maskRadius = 10;
    
    // 기본 이미지 로드 (샘플)
    fftProcessor.loadImage("images/sample.jpg");
    
    // 이벤트 리스너 설정
    resetButton.addListener(this, &ofApp::resetMask);
    saveResultButton.addListener(this, &ofApp::saveResult);
    
    // 윈도우 제목 설정
    ofSetWindowTitle("ofxInverseFFT2 Example");
}

void ofApp::update() {
    // GUI 값 변경 확인 및 설정
    static bool prevLogScale = useLogScale;
    static bool prevShift = useShift;
    static bool prevNormalize = useNormalize;
    
    if(prevLogScale != useLogScale) {
        fftProcessor.setLogScale(useLogScale);
        prevLogScale = useLogScale;
    }
    
    if(prevShift != useShift) {
        fftProcessor.setSpectrumShift(useShift);
        prevShift = useShift;
    }
    
    if(prevNormalize != useNormalize) {
        fftProcessor.setNormalize(useNormalize);
        prevNormalize = useNormalize;
    }
    
    // FFT 프로세서 업데이트
    fftProcessor.update();
}

void ofApp::draw() {
    ofBackground(40);
    
    // 이미지 그리기
    fftProcessor.drawAll(0, 0, ofGetWidth(), ofGetHeight() - 100);
    
    // 설명 텍스트 그리기
    ofSetColor(255);
    ofDrawBitmapString("Original", 50, ofGetHeight() - 70);
    ofDrawBitmapString("Spectrum (click to apply notch filter)", ofGetWidth()/3 + 50, ofGetHeight() - 70);
    ofDrawBitmapString("Inverse FFT", 2*ofGetWidth()/3 + 50, ofGetHeight() - 70);
    ofDrawBitmapString("Drag & drop images to load", 50, ofGetHeight() - 50);
    
    // GUI 그리기
    gui.draw();
}

void ofApp::keyPressed(int key) {
    if(key == 's') {
        saveResult();
    }
    else if(key == 'r') {
        resetMask();
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    // 스펙트럼 영역 내에서만 마스크 적용
    float spectrumX = ofGetWidth() / 3;
    float spectrumWidth = ofGetWidth() / 3;
    
    if(x >= spectrumX && x < spectrumX + spectrumWidth && y < ofGetHeight() - 100) {
        isDrawingMask = true;
        maskX = x - spectrumX;
        maskY = y;
        
        // 마스크 위치를 스펙트럼 이미지 좌표로 변환
        maskX = (maskX / spectrumWidth) * 256;
        maskY = (maskY / (ofGetHeight() - 100)) * 256;
        
        // 노치 필터 적용
        fftProcessor.applyNotchFilter(maskX, maskY, maskRadius);
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    if(isDrawingMask) {
        float spectrumX = ofGetWidth() / 3;
        float spectrumWidth = ofGetWidth() / 3;
        
        if(x >= spectrumX && x < spectrumX + spectrumWidth && y < ofGetHeight() - 100) {
            maskX = x - spectrumX;
            maskY = y;
            
            // 마스크 위치를 스펙트럼 이미지 좌표로 변환
            maskX = (maskX / spectrumWidth) * 256;
            maskY = (maskY / (ofGetHeight() - 100)) * 256;
            
            // 노치 필터 적용
            fftProcessor.applyNotchFilter(maskX, maskY, maskRadius);
        }
    }
}

void ofApp::mouseReleased(int x, int y, int button) {
    isDrawingMask = false;
}

void ofApp::dragEvent(ofDragInfo dragInfo) { 
    if(dragInfo.files.size() > 0) {
        fftProcessor.loadImage(dragInfo.files[0]);
    }
}

void ofApp::saveResult() {
    ofImage& spectrum = fftProcessor.getSpectrumImage();
    ofImage& inverse = fftProcessor.getInverseImage();
    
    if(spectrum.isAllocated() && inverse.isAllocated()) {
        spectrum.save("spectrum_result.png");
        inverse.save("inverse_result.png");
        ofLog() << "Results saved as spectrum_result.png and inverse_result.png";
    }
}

void ofApp::resetMask() {
    fftProcessor.resetMask();
}