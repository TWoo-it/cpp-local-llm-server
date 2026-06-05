cmake_minimum_required(VERSION 3.10)
project(CppLocalLlmServer)

set(CMAKE_CXX_STANDARD 17)

# 실행 파일과 DLL 부품이 같은 폴더에 생성되도록 강제 설정
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 인텔 GPU(Vulkan) 가속 스위치 ON (반드시 add_subdirectory 윗줄에 있어야 함)
set(GGML_VULKAN ON)

# llama.cpp 엔진 포함
add_subdirectory(third_party/llama.cpp)

# 메인 실행 파일 지정
add_executable(llm_server main.cpp)

# llama 라이브러리 연결
target_link_libraries(llm_server PRIVATE llama)