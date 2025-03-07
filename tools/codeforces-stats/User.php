<?php

class User {
  public string $userId;
  public string $cfHandle;
  public array $submissions; // sorted in increasing order by timestamp
  public int $lastSavedTimestamp;

  function __construct(string $userId, string $cfHandle) {
    $this->userId = $userId;
    $this->cfHandle = $cfHandle;
    $this->submissions = [];
    $this->lastSavedTimestamp = 0;
  }

  static function loadFromConfig(): array {
    $result = [];
    foreach (Config::USERS as $userId => $row) {
      $result[] = new User($userId, $row['codeforces']);
    }
    return $result;
  }

  function asArray(): array {
    $results = [ 'submissions' => [] ];
    foreach ($this->submissions as $sub) {
      $results['submissions'][] = $sub->asArray();
    }
    return $results;
  }

  function getSaveFileName(): string {
    return __DIR__ . '/' . sprintf(Config::SAVE_PATH, $this->userId);
  }

  function loadSavedData(): void {
    $fileName = $this->getSaveFileName();
    if (file_exists($fileName)) {
      $json = file_get_contents($fileName);
      $data = json_decode($json);
      foreach ($data->submissions as $row) {
        $sub = Submission::fromSavedJson($row);
        $this->lastSavedTimestamp = $sub->timestamp;
        $this->submissions[] = $sub;
      }
      // printf("Loaded %d submissions.\n", count($this->submissions));
    }
  }

  function saveData(): void {
    printf("Saving %d submissions locally.\n", count($this->submissions));
    $arr = $this->asArray();
    $json = json_encode($arr);
    $fileName = $this->getSaveFileName();
    file_put_contents($fileName, $json);
  }

  function fetchCodeforcesData(array $taskIds): void {
    $req = new CodeforcesRequest(
      $this->cfHandle, $taskIds, $this->lastSavedTimestamp + 1);

    $subs = $req->fetchSubmissions();

    if (count($subs)) {
      foreach ($subs as $sub) {
        $this->submissions[] = $sub;
        printf("    %s\n", $sub->toString());
      }
      $this->saveData();
    }
  }
}
