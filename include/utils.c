#include <stdio.h>
#include <string.h>
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

