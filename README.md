# ofxInverseFFT2

2D FFT 및 역 FFT(Inverse FFT) 처리와 시각화를 위한 openFrameworks 애드온입니다.

## 개요

ofxInverseFFT2는 이미지에 2D FFT(Fast Fourier Transform)와 역 FFT를 적용하여 주파수 도메인에서의 이미지 처리와 필터링을 할 수 있는 도구입니다. 원본 이미지, 주파수 스펙트럼, 그리고 역변환된 이미지를 시각화하고, 주파수 도메인에서 노치 필터와 같은 필터링을 적용할 수 있습니다.

![FFT Example](images/example.png)

## 기능

- 이미지 2D FFT 변환 및 시각화
- 주파수 스펙트럼 표시 (로그 스케일 옵션)
- 역 FFT 변환 및 결과 시각화
- 주파수 도메인에서의 필터링 (노치 필터)
- FFT Shift 지원
- 결과 이미지 저장

## 의존성

- openFrameworks 0.11.0+
- ofxOpenCv
- ofxCv
- ofxGui (예제용)

## 설치 방법

1. 이 저장소를 openFrameworks의 addons 폴더에 클론합니다:
```
cd openFrameworks/addons
git clone https://github.com/bemoregt/ofxInverseFFT2.git
```

2. Project Generator를 사용하여 새 프로젝트를 생성하거나 기존 프로젝트에 추가합니다.

## 사용 방법

```cpp
#include "ofxInverseFFT2.h"

// 클래스 내부
ofxInverseFFT2 fftProcessor;

// setup() 함수 내부
fftProcessor.setup();
fftProcessor.loadImage("image.jpg");

// update() 함수 내부
fftProcessor.update();

// draw() 함수 내부
fftProcessor.drawAll(0, 0, ofGetWidth(), ofGetHeight());

// 또는 개별적으로
fftProcessor.drawOriginal(0, 0, 300, 300);
fftProcessor.drawSpectrum(300, 0, 300, 300);
fftProcessor.drawInverse(600, 0, 300, 300);

// 노치 필터 적용
fftProcessor.applyNotchFilter(128, 128, 10); // x, y, 반지름
```

## 주요 메서드

- `setup(int width, int height)` - 초기화 (기본값: 256x256)
- `loadImage(const string& path)` - 이미지 파일 로드
- `setImage(const ofImage& inputImage)` - ofImage 객체 설정
- `update()` - FFT 및 역 FFT 계산 갱신
- `getOriginalImage()`, `getSpectrumImage()`, `getInverseImage()` - 결과 이미지 가져오기
- `setLogScale(bool)`, `setNormalize(bool)`, `setSpectrumShift(bool)` - 옵션 설정
- `applyNotchFilter(float x, float y, float radius)` - 노치 필터 적용
- `resetMask()` - 필터 초기화
- `drawOriginal()`, `drawSpectrum()`, `drawInverse()`, `drawAll()` - 결과 시각화

## 예제

제공된 예제는 다음과 같은 기능을 보여줍니다:

1. 이미지 로드 및 FFT 처리
2. GUI를 통한 옵션 설정 (로그 스케일, FFT Shift, 정규화)
3. 마우스 클릭을 통한 노치 필터 적용
4. 결과 이미지 저장

## 이론적 배경

FFT(Fast Fourier Transform)는 신호나 이미지를 주파수 도메인으로 변환하는 알고리즘입니다. 2D FFT는 이미지의 공간적 특성을 주파수 특성으로 변환하며, 다음과 같은 응용이 가능합니다:

- 이미지 필터링 (저주파/고주파 필터)
- 노이즈 제거
- 이미지 압축
- 패턴 및 특징 검출

역 FFT는 주파수 도메인에서 다시 공간 도메인으로 변환하는 과정입니다.

## 성능 고려사항

- 이미지 크기가 커질수록 계산 시간이 증가합니다.
- 실시간 처리를 위해 이미지 크기를 256x256으로 제한하는 것을 권장합니다.
- 노치 필터링 적용 시 업데이트 시간이 길어질 수 있습니다.

## 향후 개발 계획

- 다양한 주파수 필터 추가 (저역/고역/대역 통과 필터)
- 멀티스레딩 지원으로 성능 향상
- 실시간 비디오 스트림 처리
- 주파수 도메인 이미지 합성 도구

## 라이센스

MIT

## 출처

이 애드온은 Fast Fourier Transform과 이미지 처리 원리를 기반으로 하며, OpenCV의 DFT 구현을 활용합니다.
