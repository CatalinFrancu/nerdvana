# Protocol de interacțiune

Programul vostru va fi invocat ori de cîte ori este rîndul lui să mute. El va primi în fișierul `input.txt` situația jocului conform specificațiilor de mai jos. El trebuie să tipărească în `output.txt` un singur întreg, respectiv:

* Pentru faza de licitație: un număr între 0 și mărimea jocului inclusiv.
* Pentru faza de depunere: un număr de carte între 0 și 51 inclusiv.

Apoi, programul trebuie să se încheie cu cod de ieșire 0.

Programul poate să stocheze orice date în directorul curent, în limite rezonabile, să zicem 1 GB. Dacă doriți să-mi trimiteți dinainte niște date precalculate, dați-mi un link sau ceva.

Programul trebuie să se încadreze în timpul de rulare de 1 secundă / mutare și într-o limită de memorie rezonabilă (laptopul meu are 16 GB de RAM).

Dacă programul se termină anormal, se blochează sau mută incorect, arbitrul va face o mutare corectă, aleasă uniform aleatoriu dintre toate mutările corecte la dispoziție. În faza de licitație asta înseamnă că va licita în numele vostru un număr
aleatoriu între 0 și mărimea jocului.

# Fișierul input.txt: Generalități

Fișierul va conține cîte o informație pe fiecare linie, exact în ordinea de mai jos. Fișierul **nu** va conține linii goale nici comentarii. Ele există mai jos doar pentru clarificări.

# Fișierul input.txt: Situația la zi a jocului

```txt
# Numărul de jucători, 3 ≤ num_players ≤ 6.
num_players <int>

# Id-ul tău, 0 ≤ player_id < num_players.
player_id <int>

# Scorurile la zi.
scores <score_0> ... <score_{num_players-1}>

# Seria de jocuri cîștigate / pierdute consecutiv, -4 ≤ streak_i ≤ 4.
streaks <streak_0> ... <streak_{num_players-1-1}>
```

# Fișierul input.txt: Informații despre runda curentă

```txt
# 0 ≤ round_id < numărul de runde
round_id <int>

# Mărimea jocului, 1 ≤ hand_size ≤ 8.
hand_size <int>

# Primul jucător la licitație, 0 ≤ first_bidder < num_players
# Informație redundantă: first_bid = (player_id + round_id) % num_players.
first_bidder <int>

# Ordinea ta în acest joc, 0 ≤ player_order < n.
# Informație redundantă: player_order = (player_id - round_id) % num_players.
player_order <int>

# Atu-ul sau -1 pentru jocurile fără atu.
trump <card>

# Mîna primită
initial_hand <card_0> ... <card_{hand_size-1}>

# Faza jocului, 0 = licitație sau 1 = depunere
phase <int>

# Licitațiile făcute pînă acum, unde -1 = jucătorul încă nu a licitat
bids <bid_0> ... <bid_{num_players-1}
```

# Fișierul input.txt: Informații despre levatele jucate

Numai cînd `phase = 1`:

```txt
# Numărul levatei curente, 0 ≤ trick_id < hand_size
trick_id = <int>
```
Urmează `trick_id+1` înregistrări, toate conținînd cîte `num_players` cards, în afară de ultima (levata curentă), care poate fi incompletă. Levatele sînt date în ordinea în care au fost puse cărțile pe masă, așadar prima carte este a jucătorului care a cîștigat levata anterioară. Ultima levată poate avea `trick_size=0` dacă tu deschizi levata.

```txt
trick = <trick_size> <card_0> ... <card_{num_cards-1}>
...
trick = <trick_size> <card_0> ... <card_{num_cards-1}>
```

# Convenții

* `player_id` este ordinea la primul joc, imutabilă pentru toată partida.
* Toate valorile vectoriale (scores, streaks, bids) sînt date în ordinea `player_id` (**nu** începînd cu tine sau cu primul jucător la runda curentă etc.).
* Cărțile sînt indicate ca valori între 0 și 51 inclusiv, unde:
  *  0...12 = 2 de cupă ... as de cupă;
  * 13...25 = 2 de pică ... as de pică;
  * 26...38 = 2 de romb ... as de romb;
  * 39...51 = 2 de treflă ... as de treflă.
* Mărimile jocurilor sînt următoarele (am încercat să nu fie mai multe jocuri de 1 decît de 8, căci cele de 1 sînt cam la noroc):
  * 3 jucători: 1 1 1, 2 3 4, 5 6 7, 8 8 8, 8 8 8, 7 6 5, 4 3 2, 1 1 1
  * 4 jucători: 1 1 1 1, 2 3 4 5, 6 7 8 8, 8 8 8 8, 8 8 7 6, 5 4 3 2, 1 1 1 1
  * 5 jucători: 1 1 1 1 1, 2 3 4 5 6, 7 8 8 8 8, 8 8 8 8 7, 6 5 4 3 2, 1 1 1 1 1
  * 6 jucători: 1 1 1 1 1 1, 2 3 4 5 6 7, 8 8 8 8 8 8, 8 8 8 8 8 8, 7 6 5 4 3 2, 1 1 1 1 1 1
