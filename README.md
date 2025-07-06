# durpkg 

**durpkg** - Debian sistemleri için minimalist, güvenli ve kullanıcı dostu paket yükleyici

## 📋 İçindekiler

- [Nedir?](#nedir)
- [Özellikler](#özellikler)
- [Kurulum](#kurulum)
- [Kullanım](#kullanım)
- [Pkgfile Formatı](#pkgfile-formatı)
- [Örnekler](#örnekler)
- [Güvenlik](#güvenlik)
- [SSS](#sss)

## Nedir?

**durpkg**, Debian tabanlı sistemlerde resmi apt depolarında bulunmayan yazılımları kolayca yüklemenizi sağlayan bir paket yöneticisidir. Basit INI formatında yazılmış pkgfile'lar kullanarak paketleri güvenli bir şekilde sisteminize yükler.

### Neden durpkg?

- 🎯 **Basit ve Odaklı**: Sadece paket yükleme işine odaklanır
- 🛡️ **Güvenli**: Command injection koruması ve güvenlik kontrolleri
- 🎨 **Kullanıcı Dostu**: Renkli çıktılar ve açık hata mesajları
- 📝 **Kolay Pkgfile**: INI formatında basit paket tanımlama
- 🚀 **Hızlı**: Minimal bağımlılık, hızlı çalışma

## Özellikler

### ✅ Temel Özellikler

- **Çoklu Yükleme Tipi**: DEB paketi, kaynak kod, özel script desteği
- **Otomatik Kaynak İndirme**: Git repository'lerinden otomatik kod indirme
- **Bağımlılık Yönetimi**: Kurulum öncesi bağımlılık kontrolü
- **Yapılandırma Scriptleri**: Kurulum sonrası otomatik yapılandırma

### 🛡️ Güvenlik Özellikleri

- **Command Injection Koruması**: Tehlikeli karakter kontrolü
- **Buffer Overflow Koruması**: Güvenli bellek yönetimi
- **Kullanıcı Onayı**: Her kritik işlem için kullanıcı onayı
- **Geçici Dosya Temizliği**: Otomatik geçici dosya silme

### 🎨 Kullanıcı Deneyimi

- **Renkli Terminal Çıktıları**: Kolay takip edilebilir çıktılar
- **Detaylı Bilgilendirme**: Paket bilgileri ve ilerleme durumu
- **Sessiz Mod**: Otomatik kurulum için sessiz çalışma
- **Hata Toleransı**: Force mode ile hatalara rağmen devam etme

## Kurulum

### Gereksinimler

- GCC derleyicisi
- Git (kaynak kod indirme için)
- wget (DEB paketleri için)

### İndirme

```bash
git clone https://github.com/mmapro12/durpkg.git 
cd durpkg 
chmod +x install.sh 
./install.sh
```

## Kullanım

### Temel Kullanım

```bash
# Basit paket yükleme
durpkg paket.ini
```

### Komut Satırı Seçenekleri

```bash
durpkg [SEÇENEKLER] <pkgfile>
```

| Seçenek | Kısa | Açıklama |
|---------|------|----------|
| `--help` | `-h` | Yardım mesajını göster |
| `--version` | `-v` | Sürüm bilgisini göster |
| `--silent` | `-s` | Sessiz mod (onay istemez) |

### Kullanım Örnekleri

```bash
# Normal kurulum
durpkg pkgfile.turkman.ini

# Sessiz kurulum (CI/CD için ideal)
durpkg -s paket.ini

# Yardım alma
durpkg --help
```

## Pkgfile Formatı

Pkgfile, paket bilgilerini ve kurulum talimatlarını içeren INI formatında bir dosyadır.

### Temel Yapı

```ini
[package]
# Paket bilgileri

[scripts]
# Kurulum scriptleri
```

### Package Bölümü

```ini
[package]
name = paket-adi                 # Paketin benzersiz adı (zorunlu)
author = "yazar-adi              # Paketin yazarı
version = 1.0.0                  # Paket sürümü
description = Paket açıklaması   # Kısa açıklama

# Kurulum tipi
install_type = "deb"             # deb, script, custom

# Kaynak kodları
source = "https://github.com/user/repo.git"  # Git repository
deb_source = "https://example.com/pkg.deb"   # DEB dosyası URL'i
needs = 1                                    # Kaynak kod gerekli mi? (0/1)

# Yardım bilgileri
help_command = "paket --help"           # Yardım komutu
help_page = "https://docs.example.com"  # Dokümantasyon URL'i
```

#### Package Alanları Detayı

| Alan | Zorunlu | Açıklama |
|------|---------|----------|
| `name` | ✅ | Paketin benzersiz adı |
| `author` | ❌ | Paket yazarı |
| `version` | ❌ | Paket sürümü |
| `description` | ❌ | Paket açıklaması |
| `install_type` | ❌ | Kurulum tipi (`deb`, `script`) |
| `source` | ❌ | Git repository URL'i |
| `deb_source` | ❌ | DEB dosyası URL'i |
| `needs` | ❌ | Kaynak kod gereksinimi (`0` veya `1`) |
| `help_command` | ❌ | Yardım komutu |
| `help_page` | ❌ | Dokümantasyon URL'i |

### Scripts Bölümü

```ini
[scripts]
# Kurulum öncesi hazırlık
setup = sudo apt install git

# Ana kurulum scripti (sanki source içinde işlem yapıyormuş gibi yapınız)
install = ./install.sh 

# Kurulum sonrası yapılandırma
config = turkman db sync

# Kaldırma scripti (gelecek kullanım için)
remove = turkman uninstall

# Güncelleme scripti (gelecek kullanım için)
update = turkman update
```

#### Script Alanları

| Script | Açıklama | Çalışma Zamanı |
|--------|----------|----------------|
| `setup` | Sistem bağımlılıklarını yükler | Kurulum öncesi |
| `install` | Ana kurulum işlemini yapar | Ana kurulum |
| `config` | Kurulum sonrası yapılandırma | Kurulum sonrası |
| `remove` | Paketi kaldırır | Manuel çalıştırma |
| `update` | Paketi günceller | Manuel çalıştırma |

## Örnekler

### Örnek 1: DEB Paketi Kurulumu

```ini
[package]
name = "turkman"
author = "mmapro12"
version = "0.6.5"
description = "Türkçe Linux man sayfaları okuma aracı"
install_type = "deb"
source = "https://github.com/mmapro12/turkman.git"
deb_source = "https://github.com/mmapro12/turkman/releases/latest/download/turkman_0.6.5_all.deb"
needs = 0
help_command = "turkman --help"
help_page = "https://github.com/turkman/turkman/issues"

[scripts]
setup = """
sudo apt update
sudo apt install -y manpages-tr
"""

install = ""

config = """
echo "Turkman başarıyla kuruldu!"
echo "Kullanım: turkman <komut>"
"""
```

### Örnek 2: Kaynak Koddan Kurulum

```ini
[package]
name = "my-tool"
author = "developer"
version = "1.2.3"
description = "Özel geliştirme aracı"
install_type = "script"
source = "https://github.com/user/my-tool.git"
needs = 1
help_command = "my-tool --help"

[scripts]
setup = sudo apt install -y build-essential cmake

install = ./install.sh

config = echo "my-tool kuruldu!"

```

## Güvenlik

durpkg güvenliği ciddiye alır ve aşağıdaki önlemleri içerir:

### 🛡️ Güvenlik Özellikleri

- **Command Injection Koruması**: Tehlikeli karakterler (`; & | \` $`) kontrol edilir
- **Buffer Overflow Koruması**: Tüm string işlemleri güvenli fonksiyonlarla yapılır
- **Kullanıcı Onayı**: Her kritik işlem için kullanıcı onayı alınır
- **Geçici Dosya Güvenliği**: Geçici dosyalar otomatik olarak temizlenir

### 🚨 Güvenlik Uyarıları

- **Sadece güvendiğiniz pkgfile'ları kullanın**
- **Pkgfile içeriğini kurulum öncesi kontrol edin**
- **Bilinmeyen kaynaklardan gelen paketleri yüklemeyin**
- **Sudo yetkisi gerektiren scriptleri dikkatli inceleyin**

### 📋 Güvenlik Kontrol Listesi

Bir pkgfile'ı kullanmadan önce:

- [ ] Paket yazarı güvenilir mi?
- [ ] Script içeriği zararlı komutlar içeriyor mu?
- [ ] DEB kaynağı resmi bir kaynak mı?
- [ ] Git repository'si güvenilir mi?

## SSS

### ❓ Sık Sorulan Sorular

**S: durpkg apt ile aynı mı?**
C: Hayır, durpkg apt depolarında bulunmayan paketleri yüklemek için tasarlanmıştır. apt'yi tamamlayıcı bir araçtır.

**S: Kurulu paketleri nasıl yönetebilirim?**
C: durpkg sadece paket yükleme odaklıdır.Yakında dur adında bir yönetici oluşturacağız. Paket yönetimi için sistem paket yöneticisini kullanın.

**S: Pkgfile hatası alıyorum, ne yapmalıyım?**
C: Pkgfile formatını kontrol edin. Zorunlu alanların (özellikle `name`) dolu olduğundan emin olun.

**S: Kurulum yarıda kesilirse ne olur?**
C: Geçici dosyalar otomatik olarak temizlenir. Tekrar deneyebilirsiniz.

**S: Hangi Linux dağıtımlarında çalışır?**
C: Debian tabanlı tüm dağıtımlarda (Ubuntu, Mint, vb.) çalışır.

### 🔧 Sorun Giderme

**Problem**: `command not found` hatası
**Çözüm**: durpkg'nin PATH'de olduğundan emin olun veya tam yol ile çalıştırın.

**Problem**: `permission denied` hatası
**Çözüm**: durpkg'yi çalıştırma yetkisi verin: `chmod +x durpkg`

**Problem**: Git clone hatası
**Çözüm**: Git'in yüklü olduğundan ve internet bağlantısından emin olun.

**Problem**: DEB yükleme hatası
**Çözüm**: Bağımlılıkları kontrol edin: `sudo apt install -f`

---

## 🤝 Katkıda Bulunma 

### Geliştirme Süreci

1. Repository'yi fork edin
2. Özellik branch'i oluşturun
3. Değişikliklerinizi commit edin
4. Pull request gönderin

### Kod Standartları

- C99 standardına uygun kod yazın
- Güvenlik önlemlerini göz önünde bulundurun
- Kod yorum satırları ekleyin
- Hata durumlarını handle edin

---

## 📄 Lisans

Bu proje GPLv3 lisansı altında yayınlanmıştır.

---

*Sorularınız için issue açabilir veya pull request gönderebilirsiniz.*
