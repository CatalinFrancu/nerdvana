package twinduel;

import robocode.control.*;

import java.io.File;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Collections;

public class RunTwinTourney {
  public static final String ROBOCODE_PATH = "/PATH/TO/ROBOCODE";
  static final int NUM_ROUNDS = 75;
  static final int NUM_ITERATIONS = 1;
  static final int BATTLEFIELD_WIDTH = 800;
  static final int BATTLEFIELD_HEIGHT = 800;
  static final String[] competitors = {
    "ags.lunartwins.LunarTwins 1.2",
    "bvh.two.Ravens 0.2",
    "bvh.two.Valkiries 0.44t_mk3",
    "chase.twin.SurroundingTeam 1.0",
    "davidalves.twin.YinYang 1.2",
    "gh.GruwelTwins 0.1",
    "gh.twin.GrauwuarG 0.41",
    "jk.team.NightAndDay 1.6",
    "kawigi.micro.ShizPair 1.1",
    "kawigi.twin.MarioBros 1.0",
    "kc.twins.GeminiTeam 2.1",
    "kinsen.twin.SelcouthTeam 1.1",
    "krillr.mini.JointStrikeForce 2.0",
    "voidious.team.LuminariousDuo 1.0591",
    "wcsv.Coyote.CoyotePair .1",
    "whind.TwintelligenceTeam 1.0",
    "wiki.twin.InevitableTeam 0.1",
    "wiki.twin.KomariousTeam 1.0"

  };

  static final boolean WAIT_UNTIL_OVER = true;

  private TwinListener _twinListener;
  private BattlefieldSpecification _battlefield;
  private RobocodeEngine _roboEngine;

  public RunTwinTourney() {
    _twinListener = new TwinListener();
    _roboEngine = new RobocodeEngine(new File(ROBOCODE_PATH));
    _roboEngine.addBattleListener(_twinListener);
    _roboEngine.setVisible(false);
    _battlefield =
      new BattlefieldSpecification(BATTLEFIELD_WIDTH, BATTLEFIELD_HEIGHT);
  }

  public void runTwinDuel() {
    ArrayList<CompetitorData> competitorList = processRoundRobin();
    ArrayList<CompetitorData> sortedCompetitorList =
      (ArrayList<CompetitorData>)competitorList.clone();
    Collections.sort(sortedCompetitorList, new CompetitorSort());

    System.out.println("----");
    System.out.println();
    System.out.println("Round Robin Results: ");
    printCompetitorStats(sortedCompetitorList);

    processBracketTourney(sortedCompetitorList);
  }

  public ArrayList<CompetitorData> processRoundRobin() {
    HashMap<String, CompetitorData> competitorHash =
      new HashMap<String, CompetitorData>();

    for (int z = 0; z < NUM_ITERATIONS; z++) {
      System.out.println("Round Robin Iteration " + (z + 1) + "...");
      for (int x = 0; x < competitors.length; x++) {
        for (int y = 0; y < competitors.length; y++) {
          if (x < y) {
            //                    if (x == 0 && x != y) {
            String botNameFirst = "", botNameSecond = "";
            int botSurvivalFirst = -1, botSurvivalSecond = -1;
            do {
              BattleSpecification battleSpec =
                new BattleSpecification(NUM_ROUNDS,
                                        _battlefield,
                                        _roboEngine.getLocalRepository(
                                                                       competitors[x] + "," + competitors[y]));
              _twinListener.lastBattleErrored = false;
              _roboEngine.runBattle(battleSpec, WAIT_UNTIL_OVER);

              botNameFirst =
                _twinListener.lastResult1.getRobot().getTeamId();
              botNameFirst =
                botNameFirst.replaceFirst("\\[.*\\]", "");
              botSurvivalFirst =
                _twinListener.lastResult1.getFirsts();
              botNameSecond =
                _twinListener.lastResult2.getRobot().getTeamId();
              botNameSecond =
                botNameSecond.replaceFirst("\\[.*\\]", "");
              botSurvivalSecond =
                _twinListener.lastResult2.getFirsts();
              if (botSurvivalFirst == botSurvivalSecond) {
                System.out.println(botNameFirst + " tied with " +
                                   botNameSecond + ": " + botSurvivalFirst +
                                   " rounds won each. Rerunning...");
              }
              if (_twinListener.lastBattleErrored) {
                System.out.println("Encountered a Robocode " +
                                   "error. Re-initializing Robocode engine " +
                                   "and rerunning...");
                _roboEngine =
                  new RobocodeEngine(new File(ROBOCODE_PATH));
                _roboEngine.addBattleListener(_twinListener);
              }
            } while (botSurvivalFirst == botSurvivalSecond ||
                     _twinListener.lastBattleErrored);

            String winnerName, loserName;
            int winnerSurvival, loserSurvival;
            if (botSurvivalFirst > botSurvivalSecond) {
              winnerName = botNameFirst;
              winnerSurvival = botSurvivalFirst;
              loserName = botNameSecond;
              loserSurvival = botSurvivalSecond;
            } else {
              loserName = botNameFirst;
              loserSurvival = botSurvivalFirst;
              winnerName = botNameSecond;
              winnerSurvival = botSurvivalSecond;
            }

            CompetitorData winnerData, loserData;
            if (competitorHash.containsKey(winnerName)) {
              winnerData = competitorHash.get(winnerName);
            } else {
              winnerData = new CompetitorData();
              winnerData.name = winnerName;
              competitorHash.put(winnerName, winnerData);
            }
            winnerData.matchesWon++;
            winnerData.roundsWon += winnerSurvival;
            winnerData.roundsTotal += NUM_ROUNDS;

            if (competitorHash.containsKey(loserName)) {
              loserData = competitorHash.get(loserName);
            } else {
              loserData = new CompetitorData();
              loserData.name = loserName;
              competitorHash.put(loserName, loserData);
            }
            loserData.matchesLost++;
            loserData.roundsWon += loserSurvival;
            loserData.roundsTotal += NUM_ROUNDS;

            System.out.println("RESULT = "+ winnerName +" defeats "
                               + loserName + ": " + winnerSurvival + " to " +
                               loserSurvival);
          }
        }
      }
      System.out.println();
    }

    return new ArrayList<CompetitorData>(competitorHash.values());
  }

  public void processBracketTourney(ArrayList<CompetitorData> competitorList) {
    for (int x = 0; x < competitorList.size(); x++) {
      CompetitorData competitor = competitorList.get(x);
      competitor.tourneySeed = (x + 1);
    }

    int rounds = 1, slots = 2;

    while (slots < competitorList.size()) {
      rounds++;
      slots *= 2;
    }

    for (int x = 0; x < rounds; x++) {
      System.out.println();
      System.out.println("----");
      System.out.println();
      if (x < rounds - 1) {
        System.out.println("Bracket Tourney Round " + (x + 1));
      } else {
        System.out.println("Tourney Finals");
      }
      System.out.println();

      boolean needExtraNewline = false;

      for (int y = 0; y < (slots / 2); y++) {
        int effectiveSeed1 = (y + 1);
        int effectiveSeed2 = slots - y;

        if (competitorList.size() < effectiveSeed2) {
          System.out.println("Seed " + effectiveSeed1
                             + " gets a bye");
          needExtraNewline = true;
        } else {
          if (needExtraNewline) {
            System.out.println();
            needExtraNewline = false;
          }

          CompetitorData higherSeed =
            competitorList.get(effectiveSeed1 - 1);
          CompetitorData lowerSeed =
            competitorList.get(effectiveSeed2 - 1);

          System.out.println(
                             higherSeed.tourneySeed + " " + higherSeed.name);
          System.out.println(
                             lowerSeed.tourneySeed + " " + lowerSeed.name);

          String tourneyResultString = tourneyBattleResult(
                                                           higherSeed, lowerSeed, (slots == 2));
          System.out.println("\t" + tourneyResultString);

          if (!tourneyResultString.contains(
                                            higherSeed.name + " wins")) {
            competitorList.set(y, lowerSeed);
          }

          needExtraNewline = true;
        }
      }

      slots /= 2;
    }

    System.out.println();
  }

  public static void printCompetitorStats(
                                          ArrayList<CompetitorData> competitorStats) {

    for (int x = 0; x < competitorStats.size(); x++) {
      System.out.println();
      CompetitorData stats = competitorStats.get(x);
      System.out.println("Competitor: " + stats.name);
      System.out.println("Win/loss: " + stats.matchesWon + " - " + stats.matchesLost);
      System.out.println("Rounds won: " + stats.roundsWon +
                         " (" + stats.roundWinPercentage() + "%)");
    }
  }

  public String tourneyBattleResult(CompetitorData firstBot,
                                    CompetitorData secondBot, boolean tourneyFinals) {

    String botNameFirst = "", botNameSecond = "";
    int botSurvivalFirst = -1, botSurvivalSecond = -1;
    int battlesFought = 0;

    TwinBattleResult tourneyBattleResult = new TwinBattleResult();
    tourneyBattleResult.firstBotName = firstBot.name;
    tourneyBattleResult.secondBotName = secondBot.name;

    do {
      BattleSpecification battleSpec =
        new BattleSpecification(NUM_ROUNDS, _battlefield,
                                _roboEngine.getLocalRepository(firstBot.name +
                                                               "," + secondBot.name));
      _twinListener.lastBattleErrored = false;
      _roboEngine.runBattle(battleSpec, WAIT_UNTIL_OVER);

      if (_twinListener.lastBattleErrored) {
        System.out.println("Encountered a Robocode error. " +
                           "Re-initializing Robocode engine and rerunning...");
        _roboEngine = new RobocodeEngine(new File(ROBOCODE_PATH));
        _roboEngine.addBattleListener(_twinListener);
      } else {
        botNameFirst = _twinListener.lastResult1.getRobot().getTeamId();
        botNameFirst = botNameFirst.replaceFirst("\\[.*\\]", "");
        botSurvivalFirst = _twinListener.lastResult1.getFirsts();
        botNameSecond = _twinListener.lastResult2.getRobot().getTeamId();
        botNameSecond = botNameSecond.replaceFirst("\\[.*\\]", "");
        botSurvivalSecond = _twinListener.lastResult2.getFirsts();

        battlesFought++;
        if (firstBot.name.equals(botNameFirst)) {
          tourneyBattleResult.firstSurvival.add(
                                                new Integer(botSurvivalFirst));
          tourneyBattleResult.secondSurvival.add(
                                                 new Integer(botSurvivalSecond));
        } else {
          tourneyBattleResult.secondSurvival.add(
                                                 new Integer(botSurvivalFirst));
          tourneyBattleResult.firstSurvival.add(
                                                new Integer(botSurvivalSecond));
        }
        // For debugging suspected problems...
        //                System.out.println("RAW TOURNEY RESULT: " + botNameFirst +
        //                    " = " + botSurvivalFirst + ", " + botNameSecond + " = " +
        //                    botSurvivalSecond);
      }
    } while ((tourneyFinals && battlesFought < 3) ||
             (!tourneyFinals && botSurvivalFirst == botSurvivalSecond) ||
             _twinListener.lastBattleErrored);

    return tourneyBattleResult.winString();
  }

  class CompetitorData {
    String name;
    long matchesWon = 0;
    long matchesLost = 0;
    long roundsWon = 0;
    long roundsTotal = 0;
    int tourneySeed = -1;

    public double roundWinPercentage() {
      return (
              ((double)Math.round((((double)roundsWon)/roundsTotal)*10000))
              / 100);
    }
  }

  class CompetitorSort implements Comparator<CompetitorData> {
    public int compare(CompetitorData c1, CompetitorData c2) {
      if (c1.matchesWon - c1.matchesLost <
          c2.matchesWon - c2.matchesLost) {
        return 1;
      } else if (c1.matchesWon - c1.matchesLost >
                 c2.matchesWon - c2.matchesLost) {
        return -1;
      } else {
        if (c1.roundWinPercentage() < c2.roundWinPercentage()) {
          return 1;
        } else if (c1.roundWinPercentage() > c2.roundWinPercentage()) {
          return -1;
        } else {
          return 0;
        }
      }
    }

    public boolean equals(Object obj) {
      return (this == obj);
    }
  }

  class TwinBattleResult {
    String firstBotName, secondBotName;
    ArrayList<Integer> firstSurvival;
    ArrayList<Integer> secondSurvival;

    public TwinBattleResult() {
      firstSurvival = new ArrayList<Integer>();
      secondSurvival = new ArrayList<Integer>();
    }

    public String winString() {
      int firstWins = 0, secondWins = 0;

      for (int x = 0; x < firstSurvival.size(); x++) {
        if (((Integer)firstSurvival.get(x)).intValue()
            > ((Integer)secondSurvival.get(x)).intValue()) {
          firstWins++;
        } else if (((Integer)firstSurvival.get(x)).intValue()
                   < ((Integer)secondSurvival.get(x)).intValue()) {
          secondWins++;
        }
      }

      String winString = "";

      if (firstWins > secondWins) {
        winString += firstBotName + " wins ";
      } else if (firstWins < secondWins) {
        winString += secondBotName + " wins ";
      } else {
        winString += firstBotName + " tied with " + secondBotName
          + " ";
      }

      for (int x = 0; x < firstSurvival.size(); x++) {
        if (x != 0) {
          winString += ", ";
        }

        if (firstWins >= secondWins) {
          winString += firstSurvival.get(x) + " - "
            + secondSurvival.get(x);
        } else {
          winString += secondSurvival.get(x) + " - "
            + firstSurvival.get(x);
        }
      }

      return winString;
    }
  }

  public static void main(String[] args) {
    RunTwinTourney rtt = new RunTwinTourney();
    rtt.runTwinDuel();
  }
}
