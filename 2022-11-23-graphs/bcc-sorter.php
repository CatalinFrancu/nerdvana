<?php
// Sorter for biconnected components algorithms. A separate sorter avoids
// cluttering the actual implementations.
//
// We only sort values within each line. Linux sort will do the rest.

// converts a string like u-v to a pair of integers
function edge($str) {
  $parts = explode('-', $str);
  assert(count($parts) == 2);
  $u = (int)$parts[0];
  $v = (int)$parts[1];
  return ($u < $v) ? [ $u, $v ] : [ $v, $u ];
}

while ($line = trim(fgets(STDIN))) {
  $parts = preg_split('/\s+/', $line);
  switch ($parts[0]) {
    case 'AP:':
      assert(count($parts) == 2);
      printf("AP: %d\n", $parts[1]);
      break;

    case 'B:':
      assert(count($parts) == 2);
      $e = edge($parts[1]);
      printf("B: %d-%d\n", $e[0], $e[1]);
      break;

    case 'BCC:':
      // read edges into an array of pairs and sort it
      $edges = [];
      foreach (array_slice($parts, 1) as $part) {
        $edges[] = edge($part);
        usort($edges, function($e, $f) {
          return ($e[0] == $f[0])
            ? $e[1] <=> $f[1]
            : $e[0] <=> $f[0];
        });
      }
      print 'BCC:';
      foreach ($edges as $e) {
        print " {$e[0]}-{$e[1]}";
      }
      print "\n";
      break;

    default:
      exit("Bad line header");
  }
}
