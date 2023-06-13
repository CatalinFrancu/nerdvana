// Adapted from https://www.cse.chalmers.se/~bergert/robowiki-mirror/RoboWiki/robowiki.net/wiki/Twin_Duel/Tourney_Runner.html
package roundrobin;

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
    "MateiArmin.MateiArmin*",
    "megarobo.MegaRobot*",
    "Mircea.FuriaNoptii.FuriaNoptii*",
    "mma.TTCO*",
    "PerformantRobot.PerformantRobot*",
    "victor.D3XtrBot*",
    "vr.Robo*",
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
    System.out.println();
    printCompetitorStats(sortedCompetitorList);
  }

  public ArrayList<CompetitorData> processRoundRobin() {
    HashMap<String, CompetitorData> competitorHash =
      new HashMap<String, CompetitorData>();

    for (int z = 0; z < NUM_ITERATIONS; z++) {
      System.out.println("Round Robin Iteration " + (z + 1) + "...");
      for (int x = 0; x < competitors.length - 1; x++) {
        for (int y = x + 1; y < competitors.length; y++) {
          String botName1 = competitors[x];
          String botName2 = competitors[y];
          String joined = botName1 + ',' + botName2;
          BattleSpecification battleSpec =
          new BattleSpecification(NUM_ROUNDS,
                                    _battlefield,
                                  _roboEngine.getLocalRepository(joined));
          _roboEngine.runBattle(battleSpec, WAIT_UNTIL_OVER);


          int score1 = _twinListener.lastResult1.getScore();
          int score2 = _twinListener.lastResult2.getScore();
          System.out.printf("SCORE = %-33s %7d - %-33s %7d\n",
                            botName1, score1, botName2, score2);

          CompetitorData cd1, cd2;
          if (competitorHash.containsKey(botName1)) {
            cd1 = competitorHash.get(botName1);
          } else {
            cd1 = new CompetitorData();
            cd1.name = botName1;
            competitorHash.put(botName1, cd1);
          }
          cd1.score += score1;

          if (competitorHash.containsKey(botName2)) {
            cd2 = competitorHash.get(botName2);
          } else {
            cd2 = new CompetitorData();
            cd2.name = botName2;
            competitorHash.put(botName2, cd2);
          }
          cd2.score += score2;
        }
      }
      System.out.println();
    }

    return new ArrayList<CompetitorData>(competitorHash.values());
  }

  public static void printCompetitorStats(
                                          ArrayList<CompetitorData> competitorStats) {

    for (int x = 0; x < competitorStats.size(); x++) {
      CompetitorData stats = competitorStats.get(x);
      System.out.printf("%2d. %-33s %7d\n", x + 1, stats.name, stats.score);
    }
  }

  class CompetitorData {
    String name;
    long matchesWon = 0;
    long matchesLost = 0;
    long roundsWon = 0;
    long roundsTotal = 0;
    int tourneySeed = -1;
    int score = 0;

    public double roundWinPercentage() {
      return (
              ((double)Math.round((((double)roundsWon)/roundsTotal)*10000))
              / 100);
    }
  }

  class CompetitorSort implements Comparator<CompetitorData> {
    public int compare(CompetitorData c1, CompetitorData c2) {
      return Integer.signum(c2.score - c1.score);
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
