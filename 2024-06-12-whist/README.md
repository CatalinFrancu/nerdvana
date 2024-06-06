# Arbitru, replayer și exemplu de client pentru jocul de whist

Acest set de unelte vă permite să creați un program de whist și să îl vedeți în acțiune.

## Clientul

Începeți prin a vă scrie propriul client conform specificațiilor din [protocol.md](protocol.md).

În subdirectorul `client` am creat și un exemplu de client, [RandomClient.php](client/RandomClient.php). Este minimalist. Licitează și joacă absolut aleatoriu. Respectă regulile jocului: nu licitează valori ilegale dacă este ultimul la mutare și nu face decît mutări legale.

## Arbitrul

Aceste instrucțiuni sînt pentru Linux. Cred că arbitrul poate fi făcut să ruleze și sub Windows cu [WSL](https://learn.microsoft.com/en-us/windows/wsl/install) și [PHP](https://ubuntu.com/server/docs/how-to-install-and-configure-php) (probabil pachetele `php` și `php-cli`). Dacă cineva reușește să-l facă să meargă, trimiteți-mi și mie detalii, vă rog!

Clonați acest repo:

```bash
git clone https://github.com/CatalinFrancu/nerdvana/
```

Rulați arbitrul, fără argumente ca să vă asigurați că merge:

```bash
cd nerdvana/2024-06-12-whist/arbiter
php arbiter.php
```

Dacă merge, veți vedea un mesaj cu detalii de folosire: `Apel: arbiter.php -b <cale> -n <nume>` etc. etc.

Opțional, încercați să-l rulați cu `RandomClient.php` ca să confirmați că totul funcționează:

```bash
php arbiter.php -b ../client/RandomClient.php -n juc0 -b ../client/RandomClient.php -n juc1 -b ../client/RandomClient.php -n juc2
```

Acum rulați arbitrul cu calea spre clientul vostru. Puteți crea trei instanțe ale aceluiași client, căci este nevoie de minimum trei jucători.

Dacă totul merge, veți vedea gonind mesajele de debug. La final, arbitrul creează fișierul `game.txt` pe care îl puteți folosi ca să vizualizați jocul grafic.

## Replayerul

Deschideți într-un browser fișierul `viewer/index.html`. Folosiți butonul de încărcare (jos) pentru a încărca fișierul `game.txt`. Dacă totul a mers bine, puteți derula înainte și înapoi prin partidă folosind butoanele.

