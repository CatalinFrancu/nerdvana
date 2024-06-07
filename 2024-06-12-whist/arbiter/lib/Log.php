<?php

class Log {
  const int FATAL = 0;
  const int ERROR = 1;
  const int WARN = 2;
  const int SUCCESS = 3;
  const int INFO = 4;
  const int DEBUG = 5;

  static function fatal(string $msg, array $args = []) {
    self::error($msg, $args);
    exit(1);
  }

  static function error(string $msg, array $args = [], $indent = 0) {
    if (Config::LOG_LEVEL >= self::ERROR) {
      self::write(AnsiColors::ERROR, $msg, $args, $indent);
    }
  }

  static function warn(string $msg, array $args = [], $indent = 0) {
    if (Config::LOG_LEVEL >= self::WARN) {
      self::write(AnsiColors::WARNING, $msg, $args, $indent);
    }
  }

  static function success(string $msg, array $args = [], $indent = 0) {
    if (Config::LOG_LEVEL >= self::SUCCESS) {
      self::write(AnsiColors::SUCCESS, $msg, $args, $indent);
    }
  }

  static function info(string $msg, array $args = [], $indent = 0) {
    if (Config::LOG_LEVEL >= self::INFO) {
      self::write(AnsiColors::INFO, $msg, $args, $indent);
    }
  }

  static function debug(string $msg, array $args = [], $indent = 0) {
    if (Config::LOG_LEVEL >= self::DEBUG) {
      self::write(AnsiColors::DEFAULT, $msg, $args, $indent);
    }
  }

  static function emptyLine() {
    self::write(AnsiColors::DEFAULT, '');
  }

  private static function write(string $color, string $msg, array $args = [], int $indent = 0) {
    $spaces = str_repeat(' ', 4 * $indent);
    $str = vsprintf($msg, $args);
    printf("%s%s%s%s\n", $spaces, $color, $str, AnsiColors::DEFAULT);
  }
}
