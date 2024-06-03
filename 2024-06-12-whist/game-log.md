# Formatul pentru jurnalul unei partide

Nu vor exista linii goale nici comentarii. Ele există mai jos doar pentru clarificări. Vezi și `protocol.md` pentru detalii suplimentare.


# Informații globale

```txt
num_players <int>

# Identificatorii celor num_players jucători (șiruri de caractere fără spații)
player_name <identifier>
...
player_name <identifier>
```

# Informații despre fiecare rundă

```txt
round_id <int>
first_bidder <int>
hand_size <int>

# Mîna fiecărui jucător, în ordinea player_id
hand <card_0> ... <card_{hand_size-1}>
...
hand <card_0> ... <card_{hand_size-1}>

trump <card>

# Contractele licitate, în ordinea player_id
bids <bid_0> ... <bid_{num_players-1}>

# Levatele jucate. Cărțile apar în ordinea în care au fost jucate.
trick <card_0> ... <card_{num_players-1}>
...
trick <card_0> ... <card_{num_players-1}>

# Rezultatul pentru fiecare jucător, 0 = licitație neîndeplinită, 1 = licitație îndeplinită
outcomes <outcome_0> ... <outcome_{num_players-1}>

# Punctele pentru fiecare jucător
points <points_0> ... <points_{num_players-1}>

# Numărul de jocuri cîștigate/pierdute consecutiv, inclusiv runda curentă
# -5 ≤ streak_i ≤ 5, streak_i ≠ 0
streaks <streak_0> ... <streak_{num_players-1}>
```
