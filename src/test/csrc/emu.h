#include "VSimTop.h"
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>

int a[] = {23, 25, 78, 13, 64};
int b[] = {34, 68, 18, 40, 25};

int genInput(int ready) {
  static int i = 0;
  if (ready) {
    return i++;
  } else {
    return i;
  }
}

class Emulator {

  VSimTop *dut_ptr;
  uint64_t cycle;

  public:
    Emulator(int argc, const char *argv[]):
      dut_ptr(new VSimTop),
      cycle(0)
    {
      cycle = 0;
    }

    ~Emulator() {
      return;
    }

    uint64_t single_cycle () {
      dut_ptr->clock = 0;
      dut_ptr->eval();

      dut_ptr->clock = 1;
      dut_ptr->eval();

      int idx = genInput(dut_ptr->io_ready);
      dut_ptr->io_loadingValues = 1;
      dut_ptr->io_value1 = a[idx];
      dut_ptr->io_value2 = b[idx];

      if (dut_ptr->io_outputValid) {
        printf("[EMU] cycle:%d out:%d\n", cycle, dut_ptr->io_outputGCD);
      }

      cycle ++;
      return 0;
    }

    void reset_ncycles(size_t cycles) {
      for (int i = 0; i < cycles; i++) {
        dut_ptr->reset = 1;
        dut_ptr->clock = 0;
        dut_ptr->eval();
        dut_ptr->clock = 1;
        dut_ptr->eval();
        dut_ptr->reset = 0;

        cycle ++;
      }

    }

    uint64_t execute_cycles(uint64_t max_cycle) {
      for (int i = 0; i < max_cycle; i++) {
        single_cycle();
      }
      return 0;
    }

    void execute() {
      reset_ncycles(10);
      execute_cycles((uint64_t)(20));
    }
};