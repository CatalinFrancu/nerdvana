$(function() {
  const MAX_HAND_SIZE = 8;
  const CARD_NONE = -2;
  const CARD_BACK = -1;

  const COLOR_CLASSES = [
    'text-bg-light',
    'text-bg-success',
    'text-bg-danger',
  ];

  const GEOMETRY = {
    3: {
      areas: [
        'h-left v-center',
        'h-right v-top',
        'h-right v-bottom',
      ],
      drops: [
        'h-right-of-left v-center',
        'h-center-of-right v-below-top',
        'h-center-of-right v-above-bottom',
      ],
    },
    4: {
      areas: [
        'h-left v-center',
        'h-center v-top',
        'h-right v-center',
        'h-center v-bottom',
      ],
      drops: [
        'h-right-of-left v-center',
        'h-center v-below-top',
        'h-left-of-right v-center',
        'h-center v-above-bottom',
      ],
    },
    5: {
      areas: [
        'h-left v-center',
        'h-center-left v-top',
        'h-center-right v-top',
        'h-right v-center',
        'h-center v-bottom',
      ],
      drops: [
        'h-right-of-left v-center',
        'h-center-left v-below-top',
        'h-center-right v-below-top',
        'h-left-of-right v-center',
        'h-center v-above-bottom',
      ],
    },
    6: {
      areas: [
        'h-left v-center',
        'h-center-left v-top',
        'h-center-right v-top',
        'h-right v-center',
        'h-center-right v-bottom',
        'h-center-left v-bottom',
      ],
      drops: [
        'h-right-of-left v-center',
        'h-center-left v-below-top',
        'h-center-right v-below-top',
        'h-left-of-right v-center',
        'h-center-right v-above-bottom',
        'h-center-left v-above-bottom',
      ],
    },
  };

  let playerAreaStub = null;
  let dropAreaStub = null;
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

  // up = cărțile din mînă; down = cartea jucată pe masă
  class PlayerState {
    constructor(hand, bid) {
      this.up = hand;
      this.down = CARD_NONE;
      this.taken = 0;
      this.bid = bid;
    }

    removeCard(card) {
      let index = this.up.indexOf(card);
      if (index > -1) {
        this.up.splice(index, 1);
      } else {
        assert(false, 'Cartea de jucat nu există în mînă.');
      }
    }

    playCard(card) {
      this.removeCard(card);
      this.down = card;
    }

    takeTrick() {
      this.taken++;
    }
  }

  class Round {
    constructor(id) {
      this.id = id;
      this.hands = [];
      this.trump = null;
      this.bids = [];
      this.tricks = [];
      this.trickWinners = [];
      this.outcomes = [];
      this.points = [];
      this.scores = [];
      this.streaks = [];
      this.startFrame = 0;
    }

    getFrameCount() {
      let framesPerTrick = players.length + 1;

      return 1 +          // scena inițială
        players.length +  // licitația
        framesPerTrick * this.handSize;
    }

    getBidClass(player) {
      let s = this.streaks[player];
      return (s < 0) ? 'text-bg-danger' : 'text-bg-success';
    }

    getFirstPlayerForTrick(t) {
      return t ? this.trickWinners[t - 1] : (this.id % players.length);
    }

    playTrick(t, states) {
      let current = this.getFirstPlayerForTrick(t);
      for (let i = 0; i < players.length; i++) {
        states[current].removeCard(this.tricks[t][i]);
        current = (current + 1) % players.length;
      }
      states[this.trickWinners[t]].takeTrick();
    }

    collectTrick(winner, states) {
      for (let i = 0; i < players.length; i++) {
        states[i].down = CARD_NONE;
      }
      states[winner].takeTrick();
    }

    getStatesAtFrame(frame) {
      let states = [];
      for (let i = 0; i < players.length; i++) {
        states.push(new PlayerState([...this.hands[i]], this.bids[i]));
      }

      if (frame >= players.length + 1) {
        // sari peste licitații
        frame -= players.length + 1;
        let trick = 0;

        while (frame >= players.length + 1) {
          this.playTrick(trick++, states);
          frame -= players.length + 1;
        }

        let current = this.getFirstPlayerForTrick(trick);
        for (let i = 0; i <= Math.min(frame, players.length - 1); i++) {
          states[current].playCard(this.tricks[trick][i]);
          current = (current + 1) % players.length;
        }

        if (frame == players.length) {
          this.collectTrick(this.trickWinners[trick], states);
        }
      }

      return states;
    }
  }

  init();

  function init() {
    playerAreaStub = $('#player-area-stub').detach();
    dropAreaStub = $('#drop-area-stub').detach();
    $('#file-field').on('change', fileUploaded);
    $('#button-first').on('click', goFirst);
    $('#button-back').on('click', goBack);
    $('#button-forward').on('click', goForward);
    $('#button-last').on('click', goLast);
    $('#score-table tbody').on('click', 'tr', goToRound);
    $(document).on('keydown', keyHandler);
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
          rounds.push(new Round(val));
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
  }

  function computeTotalScores() {
    for (let p = 0; p < players.length; p++) {
      let total = 0;
      for (let r = 0; r < rounds.length; r++) {
        total += rounds[r].points[p];
        rounds[r].scores.push(total);
      }
    }
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

  function createPlayerAreas() {
    players.forEach(function(p, ind) {
      let a = playerAreaStub.clone();
      a.find('.card-header').text(p.name);
      a.appendTo('#board')
      a.addClass(GEOMETRY[players.length].areas[ind]);

      let d = dropAreaStub.clone();
      d.appendTo('#board')
      d.addClass(GEOMETRY[players.length].drops[ind]);
    });
  }

  function adjustHeaderRow(sel) {
    let head = $(sel);
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
    let footer = $('#score-table tfoot tr');
    for (let i = 0; i < players.length; i++) {
      head.find('th').eq(i + 1).text(players[i].name);
      footer.find('th').eq(i + 1).text(players[i].name);
    }

    let colHeads = $('#score-table tbody tr th');
    for (let i = 0; i < rounds.length; i++) {
      colHeads.eq(i).text(rounds[i].handSize);
    }
  }

  function drawGroupBorders() {
    let sel = '#score-table tbody tr:nth-child(' + players.length + 'n)';
    $(sel).css({ borderBottomColor: 'black'});
  }

  function createScoreTable() {
    adjustHeaderRow('#score-table thead tr');
    adjustHeaderRow('#score-table tfoot tr');
    adjustFirstRow();
    createRemainingRows();
    fillInTableData();
    drawGroupBorders();
  }

  function computeFrames() {
    let frame = 0;
    maxFrames = 1; // scena inițială
    for (let i = 0; i < rounds.length; i++) {
      rounds[i].startFrame = maxFrames;
      maxFrames += rounds[i].getFrameCount();
    }
  }

  function showGame() {
    $('#game-area').css({display: 'flex'});
    $('#file-loader').hide();
  }

  function initGame(contents) {
    loadGameFile(contents);
    processGameLines();
    computeTotalScores();
    sanityCheck();
    createPlayerAreas();
    createScoreTable();
    computeFrames();
    showGame();
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

    $('.deck img').each(function() {
      $(this).css({
        display: 'inline',
        top: ycenter - h / 2,
        left: xcenter - w / 2 + drift,
      });
      drift += 2;
    });
  }

  function getCardImg(card) {
    if (card == CARD_NONE) {
      return '';
    } else if (card == CARD_BACK) {
      return 'img/deck/back.svg';
    } else {
      let f = ('0' + card).slice(-2);
      return 'img/deck/' + f + '.svg';
    }
  }

  function eraseFrame() {
    $('#score-table tbody td span').text('');
  }

  function drawTrump(rid) {
    $('.deck img').last().attr('src', getCardImg(rounds[rid].trump));
  }

  function setBidClass(elt, cssClass) {
    COLOR_CLASSES.forEach(function(c) {
      elt.removeClass(c);
    });
    elt.addClass(cssClass);
  }

  function drawRoundBids(rid, num_bids) {
    let first_bidder = rid % players.length;
    for (let i = 0; i < num_bids; i++) {
      let bidder = (i + first_bidder) % players.length;
      let row = $('#score-table tbody tr').eq(rid);
      let cell = row.find('td').eq(bidder);
      cell.find('span.bid').text(rounds[rid].bids[bidder]);
      setBidClass(cell.find('span.bid'), 'text-bg-light');
    }
  }

  function drawRoundScores(rid) {
    for (let i = 0; i < players.length; i++) {
      let row = $('#score-table tbody tr').eq(rid);
      let cell = row.find('td').eq(i);
      cell.find('span.score').text(rounds[rid].scores[i]);

      let cssClass = rounds[rid].getBidClass(i);
      setBidClass(cell.find('span.bid'), cssClass);
    }
  }

  function drawPlayerStates(states) {
    for (let i = 0; i < players.length; i++) {
      let area = $('.player-area').eq(i);
      area.find('.taken').text(states[i].taken);
      area.find('.bid').text(states[i].bid);
      for (let j = 0; j < states[i].up.length; j++) {
        area.find('.card-holder img').eq(j).attr('src', getCardImg(states[i].up[j]));
      }
      for (let j = states[i].up.length; j < MAX_HAND_SIZE; j++) {
        area.find('.card-holder img').eq(j).attr('src', '');
      }

      let dropArea = $('.drop-area').eq(i);
      dropArea.attr('src', getCardImg(states[i].down));

    }
  }

  function drawRoundFrame(frame, rid) {
    drawTrump(rid);

    let num_bids = Math.min(frame, players.length);
    drawRoundBids(rid, num_bids);

    let states = rounds[rid].getStatesAtFrame(frame);
    drawPlayerStates(states);

    if (frame == rounds[rid].getFrameCount() - 1) {
      drawRoundScores(rid);
    }
  }

  function redrawGame() {
    eraseFrame();
    if (frame == 0) {
      // Player areas are empty
      return;
    }

    let fc = frame - 1;
    let rid = 0;

    while (fc >= rounds[rid].getFrameCount()) {
      drawRoundBids(rid, players.length);
      drawRoundScores(rid);
      fc -= rounds[rid].getFrameCount();
      rid++;
    }

    drawRoundFrame(fc, rid);
  }

  function goFirst() {
    frame = 0;
    redrawGame();
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

  function goLast() {
    frame = maxFrames - 1;
    redrawGame();
  }

  function goToRound() {
    let i = $(this).index();
    frame = rounds[i].startFrame;
    redrawGame();
  }

  function goToNextRound() {
    let i = 0;
    while ((i < rounds.length) && (rounds[i].startFrame <= frame)) {
      i++;
    }
    if (i < rounds.length) {
      frame = rounds[i].startFrame;
      redrawGame();
    }
  }

  function goToPrevRound() {
    let i = rounds.length - 1;
    while ((i >= 0) && (rounds[i].startFrame >= frame)) {
      i--
    }
    if (i >= 0) {
      frame = rounds[i].startFrame;
      redrawGame();
    }
  }

  function keyHandler(e) {
    let captured = true;
    switch (e.which) {
      case 37: goBack(); break;        // left arrow
      case 39: goForward(); break;     // right arrow
      case 38: goToPrevRound(); break; // up arrow
      case 40: goToNextRound(); break; // down arrow
      case 36: goFirst(); break;       // home arrow
      case 35: goLast(); break;        // end arrow
      default: captured = false;
    }
    if (captured) {
      // let other keys work as expected (enter, tab etc.)
      e.preventDefault();
    }
  }
});
