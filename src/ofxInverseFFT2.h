#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"

class ofxInverseFFT2 {
public:
    ofxInverseFFT2();
    ~ofxInverseFFT2();
    
    // 주요 메서드
    void setup(int width = 256, int height = 256);
    void update();
    
    // 이미지 로딩 메서드
    bool loadImage(const string& path);
    void setImage(const ofImage& inputImage);
    
    // 결과 얻기 메서드
    ofImage& getOriginalImage();
    ofImage& getSpectrumImage();
    ofImage& getInverseImage();
    
    // 옵션 설정 메서드
    void setLogScale(bool useLog);
    void setNormalize(bool useNormalize);
    void setSpectrumShift(bool useShift);
    
    // 그리기 메서드
    void drawOriginal(float x, float y, float width, float height);
    void drawSpectrum(float x, float y, float width, float height);
    void drawInverse(float x, float y, float width, float height);
    void drawAll(float x, float y, float width, float height);
    
    // 마스킹 및 필터링 메서드
    void applyFrequencyMask(const cv::Mat& mask);
    void applyNotchFilter(float centerX, float centerY, float radius);
    void resetMask();
    
private:
    // 이미지 데이터
    ofImage originalImage;
    ofImage spectrumImage;
    ofImage inverseImage;
    
    // OpenCV 행렬
    cv::Mat originalMat;
    cv::Mat grayMat;
    cv::Mat resizedMat;
    cv::Mat complexMat;
    cv::Mat maskMat;
    
    // 옵션 플래그
    bool useLogScale;
    bool useNormalize;
    bool useShift;
    bool hasMask;
    
    // 내부 메서드
    void computeFFT();
    void computeIFFT();
    void applyFFTShift(cv::Mat& mat);
    void createDefaultMask();
};