#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "include/ini.h"
#include "include/utils.h"

#define MAX_LINE_LENGTH 1024
#define MAX_FIELD_LENGTH 256
#define MAX_SCRIPT_LENGTH 2048
#define TEMP_DIR "/tmp/durpkg"


static int silent = 0;

typedef struct {
        char name[MAX_FIELD_LENGTH];          // Paket ismi
        char author[MAX_FIELD_LENGTH];        // 
        char version[MAX_FIELD_LENGTH];       // 
        char description[MAX_FIELD_LENGTH];   // 
        char install_type[MAX_FIELD_LENGTH];  //  
        char source[MAX_FIELD_LENGTH];        // 
        char deb_source[MAX_FIELD_LENGTH];    //  
        char help_command[MAX_FIELD_LENGTH];  // 
        char help_page[MAX_FIELD_LENGTH];     // 
        char setup[MAX_SCRIPT_LENGTH];        // 
        char install[MAX_SCRIPT_LENGTH];      // 
        char config[MAX_SCRIPT_LENGTH];       // 
        int needs;                            // 
} pkgfile;

static int ini_read(void* user, const char* section, const char* name,
                      const char* value) {
        pkgfile *pkgf = (pkgfile*)user;

        if (strcmp(section, "package") == 0) {
                if (strcmp(name, "name") == 0) {
                        strncpy(pkgf->name, value, sizeof(pkgf->name));
                        clear_quotes(pkgf->name);

                } else if (strcmp(name, "author") == 0) {
                        strncpy(pkgf->author, value, sizeof(pkgf->author));
                        clear_quotes(pkgf->author);

                } else if (strcmp(name, "version") == 0) {
                        strncpy(pkgf->version, value, sizeof(pkgf->version));
                        clear_quotes(pkgf->version);

                } else if (strcmp(name, "description") == 0) {
                        strncpy(pkgf->description, value, sizeof(pkgf->description));
                        clear_quotes(pkgf->description);

                } else if (strcmp(name, "source") == 0) {
                        strncpy(pkgf->source, value, sizeof(pkgf->source));
                        clear_quotes(pkgf->source);

                } else if (strcmp(name, "needs") == 0) {
                        char *endptr;
                        int num = (int)strtol(value, &endptr, 10);
                        if (*endptr != '\0')
                                return 0;

                } else if (strcmp(name, "install_type") == 0) {
                        strncpy(pkgf->install_type, value, sizeof(pkgf->install_type));
                        clear_quotes(pkgf->install_type);

                } else if (strcmp(name, "deb_source") == 0) {
                        strncpy(pkgf->deb_source, value, sizeof(pkgf->deb_source));
                        clear_quotes(pkgf->deb_source);

                } else if (strcmp(name, "help_command") == 0) {
                        strncpy(pkgf->help_command, value, sizeof(pkgf->help_command));
                        clear_quotes(pkgf->help_command);

                } else if (strcmp(name, "help_page") == 0) {
                        strncpy(pkgf->help_page, value, sizeof(pkgf->help_page));
                        clear_quotes(pkgf->help_page);

                } else
                        return 0;

        } else if (strcmp(section, "scripts") == 0) {
                if (strcmp(name, "setup") == 0) {
                        strncpy(pkgf->setup, value, sizeof(pkgf->setup));
                        clear_quotes(pkgf->setup);

                } else if (strcmp(name, "install") == 0) {
                        strncat(pkgf->install, value, sizeof(pkgf->install) - strlen(pkgf->install) - 1);
                        strncat(pkgf->install, "\n", sizeof(pkgf->install) - strlen(pkgf->install) - 1);
                        printf("install value: %s\n", value);

                } else if (strcmp(name, "config") == 0) {
                        strncpy(pkgf->config, value, sizeof(pkgf->config));
                        clear_quotes(pkgf->config);

                } else
                        return 0;
        } else 
                return 0;

        return 1;
}

int safe_execute(const char *command) {
        if (!command || strlen(command) == 0) {
                return 0;
        }
        
        // Tehlikeli karakterleri kontrol et
        if (strpbrk(command, ";|`$")) {
                printf(COLOR_RED "HATA:" COLOR_RESET " Tehlikeli karakterler tespit edildi!\n");
                return -1;
        }
        
        if (!silent) {
                printf(COLOR_YELLOW "Komut:" COLOR_RESET " %s\n", command);
        }
        int status = system(command);
        
        if (status == -1) {
                perror("Komut çalıştırılamadı");
                return -1;
        }
        
        if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                if (exit_code != 0) {
                        printf(COLOR_RED "HATA:" COLOR_RESET " Komut hata kodu %d ile çıktı\n", exit_code);
                        return exit_code;
                }
        }
        
        return 0;
}

void display_package_info(const pkgfile *pkg) {
        printf("\n" COLOR_CYAN "═══════════════════════════════════════════════════════════════════════════════" COLOR_RESET "\n");
        printf(COLOR_MAGENTA "  PAKET BİLGİLERİ" COLOR_RESET "\n");
        printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════════════════════" COLOR_RESET "\n");
        printf(COLOR_YELLOW "  Paket Adı        :" COLOR_RESET " %s\n", pkg->name);
        printf(COLOR_YELLOW "  Yazar            :" COLOR_RESET " %s\n", pkg->author);
        printf(COLOR_YELLOW "  Sürüm            :" COLOR_RESET " %s\n", pkg->version);
        printf(COLOR_YELLOW "  Açıklama         :" COLOR_RESET " %s\n", pkg->description);
        
        if (strlen(pkg->install_type) > 0) {
                printf(COLOR_YELLOW "  Yükleme Tipi     :" COLOR_RESET " %s\n", pkg->install_type);
        }
        
        if (strlen(pkg->source) > 0) {
                printf(COLOR_YELLOW "  Kaynak           :" COLOR_RESET " %s\n", pkg->source);
        }
        
        if (strlen(pkg->help_command) > 0) {
                printf(COLOR_YELLOW "  Yardım Komutu    :" COLOR_RESET " %s\n", pkg->help_command);
        }
        
        printf(COLOR_CYAN "═══════════════════════════════════════════════════════════════════════════════" COLOR_RESET "\n");
}

int download_source(pkgfile *pkg) {
        if (!pkg->needs || strlen(pkg->source) == 0) {
                return 0;
        }
        
        char command[512];
        char temp_path[256];
        
        snprintf(temp_path, sizeof(temp_path), "%s/%s", TEMP_DIR, pkg->name);
        
        snprintf(command, sizeof(command), "mkdir -p %s", TEMP_DIR);
        if (safe_execute(command) != 0) {
                printf(COLOR_RED "HATA:" COLOR_RESET " Geçici dizin oluşturulamadı\n");
                return -1;
        }
        
        snprintf(command, sizeof(command), "rm -rf %s", temp_path);
        safe_execute(command);
        
        // Kaynak kodunu indir
        printf(COLOR_BLUE "Kaynak kodu indiriliyor..." COLOR_RESET "\n");
        snprintf(command, sizeof(command), "git clone %s %s", pkg->source, temp_path);
        
        if (safe_execute(command) != 0) {
                printf(COLOR_RED "HATA:" COLOR_RESET " Kaynak kodu indirilemedi\n");
                return -1;
        }
        
        return 0;
}

int install_deb_package(pkgfile *pkg) {
        if (strlen(pkg->deb_source) == 0) {
                return 0;
        }
        
        char command[512];
        char deb_path[256];
        
        snprintf(deb_path, sizeof(deb_path), "%s/%s.deb", TEMP_DIR, pkg->name);
        
        // .deb dosyasını indir
        printf(COLOR_BLUE "DEB paketi indiriliyor..." COLOR_RESET "\n");
        snprintf(command, sizeof(command), "wget -O %s %s", deb_path, pkg->deb_source);
        
        if (safe_execute(command) != 0) {
                printf(COLOR_RED "HATA:" COLOR_RESET " DEB paketi indirilemedi\n");
                return -1;
        }
        
        // .deb paketini yükle
        printf(COLOR_BLUE "DEB paketi yükleniyor..." COLOR_RESET "\n");
        snprintf(command, sizeof(command), "sudo dpkg -i %s", deb_path);
        
        if (safe_execute(command) != 0) {
                printf(COLOR_YELLOW "UYARI:" COLOR_RESET " DEB yüklemesi tamamlanamadı, bağımlılıklar düzeltiliyor...\n");
                if (safe_execute("sudo apt-get install -f") != 0) {
                        printf(COLOR_RED "HATA:" COLOR_RESET " Bağımlılıklar düzeltilemedi\n");
                        return -1;
                }
        }
        
        // Geçici dosyayı temizle
        snprintf(command, sizeof(command), "rm -f %s", deb_path);
        safe_execute(command);
        
        return 0;
}

int install_package(pkgfile *pkg) {
        int result = 0;
        
        display_package_info(pkg);
        
        if (!asknget("Bu paketi yüklemek istediğinizden emin misiniz?")) {
                printf(COLOR_YELLOW "Yükleme iptal edildi." COLOR_RESET "\n");
                return 1;
        }
        
        if (download_source(pkg) != 0) {
                return -1;
        }
        
        // Setup scriptini çalıştır
        if (strlen(pkg->setup) > 0) {
                printf(COLOR_BLUE "Kurulum öncesi hazırlıkları yapılıyor..." COLOR_RESET "\n");
                if (safe_execute(pkg->setup) != 0) {
                        printf(COLOR_RED "HATA:" COLOR_RESET " Kurulum öncesi hazırlıklar başarısız\n");
                        return -1;
                }
        }
        
        // Install türüne göre yükleme
        if (strcmp(pkg->install_type, "deb") == 0) {
                if (install_deb_package(pkg) != 0) {
                        return -1;
                }
        }
        
        // Install scriptini çalıştır
        if (strlen(pkg->install) > 0) {
                printf(COLOR_BLUE "Paket yükleniyor..." COLOR_RESET "\n");
                
                char command[MAX_SCRIPT_LENGTH + 256];
                snprintf(command, sizeof(command), "mkdir -p %s/%s", TEMP_DIR, pkg->name);
                
                if (safe_execute(command) != 0) {
                        printf(COLOR_RED "HATA:" COLOR_RESET " Paket yüklenemedi\n");
                        return -1;
                }

                snprintf(command, sizeof(command), "cd %s/%s && %s", TEMP_DIR, pkg->name, pkg->install);
                
                if (safe_execute(command) != 0) {
                        printf(COLOR_RED "HATA:" COLOR_RESET " Paket yüklenemedi\n");
                        return -1;
                }
        }
        
        // Config scriptini çalıştır
        if (strlen(pkg->config) > 0) {
                printf(COLOR_BLUE "Paket yapılandırılıyor..." COLOR_RESET "\n");
                if (safe_execute(pkg->config) != 0) {
                        printf(COLOR_YELLOW "UYARI:" COLOR_RESET " Paket yapılandırması başarısız\n");
                        result = -1;
                }
        }
        
        // Geçici dosyaları temizle
        if (pkg->needs) {
                char command[512];
                snprintf(command, sizeof(command), "rm -rf %s/%s", TEMP_DIR, pkg->name);
                safe_execute(command);
        }
        
        if (result == 0) {
                printf(COLOR_GREEN "✓ Paket başarıyla yüklendi: %s" COLOR_RESET "\n", pkg->name);
                
                if (strlen(pkg->help_command) > 0) {
                        printf(COLOR_CYAN "Yardım için: %s" COLOR_RESET "\n", pkg->help_command);
                }
                
                if (strlen(pkg->help_page) > 0) {
                        printf(COLOR_CYAN "Dokümantasyon: %s" COLOR_RESET "\n", pkg->help_page);
                }
        } else {
                printf(COLOR_YELLOW "⚠ Paket yüklendi ancak bazı hatalar oluştu" COLOR_RESET "\n");
        }
        
        return result;
}


int parse_arguments(int argc, char *argv[], char **pkgf) {
        *pkgf = NULL;
        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        print_help();
                        return 0;
                } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
                        print_version();
                        return 0;
                } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--silent") == 0) {
                        silent = 1;
                } else if (argv[i][0] == '-') {
                        printf(COLOR_RED "HATA:" COLOR_RESET " Bilinmeyen seçenek: %s\n", argv[i]);
                        return -1;
                } else {
                        if (*pkgf == NULL) {
                                *pkgf = argv[i];
                        } else {
                                printf(COLOR_RED "HATA:" COLOR_RESET " Sadece bir pkgfile belirtin\n");
                                return -1;
                        }
                }
        }
        
        return 1;
}

int main(int argc, char *argv[]) {
        char *pkgfp = NULL;
        pkgfile *pkg = malloc(sizeof(pkgfile));
    
        int arg_result = parse_arguments(argc, argv, &pkgfp);
        if (arg_result <= 0) {
                return (arg_result == 0) ? 0 : 1;
        }
    
        if (pkgfp == NULL) {
                printf(COLOR_RED "HATA:" COLOR_RESET " Pkgfile belirtmelisiniz\n");
                printf("Yardım için: durpkg --help\n");
                return 1;
        }
    
        if (access(pkgfp, R_OK) != 0) {
                printf(COLOR_RED "HATA:" COLOR_RESET " Dosya okunamıyor: %s\n", pkgfp);
                return 1;
        }

        if (ini_parse(pkgfp, ini_read, pkg) < 0) {
                printf(COLOR_RED "HATA:" COLOR_RESET " Pkgfile parse edilemedi\n");
                return 1;
        }

        printf("INSTALL_KOMUT_DEBUG: %s", pkg->install);
    
        // Geçici dizini oluştur
        char command[256];
        snprintf(command, sizeof(command), "mkdir -p %s", TEMP_DIR);
        safe_execute(command);
    
        // Paketi yükle
        int result = install_package(pkg);
    
        // Geçici dizini temizle
        snprintf(command, sizeof(command), "rm -rf %s", TEMP_DIR);
        safe_execute(command);
    
        free(pkg);
        return (result == 0) ? 0 : 1;
}

