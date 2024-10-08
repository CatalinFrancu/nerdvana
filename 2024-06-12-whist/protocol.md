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
streaks <streak_0> ... <streak_{num_players-1}>
```

# Fișierul input.txt: Informații despre runda curentă

```txt
# 0 ≤ round_id < numărul de runde
round_id <int>

# Mărimea jocului, 1 ≤ hand_size ≤ 8.
hand_size <int>

# Primul jucător la licitație, 0 ≤ first_bidder < num_players
# Informație redundantă: first_bidder = round_id % num_players.
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
bids <bid_0> ... <bid_{num_players-1}>
```

# Fișierul input.txt: Informații despre levatele jucate

Numai cînd `phase = 1`:

```txt
# Numărul levatei curente, 0 ≤ trick_id < hand_size
trick_id <int>
```

Urmează `trick_id+1` înregistrări, toate conținînd cîte `num_players` cards, în afară de ultima (levata curentă), care poate fi incompletă. Levatele sînt date în ordinea în care au fost puse cărțile pe masă, așadar prima carte este a jucătorului care a cîștigat levata anterioară. Ultima levată poate avea `trick_size=0` dacă tu deschizi levata. După fiecare levată cu excepția ultimei, `winner` indică ID-ul jucătorului care a cîștigat levata. Desigur, informația `winner` este redundantă, căci poate fi dedusă din `trick`, dar arbitrul v-o oferă în caz că vă ajută.

```txt
trick <trick_size> <card_0> ... <card_{num_cards-1}>
winner <int>
trick <trick_size> <card_0> ... <card_{num_cards-1}>
winner <int>
...
trick <trick_size> <card_0> ... <card_{num_cards-1}>
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

# Exemple

O partidă de trei persoane, un joc de 8 cărți, levata a 3-a. Jucătorul #0 a deschis cu K♣, iar acum este rîndul jucătorului #1, care poate răspunde cu A♣ sau cu Q♣.

```txt
num_players 3
player_id 1
scores 10 20 30
streaks -2 -1 -1
round_id 11
hand_size 8
first_bidder 2
player_order 2
trump -1
initial_hand 12 37 20 51 49 8 10 7
phase 1
bids 6 1 6
trick_id 2
trick 3 38 33 37
winner 2
trick 3 23 24 20
winner 0
trick 1 50
```

O partidă de 4 persoane, un joc de 5 cărți, levata a 3-a. Atuul este 9♠, jucătorul #1 a deschis cu Q♦, iar acum este rîndul jucătorului #2, care poate răspunde cu K♦ sau cu 8♦.

```txt
num_players 4
player_id 2
scores 0 0 0 0
streaks -2 -2 -3 2
round_id 7
hand_size 5
first_bidder 3
player_order 3
trump 20
initial_hand 7 37 19 18 32
phase 1
bids 0 2 4 2
trick_id 2
trick 4 44 45 24 19
winner 1
trick 4 12 7 5 10
winner 1
trick 1 36
```
