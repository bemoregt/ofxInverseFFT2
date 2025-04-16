#include "ofxInverseFFT2.h"

using namespace ofxCv;
using namespace cv;

ofxInverseFFT2::ofxInverseFFT2() {
    useLogScale = true;
    useNormalize = true;
    useShift = false;
    hasMask = false;
}

ofxInverseFFT2::~ofxInverseFFT2() {
    // 필요한 정리 작업
}

void ofxInverseFFT2::setup(int width, int height) {
    // 기본 이미지 및 행렬 크기 초기화
    originalMat = Mat::zeros(height, width, CV_8UC3);
    grayMat = Mat::zeros(height, width, CV_8UC1);
    resizedMat = Mat::zeros(height, width, CV_8UC1);
    
    // 마스크 초기화
    createDefaultMask();
}

bool ofxInverseFFT2::loadImage(const string& path) {
    bool success = originalImage.load(path);
    if(success) {
        originalMat = toCv(originalImage);
        // 업데이트 처리
        update();
        return true;
    }
    return false;
}

void ofxInverseFFT2::setImage(const ofImage& inputImage) {
    originalImage = inputImage;
    originalMat = toCv(originalImage);
    // 업데이트 처리
    update();
}

ofImage& ofxInverseFFT2::getOriginalImage() {
    return originalImage;
}

ofImage& ofxInverseFFT2::getSpectrumImage() {
    return spectrumImage;
}

ofImage& ofxInverseFFT2::getInverseImage() {
    return inverseImage;
}

void ofxInverseFFT2::setLogScale(bool useLog) {
    useLogScale = useLog;
    if(originalMat.data) {
        update();
    }
}

void ofxInverseFFT2::setNormalize(bool useNorm) {
    useNormalize = useNorm;
    if(originalMat.data) {
        update();
    }
}

void ofxInverseFFT2::setSpectrumShift(bool useShiftVal) {
    useShift = useShiftVal;
    if(originalMat.data) {
        update();
    }
}

void ofxInverseFFT2::update() {
    if(!originalMat.data) return;
    
    // 이미지를 그레이스케일로 변환
    cvtColor(originalMat, grayMat, CV_BGR2GRAY);
    
    // 크기 조정 (256x256으로 제한)
    int maxDim = max(grayMat.cols, grayMat.rows);
    float ratio = 256.0 / maxDim;
    resize(grayMat, resizedMat, Size(grayMat.cols * ratio, grayMat.rows * ratio));
    
    // FFT 및 IFFT 계산
    computeFFT();
    computeIFFT();
}

void ofxInverseFFT2::computeFFT() {
    // 실수부와 허수부 준비
    Mat planes[] = {Mat_<float>(resizedMat), Mat::zeros(resizedMat.size(), CV_32F)};
    merge(planes, 2, complexMat);
    
    // DFT 계산
    dft(complexMat, complexMat);
    
    // FFT Shift 적용 (선택사항)
    if(useShift) {
        applyFFTShift(complexMat);
    }
    
    // 마스크 적용 (있는 경우)
    if(hasMask) {
        multiply(complexMat, maskMat, complexMat);
    }
    
    // 스펙트럼 이미지 계산
    split(complexMat, planes);
    Mat magnitudeMat, logMagnitudeMat, normalizedMat;
    
    // 크기 계산
    magnitude(planes[0], planes[1], magnitudeMat);
    
    // 로그 스케일 적용 (선택사항)
    if(useLogScale) {
        magnitudeMat += Scalar::all(1); // 로그(0) 방지
        log(magnitudeMat, logMagnitudeMat);
    } else {
        logMagnitudeMat = magnitudeMat;
    }
    
    // 시각화를 위한 정규화 (선택사항)
    if(useNormalize) {
        normalize(logMagnitudeMat, normalizedMat, 0, 255, NORM_MINMAX, CV_8U);
    } else {
        convertScaleAbs(logMagnitudeMat, normalizedMat, 1);
    }
    
    // OF 이미지로 변환
    toOf(normalizedMat, spectrumImage);
    spectrumImage.update();
}

void ofxInverseFFT2::computeIFFT() {
    // 역 DFT 계산
    Mat inverseComplexMat;
    dft(complexMat, inverseComplexMat, DFT_INVERSE | DFT_SCALE);
    
    // 실수부와 허수부 분리
    Mat planes[2];
    split(inverseComplexMat, planes);
    
    // 크기 계산
    Mat magnitudeMat, normalizedMat;
    magnitude(planes[0], planes[1], magnitudeMat);
    
    // 시각화를 위한 정규화
    if(useNormalize) {
        normalize(magnitudeMat, normalizedMat, 0, 255, NORM_MINMAX, CV_8U);
    } else {
        convertScaleAbs(magnitudeMat, normalizedMat, 1);
    }
    
    // OF 이미지로 변환
    toOf(normalizedMat, inverseImage);
    inverseImage.update();
}

void ofxInverseFFT2::applyFFTShift(Mat& mat) {
    int cx = mat.cols / 2;
    int cy = mat.rows / 2;
    
    Mat q0(mat, Rect(0, 0, cx, cy));
    Mat q1(mat, Rect(cx, 0, cx, cy));
    Mat q2(mat, Rect(0, cy, cx, cy));
    Mat q3(mat, Rect(cx, cy, cx, cy));
    
    Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

void ofxInverseFFT2::createDefaultMask() {
    if(resizedMat.data) {
        maskMat = Mat::ones(resizedMat.size(), CV_32FC2);
        hasMask = false;
    }
}

void ofxInverseFFT2::applyFrequencyMask(const Mat& mask) {
    if(mask.data && mask.size() == resizedMat.size()) {
        mask.copyTo(maskMat);
        hasMask = true;
        update();
    }
}

void ofxInverseFFT2::applyNotchFilter(float centerX, float centerY, float radius) {
    if(!resizedMat.data) return;
    
    // 노치 필터 생성 (특정 주파수 제거)
    maskMat = Mat::ones(resizedMat.size(), CV_32FC2);
    
    // 좌표 조정 (FFT Shift 고려)
    int cx = useShift ? centerX : centerX + resizedMat.cols / 2;
    int cy = useShift ? centerY : centerY + resizedMat.rows / 2;
    
    // 원형 마스크 생성
    for(int y = 0; y < maskMat.rows; y++) {
        for(int x = 0; x < maskMat.cols; x++) {
            float dist = sqrt(pow(x - cx, 2) + pow(y - cy, 2));
            if(dist < radius) {
                // 해당 주파수 영역을 제거 (0으로 설정)
                maskMat.at<Vec2f>(y, x)[0] = 0;
                maskMat.at<Vec2f>(y, x)[1] = 0;
            }
        }
    }
    
    hasMask = true;
    update();
}

void ofxInverseFFT2::resetMask() {
    createDefaultMask();
    update();
}

void ofxInverseFFT2::drawOriginal(float x, float y, float width, float height) {
    if(originalImage.isAllocated()) {
        originalImage.draw(x, y, width, height);
    }
}

void ofxInverseFFT2::drawSpectrum(float x, float y, float width, float height) {
    if(spectrumImage.isAllocated()) {
        spectrumImage.draw(x, y, width, height);
    }
}

void ofxInverseFFT2::drawInverse(float x, float y, float width, float height) {
    if(inverseImage.isAllocated()) {
        inverseImage.draw(x, y, width, height);
    }
}

void ofxInverseFFT2::drawAll(float x, float y, float width, float height) {
    float w = width / 3;
    drawOriginal(x, y, w, height);
    drawSpectrum(x + w, y, w, height);
    drawInverse(x + 2*w, y, w, height);
}