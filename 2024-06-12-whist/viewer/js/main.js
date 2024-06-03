$(function() {
  const BOARD_BORDER = 20;
  const MAX_HAND_SIZE = 8;

  let playerAreaStub = null;
  let gameFileLines = [];
  let players = [];
  let rounds = [];
  let frame = null;
  let maxFrames = 0;

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

    getFrameCount() {
      let framesPerTrick = players.length + 1;

      return 1 +          // scena inițială
        players.length +  // licitația
        framesPerTrick * this.handSize;
    }
  }

  init();

  function init() {
    playerAreaStub = $('#player-area-stub').detach();
    $('#file-field').on('change', fileUploaded);
    $('#button-back').on('click', goBack);
    $('#button-forward').on('click', goForward);
    positionDeck();
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

  function positionPlayerArea(area, dir) {
    let boardW = $('#board').width();
    let boardH = $('#board').height();
    let xcenter = boardW / 2;
    let ycenter = boardH / 2;
    let w = area.outerWidth();
    let h = area.outerHeight();
    let cs = Math.cos(dir);
    let sn = Math.sin(dir);
    let xcoef, ycoef;

    if (cs > 0) {
      xcoef = (boardW - BOARD_BORDER - w / 2 - xcenter) / cs;
    } else if (cs < 0) {
      xcoef = (xcenter - BOARD_BORDER - w / 2) / -cs;
    } else {
      xcoef = Infinity;
    }

    if (sn > 0) {
      ycoef = (boardH - BOARD_BORDER - h / 2 - ycenter) / sn;
    } else if (sn < 0) {
      ycoef = (ycenter - BOARD_BORDER - h / 2) / -sn;
    } else {
      ycoef = Infinity;
    }

    let coef = Math.min(xcoef, ycoef);

    area.css({
      top: ycenter - coef * sn - h / 2,
      left: xcenter + coef * cs - w / 2,
    });
  }

  function createPlayerAreas() {
    let dir = -Math.PI;

    players.forEach(function(p) {
      let c = playerAreaStub.clone();
      c.find('.card-header').text(p.name);
      c.appendTo('#board')
      positionPlayerArea(c, dir);
      dir -= 2 * Math.PI / players.length;
    });
  }

  function adjustHeadRow() {
    let head = $('#score-table thead tr');
    head.find('th').slice(players.length + 1).remove();
    while (head.find('th').length < players.length + 1) {
      head.find('th').last().clone().appendTo(head);
    }
  }

  function adjustFirstRow() {
    // Păstrează un singur rînd cu o singură celulă și șterge-i conținutul.
    $('#score-table tbody tr').slice(1).remove();
    $('#score-table tbody tr td').slice(1).remove();
    $('#score-table tbody tr td span').text('');

    // Completează rîndul.
    let row = $('#score-table tbody tr');
    while (row.find('td').length < players.length) {
      row.find('td').last().clone().appendTo(row);
    }
  }

  function createRemainingRows() {
    let body = $('#score-table tbody');
    while (body.find('tr').length < rounds.length) {
      body.find('tr').last().clone().appendTo(body);
    }
  }

  function fillInTableData() {
    let head = $('#score-table thead tr');
    for (let i = 0; i < players.length; i++) {
      head.find('th').eq(i + 1).text(players[i].name);
    }

    let colHeads = $('#score-table tbody tr th');
    for (let i = 0; i < rounds.length; i++) {
      colHeads.eq(i).text(rounds[i].handSize);
    }
  }

  function createScoreTable() {
    adjustHeadRow();
    adjustFirstRow();
    createRemainingRows();
    fillInTableData();
  }

  function computeFrames() {
    frame = 0;
    maxFrames = 1; // scena inițială
    for (let i = 0; i < rounds.length; i++) {
      maxFrames += rounds[i].getFrameCount();
    }
  }

  function initGame(contents) {
    loadGameFile(contents);
    processGameLines();
    sanityCheck();
    createPlayerAreas();
    createScoreTable();
    computeFrames();
  }

  // Vezi https://stackoverflow.com/a/39515846/6022817.
  function fileUploaded(evt) {
    let fl_file = evt.target.files[0];
    let reader = new FileReader();

    let display_file = ( e ) => {
      initGame(e.target.result);
    };

    let on_reader_load = () => {
      return display_file;
    };

    reader.onload = on_reader_load();
    reader.readAsText(fl_file);
  }

  function positionDeck() {
    let boardW = $('#board').width();
    let boardH = $('#board').height();
    let xcenter = boardW / 2;
    let ycenter = boardH / 2;
    let w = $('img.deck').outerWidth();
    let h = $('img.deck').outerHeight();
    let drift = 0;

    $('img.deck').each(function() {
      $(this).css({
        display: 'inline',
        top: ycenter - h / 2,
        left: xcenter - w / 2 + drift,
      });
      drift += 2;
    });
  }

  function getCardImg(card) {
    if (card == -1) {
      return 'img/deck/back.svg';
    } else {
      let f = ('0' + card).slice(-2);
      return 'img/deck/' + f + '.svg';
    }
  }

  function drawPlayerHands(hands) {
    for (let i = 0; i < players.length; i++) {
      let area = $('.player-area .card-holder').eq(i);
      for (let j = 0; j < hands[i].length; j++) {
        area.find('img').eq(j).attr('src', getCardImg(hands[i][j]));
      }
      for (let j = hands[i].length; j < MAX_HAND_SIZE; j++) {
        area.find('img').eq(j).attr('src', '');
      }
    }
  }

  function drawRoundFrame(frame, rid) {
    console.log('drawing frame', frame, 'of round', rid);

    let hands = [...rounds[rid].hands];
    drawPlayerHands(hands);
  }

  function drawTrump(rid) {
    $('img.deck').last().attr('src', getCardImg(rounds[rid].trump));
  }

  function redrawGame() {
    console.log('drawing frame', frame, 'of', maxFrames);
    if (frame == 0) {
      // Player areas are empty
      return;
    }

    let fc = frame - 1;
    let rid = 0;
    while (fc >= rounds[rid].getFrameCount()) {
      fc -= rounds[rid].getFrameCount();
      rid++;
    }

    drawRoundFrame(fc, rid);
    drawTrump(rid);
  }

  function goBack() {
    if (frame > 0) {
      frame--;
      redrawGame();
    }
  }

  function goForward() {
    if (frame < maxFrames - 1) {
      frame++;
      redrawGame();
    }
  }
});
