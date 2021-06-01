#include "VSimTop.h"
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include "emu.h"

int main(int argc, const char** argv) {
  auto emu = Emulator(argc, argv);

  emu.execute();

  return 0;
}