Adapted from [Twin Duel](https://www.cse.chalmers.se/~bergert/robowiki-mirror/RoboWiki/robowiki.net/wiki/Twin_Duel/Tourney_Runner.html).

Nagivate to the parent directory. Then compile with:

```bash
javac -cp /opt/robocode/libs/robocode.jar roundrobin/*.java
```

Run with the following arguments (YMMV):

```bash
java
  -Djava.security.manager=allow
  -Dsun.java2d.uiScale=2
  --add-opens java.base/sun.net.www.protocol.jar=ALL-UNNAMED
  -classpath .:/opt/robocode/libs/robocode.jar
  roundrobin/RunTwinTourney
```
