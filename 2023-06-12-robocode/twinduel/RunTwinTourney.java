// Adapted from https://www.cse.chalmers.se/~bergert/robowiki-mirror/RoboWiki/robowiki.net/wiki/Twin_Duel/Tourney_Runner.html
package twinduel;

import robocode.control.*;

import java.io.File;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.Collections;

public class RunTwinTourney {
  public static final String ROBOCODE_PATH = "/opt/robocode";
  static final int NUM_ROUNDS = 10;
  static final int NUM_ITERATIONS = 1;
  static final int BATTLEFIELD_WIDTH = 800;
  static final int BATTLEFIELD_HEIGHT = 600;
  static final String[] competitors = {
    "ana.Nitta*",
    "apadeizvor.ApaDeIzvor*",
    "dummy.Dummy*",
    "Kasparov.Kasparov*",
    //"MateiArmin.MateiArmin*",
    //"megarobo.MegaRobot*",
    //"Mircea.FuriaNoptii.FuriaNoptii*",
    //"mma.TTCO*",
    //"PerformantRobot.PerformantRobot*",
    //"victor.D3XtrBot*",
    //"vr.Robo*",
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

              int score1 = _twinListener.lastResult1.getScore();
              int score2 = _twinListener.lastResult2.getScore();
              System.out.printf("SCORE = %s %d - %s %d\n",
                                botNameFirst, score1, botNameSecond, score2);

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
