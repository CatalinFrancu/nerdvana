<?php

class StatsCollector {
  private array $tasks;
  private array $taskIds;
  private array $users;

  function buildTaskIds(): void {
    $this->taskIds = [];
    foreach ($this->tasks as $t) {
      $this->taskIds[$t->getId()] = true;
    }
  }

  function run() {
    $this->tasks = Task::loadFromConfig();
    $this->buildTaskIds();
    $this->users = User::loadFromConfig();
    foreach ($this->users as $user) {
      printf("===== User %s\n", $user->userId);
      $user->loadSavedData();
      $user->fetchCodeforcesData($this->taskIds);
    }
  }
}
