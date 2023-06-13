package roundrobin;

import robocode.control.BattleSpecification;
import robocode.control.RobotResults;
import robocode.control.events.BattleCompletedEvent;
import robocode.control.events.BattleErrorEvent;
import robocode.control.events.BattleFinishedEvent;
import robocode.control.events.BattleMessageEvent;
import robocode.control.events.BattlePausedEvent;
import robocode.control.events.BattleResumedEvent;
import robocode.control.events.BattleStartedEvent;
import robocode.control.events.RoundEndedEvent;
import robocode.control.events.RoundStartedEvent;
import robocode.control.events.TurnEndedEvent;
import robocode.control.events.TurnStartedEvent;
import robocode.BattleResults;

public class TwinListener implements robocode.control.events.IBattleListener {

  public RobotResults lastResult1;
  public RobotResults lastResult2;
  public boolean lastBattleErrored = false;

  public void battleAborted(BattleSpecification battlespecification) { }

  public void battleMessage(String string) { }

  public void onBattleCompleted(BattleCompletedEvent arg0) {
    RobotResults[] results =
      RobotResults.convertResults(arg0.getIndexedResults());

    lastResult1 = results[0];
    lastResult2 = results[1];
  }

  public void onBattleError(BattleErrorEvent arg0) {
    System.out.println(arg0.getError());
    lastBattleErrored = true;

  }

  public void onBattleFinished(BattleFinishedEvent arg0) {
    // TODO Auto-generated method stub

  }

  public void onBattleMessage(BattleMessageEvent arg0) {
    // TODO Auto-generated method stub

  }

  public void onBattlePaused(BattlePausedEvent arg0) {
    // TODO Auto-generated method stub

  }

  public void onBattleResumed(BattleResumedEvent arg0) {
    // TODO Auto-generated method stub

  }

  public void onBattleStarted(BattleStartedEvent arg0) {
    // TODO Auto-generated method stub

  }

  public void onRoundEnded(RoundEndedEvent arg0) {
    // TODO Auto-generated method stub

  }

  public void onRoundStarted(RoundStartedEvent arg0) {
    // TODO Auto-generated method stub

  }

  public void onTurnEnded(TurnEndedEvent arg0) {
    // TODO Auto-generated method stub

  }

  public void onTurnStarted(TurnStartedEvent arg0) {
    // TODO Auto-generated method stub

  }
}
