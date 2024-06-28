#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "curl-8.8.0_2-win64-mingw/curl-8.8.0_2-win64-mingw/include/curl/curl.h"
#include "cJSONFiles/cJSON/cJSON.h"

#define API_URL "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent"
#define API_KEY "YOUR_KEY"
#define CSV_FILE "chat.csv"
#define BUFFER_SIZE 4096

void obterDataAtual(char *buffer, size_t buffer_size) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, buffer_size, "%d/%m/%Y %Hh%M", timeinfo);
}

void gerarCSV(const char *role, const char *datetime, const char *message) {
    FILE *fp = fopen(CSV_FILE, "a");
    if (fp == NULL) {
        perror("Falha ao abrir o arquivo CSV!");
        return;
    }

    fprintf(fp, "%s; %s; %s;\n", role, datetime, message);
    fclose(fp);
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

size_t escreverRetorno(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        printf("Memória insuficiente! A função realloc() retornou NULL.\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int enviarPerguntaAPI(const char *pergunta, char *resposta) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    char data[BUFFER_SIZE];
    char datetime[20];

    obterDataAtual(datetime, sizeof(datetime));
    gerarCSV("USER", datetime, pergunta);

    curl = curl_easy_init();
    if (curl) {
        snprintf(data, sizeof(data), "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}", pergunta);
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        char url_with_key[BUFFER_SIZE];
        snprintf(url_with_key, sizeof(url_with_key), "%s?key=%s", API_URL, API_KEY);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_URL, url_with_key);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escreverRetorno);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() falhou: %s\n", curl_easy_strerror(res));
            strcpy(resposta, "ERRO NA COMUNICAÇÃO");
            curl_easy_cleanup(curl);
            free(chunk.memory);
            return 1;
        }

        obterDataAtual(datetime, sizeof(datetime));

        strncpy(resposta, chunk.memory, BUFFER_SIZE - 1);
        resposta[BUFFER_SIZE - 1] = '\0';

        free(chunk.memory);

        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Falha ao iniciar a biblioteca CURL!\n");
        strcpy(resposta, "ERRO NA COMUNICAÇÃO");
        return 1;
    }

    return 0;
}

int main() {
    char pergunta[BUFFER_SIZE];
    char resposta[BUFFER_SIZE];
    char datetime[20];

    while (1) {
        printf("Digite sua pergunta ou digite \"sair\" para finalizar a interacao: ");
        if (fgets(pergunta, sizeof(pergunta), stdin) == NULL) {
            break;
        }

        pergunta[strcspn(pergunta, "\n")] = 0;

        if (strcmp(pergunta, "sair") == 0) {
            break;
        }

        if (enviarPerguntaAPI(pergunta, resposta) == 0) {
            cJSON *json = cJSON_Parse(resposta);
            if (json) {
                cJSON *candidates = cJSON_GetObjectItem(json, "candidates");
                if (candidates && cJSON_IsArray(candidates)) {
                    cJSON *candidate = cJSON_GetArrayItem(candidates, 0);
                    if (candidate) {
                        cJSON *content = cJSON_GetObjectItem(candidate, "content");
                        if (content) {
                            cJSON *parts = cJSON_GetObjectItem(content, "parts");
                            if (parts && cJSON_IsArray(parts)) {
                                cJSON *part = cJSON_GetArrayItem(parts, 0);
                                if (part) {
                                    cJSON *text = cJSON_GetObjectItem(part, "text");
                                    if (text && cJSON_IsString(text)) {
                                        printf("Resposta: %s\n", text->valuestring);
                                        obterDataAtual(datetime, sizeof(datetime));
                                        gerarCSV("LLM", datetime, text->valuestring);
                                    }
                                }
                            }
                        }
                    }
                }
                cJSON_Delete(json);
            }
        } else {
            printf("Erro na comunicação com a API.\n");
        }
    }

    return 0;
}
