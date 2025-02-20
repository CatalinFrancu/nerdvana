<?php

// This program downloads or prints information regarding the Codeforces
// submissions for users and tasks specified in Config.php.

require_once 'Config.php';

require_once 'Args.php';
require_once 'CodeforcesRequest.php';
require_once 'StatsCollector.php';
require_once 'Submission.php';
require_once 'Task.php';
require_once 'User.php';

main();

function main(): void {
  $args = new Args();
  $args->parse();
  if ($args->hasUpdate()) {
    $collector = new StatsCollector();
    $collector->run();
  } else {
    var_dump('report');
  }
}
