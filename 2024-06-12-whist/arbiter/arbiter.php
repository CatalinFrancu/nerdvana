<?php

// Încarcă la cerere clasele din directorul lib/.
spl_autoload_register(function($className) {
  $fileName = sprintf('%s/lib/%s.php', __DIR__, $className);
  if (file_exists($fileName)) {
    require_once $fileName;
  }
});

main();

function main() {
  try {
    $args = new Args();
    $args->parse();
    $game = new Game($args);
    $game->run();
  } catch (WException $e) {
    Log::fatal($e->getMessage(), $e->getArgs());
  }
}
