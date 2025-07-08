#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "utils.h"


int asknget(char *message) {
        char cevap[10];
        while (1) {
                printf("%s (y/n): ", message);
                if (fgets(cevap, sizeof(cevap), stdin)) {
                        if (cevap[0] == 'y' || cevap[0] == 'Y') return 1;
                        if (cevap[0] == 'n' || cevap[0] == 'N') return 0;
                }
                printf("Please just enter y or n.\n");
        }
}

void clear_quotes(char* str) {
    size_t len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        // Shift string left
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

void print_help(void) {
        printf(COLOR_CYAN "durpkg" COLOR_RESET " - Debian için minimalist paket yükleyici\n\n");
        printf("Kullanım:\n");
        printf("  durpkg [SEÇENEKLER] <pkgfile>\n\n");
        printf("Seçenekler:\n");
        printf("  -h, --help         Bu yardım mesajını göster\n");
        printf("  -s, --silent   Sessiz mod\n");
        printf("  -v, --version  Sürüm bilgisini göster\n\n");
        printf("Örnek:\n");
        printf("  durpkg paket.ini\n");
        printf("  durpkg -s paket.ini\n");
}

void print_version(void) {
        printf("durpkg 0.1.0\n");
}

int validate_url(char* url) {
        char str[1024];
        snprintf(str, sizeof(str), "curl -s --head --request GET %s | grep '200 OK' > /dev/null", url);
        int status = system(str);
        if (status == 0) {
                return 0;
        } else {
                return 1;
        }
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    return size * nmemb;
}

int check_url_exists(const char *url) {
    CURL *curl;
    CURLcode res;
    long response_code;
    
    curl = curl_easy_init();
    if (!curl) {
        printf("CURL initialization failed\n");
        return -1;
    }
    
    // URL'yi ayarla
    curl_easy_setopt(curl, CURLOPT_URL, url);
    
    // HEAD request kullan (sadece header bilgisi al)
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    
    // Callback fonksiyonu ayarla
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    
    // Timeout ayarla (10 saniye)
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    
    // SSL sertifika doğrulamasını devre dışı bırak (test için)
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    
    // User-Agent ayarla
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "URL-Checker/1.0");
    
    // Request'i gönder
    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        return 0; // URL erişilemez
    }
    
    // HTTP response code'unu al
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    curl_easy_cleanup(curl);
    
    // 200-299 arası kodlar başarılı sayılır
    if (response_code >= 200 && response_code < 300) {
        return 1; // URL var
    } else {
        printf("HTTP Response Code: %ld\n", response_code);
        return 0; // URL yok veya erişilemez
    }
}
