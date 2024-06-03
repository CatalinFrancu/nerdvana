function assert(x, msg) {
  console.assert(x, msg);
}

class GameLine {
  constructor(line) {
    let parts = line.split(' ');
    this.key = parts[0];
    this.value = parts.slice(1);
  }

  getJoinedValue() {
    return this.value.join(' ');
  }

  getNumber() {
    return Number(this.value[0]);
  }

  getNumbers() {
    let result = [];
    for (let i = 0; i < this.value.length; i++) {
      result.push(Number(this.value[i]));
    }
    return result;
  }
}

class Player {
  constructor(name) {
    this.name = name;
  }
}

class Round {
  constructor() {
    this.hands = [];
    this.trump = null;
    this.bids = [];
    this.tricks = [];
    this.trickWinners = [];
    this.outcomes = [];
    this.points = [];
    this.streaks = [];
  }
}

$(function() {
  let gameFileLines = [];
  let players = [];
  let rounds = [];

  init();

  function init() {
    $('#file-field').on('change', fileUploaded);
  }

  function loadGameFile(contents) {
    gameFileLines = [];
    lines = contents.split('\n');
    lines.forEach(function(line) {
      if ((line != '') && (line[0] != '#')) {
        gameFileLines.push(new GameLine(line));
      }
    });
  }

  function processGameLines() {
    let num_players = null;
    gameFileLines.forEach(function(line) {
      let val = line.getNumber();

      switch (line.key) {
        case 'num_players':
          num_players = Number(line.value);
          break;

        case 'player_name':
          players.push(new Player(line.getJoinedValue()));
          break;

        case 'round_id':
          assert(rounds.length == val, 'ID-ul rundei nu crește secvențial');
          rounds.push(new Round());
          break;

        case 'first_bidder':
          assert(val == (rounds.length - 1) % players.length, 'first_bidder incorect');
          break;

        case 'hand_size':
          rounds[rounds.length - 1].handSize = val;
          break;

        case 'hand':
          rounds[rounds.length - 1].hands.push(line.getNumbers());
          break;

        case 'trump':
          rounds[rounds.length - 1].trump = val;
          break;

        case 'bids':
          rounds[rounds.length - 1].bids = line.getNumbers();
          break;

        case 'trick':
          rounds[rounds.length - 1].tricks.push(line.getNumbers());
          break;

        case 'trick_winner':
          rounds[rounds.length - 1].trickWinners.push(val);
          break;

        case 'outcomes':
          rounds[rounds.length - 1].outcomes = line.getNumbers();
          break;

        case 'points':
          rounds[rounds.length - 1].points = line.getNumbers();
          break;

        case 'streaks':
          rounds[rounds.length - 1].streaks = line.getNumbers();
          break;

        default:
          assert(false, 'nume de variabilă necunoscut');
      }
    });

    assert(num_players == players.length, 'num_players nu corespunde cu numele');
    console.log(rounds);
  }

  function sanityCheck() {
    rounds.forEach(function (r) {
      assert(r.hands.length == players.length, 'Numărul de mîini diferă de numărul de jucători.');
      r.hands.forEach(function (h) {
        assert(h.length == r.handSize, 'Numărul de cărți diferă de mărimea rundei.');
      });
      assert(r.trump !== null, 'Runda are atuul nespecificat.');
      assert(r.bids.length == players.length, 'Numărul de licitații este incorect.');
      assert(r.tricks.length == r.handSize, 'Numărul de levate este incorect.');
      assert(r.trickWinners.length == r.handSize, 'Numărul de cîștigători este incorect.');
      r.tricks.forEach(function (t) {
        assert(t.length == players.length, 'Mărimea levatei este incorectă.');
      });
      assert(r.points.length == players.length, 'Lungimea vectorului de puncte este incorectă.');
      assert(r.streaks.length == players.length, 'Numărul de streaks este incorect.');
    });
  }

  function initGame(contents) {
    loadGameFile(contents);
    processGameLines();
    sanityCheck();
  }

  // See https://stackoverflow.com/a/39515846/6022817.
  function fileUploaded(evt) {
    let fl_file = evt.target.files[0];
    let reader = new FileReader(); // built in API

    let display_file = ( e ) => {
      initGame(e.target.result);
    };

    let on_reader_load = () => {
      return display_file; // a function
    };

    reader.onload = on_reader_load();
    reader.readAsText(fl_file);
  }
});
