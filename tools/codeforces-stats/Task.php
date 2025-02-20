<?php

class Task {
  public string $contest;
  public string $index; // usually a single uppercase letter
  public string $name;

  function __construct(string $contest, string $index, string $name) {
    $this->contest = $contest;
    $this->index = $index;
    $this->name = $name;
  }

  function getId(): string {
    return $this->contest . $this->index;
  }

  static function loadFromConfig(): array {
    $result = [];
    foreach (Config::CODEFORCES_TASKS as $row) {
      $result[] = new Task($row['contest'], $row['index'], $row['name']);
    }
    return $result;
  }
}
