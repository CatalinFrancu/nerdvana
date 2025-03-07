<?php

class Submission {
  public int $id;
  public string $taskId; // contest + index letter
  public int $timestamp;
  public string $verdict;
  public int $passedTestCount;
  public int $time;
  public int $memory;

  // $json: A Codeforces Submission object, see
  // https://codeforces.com/apiHelp/objects#Submission
  static function fromCodeforcesJson(Object $json): Submission {
    $s = new Submission();
    $s->id = $json->id;
    $s->taskId = $json->problem->contestId . $json->problem->index;
    $s->timestamp = $json->creationTimeSeconds;
    $s->verdict = $json->verdict;
    $s->passedTestCount = $json->passedTestCount;
    $s->time = $json->timeConsumedMillis;
    $s->memory = $json->memoryConsumedBytes;
    return $s;
  }

  // $json: A Submission object in our own format.
  static function fromSavedJson(Object $json): Submission {
    $s = new Submission();
    $s->id = $json->id;
    $s->taskId = $json->taskId;
    $s->timestamp = $json->timestamp;
    $s->verdict = $json->verdict;
    $s->passedTestCount = $json->passedTestCount;
    $s->time = $json->time;
    $s->memory = $json->memory;
    return $s;
  }

  function asArray(): array {
    return [
      'id' => $this->id,
      'taskId' => $this->taskId,
      'timestamp' => $this->timestamp,
      'verdict' => $this->verdict,
      'passedTestCount' => $this->passedTestCount,
      'time' => $this->time,
      'memory' => $this->memory,
    ];
  }

  function getContest(): string {
    return substr($this->taskId, 0, -1);
  }

  function toString(): string {
    $url = sprintf(Config::SUBMISSION_URL, $this->getContest(), $this->id);
    $s = sprintf("task %s | timestamp %d | verdict %s | passed tests %d | time %d | memory %d | url %s",
                 $this->taskId,
                 $this->timestamp,
                 $this->verdict,
                 $this->passedTestCount,
                 $this->time,
                 $this->memory,
                 $url);
    return $s;
  }
}
