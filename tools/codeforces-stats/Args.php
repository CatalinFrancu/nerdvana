<?php

class Args {
  private bool $update;
  private bool $report;

  function parse() {
    $opts = getopt('', [ 'report', 'update' ]);

    $this->report = isset($opts['report']);
    $this->update = isset($opts['update']);

    if ($this->report == $this->update) {
      $this->usage();
    }
  }

  private function usage(): void {
    $scriptName = $_SERVER['SCRIPT_FILENAME'];
    print "Usage: $scriptName [--report|--update]\n";
    print "\n";
    print "    --report:   Print stats on existing data.\n";
    print "    --update:   Fetch new data from Codeforces.\n";
  }

  function hasReport(): bool {
    return $this->report;
  }

  function hasUpdate(): bool {
    return $this->update;
  }
}
