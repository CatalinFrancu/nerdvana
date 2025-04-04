<?php

main($argv);

function main(array $argv): void {
  $fileName = getFileName($argv);
  $doc = prepareDoc($fileName);
  $tests = getTests($doc);
  filterTruncatedTests($tests);
  writeTests($tests);
}

function getFileName(array $argv): string {
  if (count($argv) != 2) {
    fprintf(STDERR, "Usage:\n");
    fprintf(STDERR, "\n");
    fprintf(STDERR, "1. Open an accepted submission.\n");
    fprintf(STDERR, "2. Click on '_Click_ to see test details'.\n");
    fprintf(STDERR, "3. Save the page.\n");
    fprintf(STDERR, "4. Run php %s <path_to_saved_submission>\n", $argv[0]);
    fprintf(STDERR, "\n");
    fprintf(STDERR, "This will create a subdirectory 'test' in the current directory.\n");

    exit(1);
  }

  return $argv[1];
}

function prepareDoc(string $fileName): DOMDocument {
  $contents = @file_get_contents($fileName) or die("File [{$fileName}] not found.\n");
  $doc = new DOMDocument();
  $doc->preserveWhiteSpace = true;
  @$doc->loadHTML($contents);
  return $doc;
}

function getTests(DOMDocument $doc): array {
  $results = [];

  $xpath = new DOMXPath($doc);
  $query = "//div[@class='tests-placeholder']/div[@class='roundbox borderTopRound']";

  $testNodes = $xpath->query($query);

  foreach ($testNodes as $testNode) {
    $inQuery = 'div[@class="file input-view"]/div[@class="text"]/pre[@class="input"]';
    $in = $xpath->query($inQuery, $testNode)->item(0);
    $outQuery = 'div[@class="file answer-view"]/div[@class="text"]/pre[@class="answer"]';
    $out = $xpath->query($outQuery, $testNode)->item(0);
    $results[] = [
      canonicalize($in->nodeValue),
      canonicalize($out->nodeValue),
    ];
  }

  // reindex from 1
  array_unshift($results, 'ignored');
  unset($results[0]);

  return $results;
}

function canonicalize(string $s): string {
  $s = str_replace("\r", "", $s);
  return $s;
}

function filterTruncatedTests(array& $tests): void {
  foreach ($tests as $i => $test) {
    if (str_contains($test[0], '...') || str_contains($test[1], '...')) {
      print "Skipping test #{$i} because it is truncated.\n";
      unset($tests[$i]);
    }
  }
}

function writeTests(array $tests): void {
  @mkdir('test');

  foreach ($tests as $testNo => $test) {
    $inputFile = sprintf("test/%02d.in", $testNo);
    file_put_contents($inputFile, $test[0]);
    $outputFile = sprintf("test/%02d.ok", $testNo);
    file_put_contents($outputFile, $test[1]);
    printf("Wrote test #%d to [%s] and [%s]\n", $testNo, $inputFile, $outputFile);
  }
}
