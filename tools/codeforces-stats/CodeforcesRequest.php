<?php

class CodeforcesRequest {

  const BATCH_SIZE = 100;
  const DELAY_MICROS = 3_000_000;

  private static float $lastRequest = 0;

  private string $handle;
  private array $taskIds;
  private int $minTimestamp;
  private bool $more;

  function __construct(string $handle, array $taskIds, int $minTimestamp) {
    $this->handle = $handle;
    $this->taskIds = $taskIds;
    $this->minTimestamp = $minTimestamp;
    $this->more = true;
  }

  function wait() {
    $now = microtime(true);
    if ($now - self::$lastRequest < self::DELAY_MICROS) {
      $sleep = self::DELAY_MICROS - $now + self::$lastRequest;
      printf("sleeping for %0.3f seconds\n", $sleep / 1_000_000);
      usleep((int)$sleep);
    }
  }

  function fetchUrl(string $url): mixed {
    $this->wait();

    $c = curl_init($url);
    curl_setopt($c, CURLOPT_RETURNTRANSFER, true);
    $execResult = curl_exec($c);
    $data = json_decode($execResult);
    if ($data->status != 'OK') {
      printf("Bad status from URL '%s', expected 'OK', got '%s'\n",
             $url, $data->status);
      exit(1);
    }

    self::$lastRequest = microtime(true);
    return $data;
  }

  function apiCall(string $method, array $args): mixed {
    if (count($args)) {
      $argString = '?' . http_build_query($args);
    } else {
      $argString = '';
    }
    $url = sprintf('%s/%s%s',
                   Config::CODEFORCES_URL,
                   $method,
                   $argString);
    $json = $this->fetchUrl($url);
    return $json;
  }

  function fetchSubmissionBatch(string $from): array {
    $args = [
      'handle' => $this->handle,
      'from' => $from,
      'count' => self::BATCH_SIZE,
    ];
    $json = $this->apiCall('user.status', $args);
    $this->more =
      (count($json->result) == self::BATCH_SIZE) &&
      (end($json->result)->creationTimeSeconds >= $this->minTimestamp);

    $results = [];
    foreach ($json->result as $row) {
      if (isset($row->problem->contestId)) {
        $results[] = Submission::fromCodeforcesJson($row);
      }
    }

    printf("Fetched %d submissions, kept %d\n",
           count($json->result), count($results));
    return $results;
  }

  function fetchSubmissions(): array {
    printf("Fetching CF submissions for %s since %d.\n",
           $this->handle, $this->minTimestamp);

    $from = 1;
    $results = [];
    $totalFetched = 0;

    while ($this->more) {
      $subs = $this->fetchSubmissionBatch($from);
      $totalFetched += count($subs);
      $from += self::BATCH_SIZE;
      foreach ($subs as $sub) {
        if (isset($this->taskIds[$sub->taskId]) &&
            ($sub->timestamp >= $this->minTimestamp)) {
          $results[] = $sub;
        }
      }
    }

    printf("Fetched %d submissions, kept %d new.\n", $totalFetched, count($results));

    $results = array_reverse($results);
    return $results;
  }

}
