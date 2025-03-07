<?php

class Reporter {
  private array $tasks;
  private array $users;
  private array $map;

  function __construct() {
    $this->tasks = Task::loadFromConfig();
    $this->users = User::loadFromConfig();
    $this->map = [];
  }

  function run(): void {
    foreach ($this->users as $user) {
      $user->loadSavedData();
      foreach ($user->submissions as $sub) {
        $this->map[$sub->taskId][$user->userId][] = $sub;
      }
    }

    $this->printReport();
  }

  private function printReport(): void {
    foreach ($this->tasks as $task) {
      printf("======== Task %s %s\n", $task->getId(), $task->name);
      foreach ($this->map[$task->getId()] as $userId => $subs) {
        printf("  == %s\n", $userId);
        foreach ($subs as $sub) {
          printf("    %s\n", $sub->toString());
        }
      }
    }
  }
}
