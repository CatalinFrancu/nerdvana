<?php

class Args {
  private bool $randomize;
  private array $binaries;
  private array $names;

  function parse(): void {
    $opts = getopt('b:n:r');
    if (empty($opts)) {
      $this->usage();
      exit(1);
    }
    $this->randomize = isset($opts['r']);
    $this->binaries = $opts['b'] ?? [];
    $this->names = $opts['n'] ?? [];
    $this->validate();
  }

  private function usage(): void {
    $scriptName = $_SERVER['SCRIPT_FILENAME'];
    print "Apel: $scriptName [-r] -b <cale> -n <nume> [...]\n";
    print "\n";
    print "    -r       :  Randomizează ordinea clienților.\n";
    print "    -b <cale>:  Fișierul binar executabil al unui client.\n";
    print "    -n <nume>:  Numele clientului.\n";
    print "\n";
    print "Opțiunile -b și -n pot fi repetate pentru fiecare client.\n";
  }

  private function validate() {
    if (!count($this->binaries)) {
      throw new WException('Trebuie să specifici cel puțin un binar.');
    }
    if (count($this->binaries) != count($this->names)) {
      throw new WException('Numărul de binare nu corespunde cu numărul de nume.');
    }
  }

  function getNumPlayers(): int {
    return count($this->binaries);
  }

  function getPlayer(int $i): array {
    return [ $this->binaries[$i], $this->names[$i] ];
  }

  function getRandomize(): bool {
    return $this->randomize;
  }
}
