# Arbitru, replayer și exemplu de client pentru jocul de whist

Acest set de unelte vă permite să creați un program de whist și să îl vedeți în acțiune.

## Clientul

Începeți prin a vă scrie propriul client conform specificațiilor din [protocol.md](protocol.md).

În subdirectorul `client` am creat și un exemplu de client, [RandomClient.php](client/random/RandomClient.php). Este minimalist. Licitează și joacă corect, dar între variantele legale alege absolut aleatoriu. 

## Arbitrul

### Pași specifici pentru Windows

Instalați [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) și Ubuntu 24.04. Din terminal, rulați:

```bash
wsl --install Ubuntu 24.04
```

Va cere reboot, apoi va finaliza instalarea. Alegeți un nume de utilizator și o parolă. Apoi vă veți afla într-un prompt de Linux.

Aduceți la zi sistemul și instalați PHP:

```bash
sudo apt update
sudo apt upgrade
sudo apt install php
```

Testați că PHP merge:

```bash
php --version
```

Puteți vedea sistemul de fișiere Windows din Linux:

```bash
ls /mnt/c/
```

De asemenea, puteți vedea sistemul de fișiere Linux din Windows. Din File Explorer, navigați la Linux > Ubuntu-24.04 > /home/<username>/nerdvana etc.


### Pași comuni pentru Linux și Windows

Clonați acest repo:

```bash
git clone https://github.com/CatalinFrancu/nerdvana/
```

Pe viitor, avînd în vedere că eu continui să lucrez la cod, puteți obține ultima versiune a codului cu comanda `git pull` executată din interiorul repo-ului:

```bash
cd nerdvana
git pull
```

Rulați arbitrul, fără argumente ca să vă asigurați că merge:

```bash
cd nerdvana/2024-06-12-whist/arbiter
php arbiter.php
```

Dacă merge, veți vedea un mesaj cu detalii de folosire: `Apel: arbiter.php -b <cale> -n <nume>` etc. etc.

Opțional, încercați să-l rulați cu `RandomClient.php` ca să confirmați că totul funcționează:

```bash
php arbiter.php -b ../client/random/RandomClient.php -n juc0 -b ../client/random/RandomClient.php -n juc1 -b ../client/random/RandomClient.php -n juc2
```

Acum rulați arbitrul cu calea spre clientul vostru. Puteți crea trei instanțe ale aceluiași client, căci este nevoie de minimum trei jucători.

Dacă totul merge, veți vedea gonind mesajele de debug. La final, arbitrul creează fișierul `game.txt` pe care îl puteți folosi ca să vizualizați jocul grafic.

### Opțiuni de configurare pentru arbitru

Puteți modifica valorile constantelor din `Config.php`. Fiecare constantă este documentată. De exemplu, puteți reduce nivelul de zgomot reducînd valoarea lui `LOG_LEVEL`.

## Replayerul

Deschideți într-un browser fișierul `viewer/index.html`. Folosiți butonul de încărcare (jos) pentru a încărca fișierul `game.txt`. Dacă totul a mers bine, puteți derula înainte și înapoi prin partidă folosind butoanele.

