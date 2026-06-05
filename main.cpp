#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include "llama.h"
#include "httplib.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "Local LLM API Server 구동 시작!" << std::endl;

    llama_backend_init();
    const char* model_path = "C:/cpp-local-llm-server/models/Meta-Llama-3-8B-Instruct.Q4_K_M.gguf";
    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 99;
    llama_model* model = llama_model_load_from_file(model_path, model_params);

    if (model == nullptr) {
        std::cerr << "모델 로드 실패" << std::endl;
        return 1;
    }

    // ★ 최신 문법: 모델에서 '단어장(Vocabulary)' 부품만 따로 뽑아내기
    const llama_vocab* vocab = llama_model_get_vocab(model);

    // ★ 최신 문법: llama_init_from_model 사용
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 2048; 
    llama_context* ctx = llama_init_from_model(model, ctx_params);

    httplib::Server svr;

    svr.Get("/chat", [model, vocab, ctx](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("msg")) {
            res.set_content("명령어가 없습니다. 예: /chat?msg=안녕", "text/plain; charset=utf-8");
            return;
        }

        std::string prompt = req.get_param_value("msg");
        std::cout << "\n[사용자 질문]: " << prompt << std::endl;
        std::cout << "[AI 답변 생성 중]..." << std::flush;

        std::string formatted_prompt = "<|begin_of_text|><|start_header_id|>user<|end_header_id|>\n\n" + prompt + "<|eot_id|><|start_header_id|>assistant<|end_header_id|>\n\n";

        // ★ 최신 문법: tokenize 할 때 model 대신 단어장(vocab)을 넣어야 합니다!
        std::vector<llama_token> tokens_list(formatted_prompt.size() + 10);
        int n_tokens = llama_tokenize(vocab, formatted_prompt.c_str(), formatted_prompt.length(), tokens_list.data(), tokens_list.size(), true, true);
        if(n_tokens < 0) {
            res.set_content("프롬프트가 너무 깁니다.", "text/plain; charset=utf-8");
            return;
        }
        tokens_list.resize(n_tokens);

        // ★ 최신 문법: llama_batch_get_one 인수가 4개에서 3개로 줄어들었습니다! (seq_id 배열 제거)
        llama_batch batch = llama_batch_get_one(tokens_list.data(), tokens_list.size());
        if (llama_decode(ctx, batch) != 0) {
            res.set_content("엔진 평가 오류!", "text/plain; charset=utf-8");
            return;
        }

        std::string answer = "";
        
        // ★ 최신 문법: vocab_n_tokens 사용
        int n_vocab = llama_vocab_n_tokens(vocab);
        
        for (int i = 0; i < 1024; i++) {
            float* logits = llama_get_logits_ith(ctx, batch.n_tokens - 1);
            int new_token_id = 0;
            float max_val = logits[0];
            for (int v = 1; v < n_vocab; v++) {
                if (logits[v] > max_val) {
                    max_val = logits[v];
                    new_token_id = v;
                }
            }

            // ★ 최신 문법: EOG 확인 시 단어장(vocab) 사용
            if (llama_token_is_eog(vocab, new_token_id)) {
                break;
            }

            // ★ 최신 문법: 토큰을 글자로 바꿀 때도 단어장(vocab) 사용
            char buf[128] = {0};
            int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n > 0) {
                std::string piece(buf, n);
                answer += piece;
                std::cout << piece << std::flush;
            }

            // 다음 단어 예측을 위한 새로운 배치 설정
            llama_token new_token_id_arr[1] = { (llama_token)new_token_id };
            batch = llama_batch_get_one(new_token_id_arr, 1);
            llama_decode(ctx, batch);
        }
        
        std::cout << "\n[답변 전송 완료]" << std::endl;
        res.set_content(answer, "text/plain; charset=utf-8");
    });

    svr.set_mount_point("/", "./public");

    std::cout << "\n========================================================" << std::endl;
    std::cout << "챗봇 API 서버가 가동되었습니다!" << std::endl;
    std::cout << "브라우저 주소창에 아래처럼 질문을 던져보세요!" << std::endl;
    std::cout << "   http://localhost:8080/chat?msg=Who are you?" << std::endl;
    std::cout << "========================================================\n" << std::endl;
    
    svr.listen("0.0.0.0", 8080);

    llama_free(ctx);
    llama_model_free(model);
    llama_backend_free();
    return 0;
}