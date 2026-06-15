# C++ Local LLM API Server & Web Chat

![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B)
![CMake](https://img.shields.io/badge/CMake-Build-064F8C?logo=cmake)
![Vulkan](https://img.shields.io/badge/Vulkan-GPU_Acceleration-CC292B?logo=vulkan)
![Llama3](https://img.shields.io/badge/Model-Llama_3_(8B)-blue)

## 프로젝트 소개
**"C++ 백엔드부터 로컬 LLM 추론, 프론트엔드 UI까지 이어지는 Full-Stack AI 아키텍처 구현"**

외부 API(OpenAI 등)에 의존하지 않고, 내 PC(로컬) 메모리에 대형 언어 모델(LLM)을 직접 적재하여 구동하는 **독립형 AI 웹 서버**입니다. C++과 `llama.cpp` 엔진을 기반으로 모델 추론 파이프라인을 바닥부터 구축하였으며, 하드웨어 가속(Vulkan)을 통해 내장 GPU의 성능을 100% 끌어냈습니다.

> ** 실행 화면 (Web UI & Terminal)** > ![실행 화면 스크린샷]() 

##  기술 스택 (Tech Stack)
* **Backend Engine:** C++17, CMake
* **AI Inference:** `llama.cpp` (GGUF format)
* **Hardware Acceleration:** Vulkan SDK (Intel Iris Xe GPU 최적화)
* **Network/API:** `cpp-httplib` (REST API Server)
* **Frontend:** HTML5, CSS3, Vanilla JavaScript

##  주요 기능 (Key Features)
1. **대규모 모델 메모리 로드:** 5GB 규모의 `Llama-3-8B-Instruct` 모델을 GGUF 포맷으로 안전하게 메모리에 적재 및 해제.
2. **REST API 기반 챗봇 엔드포인트:** `/chat` 엔드포인트를 통해 클라이언트의 프롬프트를 수신하고, AI의 답변을 텍스트로 반환.
3. **GPU 하드웨어 가속 (Vulkan):** 무거운 딥러닝 행렬 연산을 CPU에서 GPU로 오프로딩(Offloading)하여 추론 속도 대폭 향상.
4. **대화형 Web UI:** ChatGPT 스타일의 비동기(Fetch API) 채팅 인터페이스 제공.

##  프로젝트 구조 (Project Structure)
```text
📦 cpp-local-llm-server
 ┣ 📂 models           # Llama 3 GGUF 모델 파일 저장 위치 (gitignore)
 ┣ 📂 public           # 프론트엔드 정적 파일
 ┃ ┗ 📜 index.html     # Web UI 채팅창 화면
 ┣ 📂 third_party      # 외부 라이브러리 (Submodules)
 ┃ ┗ 📂 llama.cpp      # 핵심 AI 추론 엔진 소스코드
 ┣ 📜 CMakeLists.txt   # CMake 빌드 설계도
 ┣ 📜 httplib.h        # C++ REST API 웹 서버 라이브러리
 ┗ 📜 main.cpp         # 백엔드 서버 메인 로직 및 추론 파이프라인
 ```
## 트러블슈팅 및 문제 해결 과정 (Troubleshooting)
1. 윈도우 네트워크 라이브러리(Winsock) 충돌 문제
Issue: cpp-httplib 웹 서버 구축 시, <windows.h>의 구형 네트워크 헤더와 신형 헤더가 충돌하여 100개 이상의 매크로 재정의(C4005) 에러 발생.

Solution: C++ 코드 최상단에 #define WIN32_LEAN_AND_MEAN 매크로를 선언하여 구형 네트워크 컴포넌트 로드를 차단, 아키텍처 충돌을 완벽하게 해결함.

2. 최신 오픈소스 API 업데이트 대응 (llama_vocab 분리)
Issue: 오픈소스(llama.cpp)의 대규모 업데이트로 인해 기존 llama_model 객체에서 단어장(Vocabulary) 기능이 분리되어 빌드 에러(C2664) 발생.

Solution: 라이브러리의 최신 PR 및 문서를 분석하여, llama_model_get_vocab() 함수로 사전(Vocab) 객체를 독립적으로 추출하고 Tokenization 과정에 주입하도록 추론 파이프라인 코드를 전면 리팩토링함.

3. 내장 GPU(Intel Iris Xe) 가속 적용
Issue: 순수 CPU 연산만으로는 8B 파라미터 모델의 추론 속도가 현저히 느려 실시간 서비스가 불가능함. (CUDA는 NVIDIA 환경에서만 동작)

Solution: 플랫폼 독립적인 범용 그래픽 API인 Vulkan SDK를 도입. CMake 빌드 옵션에 GGML_VULKAN=ON을 적용하여 인텔 내장 그래픽 코어로 신경망 연산을 분산(GPU Layers=99), 획기적인 속도 개선 달성.

## 실행 방법 (Getting Started)
사전 요구 사항 (Prerequisites)
이 프로젝트를 빌드하고 실행하려면 아래 도구들이 필요합니다.

Git

CMake (버전 3.10 이상)

Visual Studio Build Tools (C++ 컴파일러)

Vulkan SDK (GPU 가속용)

## 사전 준비: AI 모델 다운로드 (필수)
깃허브 용량 제한으로 인해 대형 언어 모델 파일(`.gguf`)은 포함되어 있지 않습니다.
서버를 빌드하기 전에 반드시 아래 지침에 따라 모델을 다운로드해주세요.

1. [Hugging Face - Llama 3 8B GGUF](https://huggingface.co/QuantFactory/Meta-Llama-3-8B-Instruct-GGUF) 페이지에 접속합니다.
2. `Meta-Llama-3-8B-Instruct.Q4_K_M.gguf` 파일을 다운로드합니다.
3. 다운로드한 파일을 프로젝트 루트의 `models/` 폴더 안에 넣어주세요. (폴더가 없다면 생성)

**최종 파일 경로 확인:** `cpp-local-llm-server/models/Meta-Llama-3-8B-Instruct.Q4_K_M.gguf`

## 빌드 및 실행 명령어
# 1. 저장소 클론 및 서브모듈(llama.cpp) 초기화
git clone [https://github.com/TWoo-it/cpp-local-llm-server.git](https://github.com/TWoo-it/cpp-local-llm-server.git)
git submodule update --init --recursive

# 2. 빌드 (build 폴더 생성 후 컴파일)
mkdir build && cd build
cmake ..
cmake --build . --config Debug

# 3. 서버 실행 (8080 포트 대기, Windows환경 실행 명령어)
# 방법 A: build 폴더 안에서 바로 실행할 경우
.\bin\Debug\llm_server.exe

# 방법 B: 절대 경로로 실행 (어느 위치에서든 가장 확실한 방법)
C:\cpp-local-llm-server\build\bin\Debug\llm_server.exe

## 실행 확인: 서버가 켜지면 웹 브라우저에서 http://localhost:8080 으로 접속하여 채팅을 시작할 수 있습니다!

## 향후 개선 목표 (To-Do)
이 프로젝트를 통해 모델이 메모리에 로드되고 텍스트가 토큰화되어 연산되는 Low-level 원리를 깊이 이해할 수 있었습니다.

추후에는 모델이 답변을 한 글자씩 실시간으로 뱉어내는 스트리밍(Server-Sent Events) 기능을 추가하여 사용자 경험(UX)을 더욱 개선해보고 싶습니다.

## Acknowledgements & License
Core Engine: llama.cpp by Georgi Gerganov

Web Server: cpp-httplib by yhirose

본 프로젝트는 학습 및 포트폴리오 목적으로 제작되었습니다.

## Contact
Author: 김태우

Email: kimha100402@gmail.com

GitHub: https://github.com/TWoo-it