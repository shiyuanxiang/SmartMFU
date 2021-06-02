#include "VSimTop.h"
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 128
#define BATCH_SIZE 32
short neuron[SIZE];
short weight[SIZE];
int result;

bool finish;

void init_nw() {
  srand(0);
  result = 0;
  for (int i = 0; i < SIZE; i ++) {
    neuron[i] = (short)rand();
    weight[i] = (short)rand();
    result += (int)neuron[i] * (int)weight[i];
    printf("[INIT] %4d %8x %8x %8x\n", i, neuron[i], weight[i], result);
  }
}
int idx = 0;
int res = 0;
int last_res = 0;

void get_data_mfu_raise(VSimTop *dut_ptr) {
  // if (!(idx == SIZE)) {
  //    if (idx != 0 && last_res != dut_ptr->io_result) {
  //      printf("[EMU] last_res:%8x pip:%8x\n", last_res, dut_ptr->io_result);
  //      assert(0);
  //    }
  //  } else {
     if (dut_ptr->io_vld_o) {
       assert(result == dut_ptr->io_result);
       finish = true;
     }
     
  //  }
}

void get_data_mfu_fail(VSimTop *dut_ptr) {
  static bool f = true;
  dut_ptr->io_uop_valid = f;
  dut_ptr->io_uop_bits = SIZE;
  if (dut_ptr->io_uop_ready && f) { f = false; }
#define nmacro(x) dut_ptr->io_neuron_bits_##x = neuron[idx + x];
#define wmacro(x) dut_ptr->io_weight_bits_##x = weight[idx + x];
  nmacro(0);nmacro(1);nmacro(2);nmacro(3);nmacro(4);nmacro(5);nmacro(6);nmacro(7)
  nmacro(8);nmacro(9);nmacro(10);nmacro(11);nmacro(12);nmacro(13);nmacro(14);nmacro(15)
  nmacro(16);nmacro(17);nmacro(18);nmacro(19);nmacro(20);nmacro(21);nmacro(22);nmacro(23)
  nmacro(24);nmacro(25);nmacro(26);nmacro(27);nmacro(28);nmacro(29);nmacro(30);nmacro(31)
  wmacro(0);wmacro(1);wmacro(2);wmacro(3);wmacro(4);wmacro(5);wmacro(6);wmacro(7)
  wmacro(8);wmacro(9);wmacro(10);wmacro(11);wmacro(12);wmacro(13);wmacro(14);wmacro(15)
  wmacro(16);wmacro(17);wmacro(18);wmacro(19);wmacro(20);wmacro(21);wmacro(22);wmacro(23)
  wmacro(24);wmacro(25);wmacro(26);wmacro(27);wmacro(28);wmacro(29);wmacro(30);wmacro(31)
  dut_ptr->io_neuron_valid = !finish;
  dut_ptr->io_weight_valid = !finish;
  last_res = res;
  if (dut_ptr->io_neuron_ready) {
    for (int i = 0; i < BATCH_SIZE; i++) {
      res += (int)neuron[idx + i] * (int)weight[idx + i];
    }

    idx += BATCH_SIZE;
  }
}

// void get_data_pip_raise(VSimTop *dut_ptr) {
//   if (!(idx == SIZE)) {
//     if (idx != 0 && last_res != dut_ptr->io_result) {
//       printf("[EMU] last_res:%8x pip:%8x\n", last_res, dut_ptr->io_result);
//       assert(0);
//     }
//   } else {
//     if (dut_ptr->io_vld_o) {
//       assert(result == dut_ptr->io_result);
//     }
//     finish = true;
//   }
// }

// void get_data_pip_fail(VSimTop *dut_ptr) {
//   dut_ptr->io_ctl_first = idx == 0;
//   dut_ptr->io_ctl_last  = idx == (SIZE - BATCH_SIZE);

// #define nmacro(x) dut_ptr->io_neuron_##x = neuron[idx + x];
// #define wmacro(x) dut_ptr->io_weight_##x = weight[idx + x];
//   nmacro(0);nmacro(1);nmacro(2);nmacro(3);nmacro(4);nmacro(5);nmacro(6);nmacro(7)
//   nmacro(8);nmacro(9);nmacro(10);nmacro(11);nmacro(12);nmacro(13);nmacro(14);nmacro(15)
//   nmacro(16);nmacro(17);nmacro(18);nmacro(19);nmacro(20);nmacro(21);nmacro(22);nmacro(23)
//   nmacro(24);nmacro(25);nmacro(26);nmacro(27);nmacro(28);nmacro(29);nmacro(30);nmacro(31)
//   wmacro(0);wmacro(1);wmacro(2);wmacro(3);wmacro(4);wmacro(5);wmacro(6);wmacro(7)
//   wmacro(8);wmacro(9);wmacro(10);wmacro(11);wmacro(12);wmacro(13);wmacro(14);wmacro(15)
//   wmacro(16);wmacro(17);wmacro(18);wmacro(19);wmacro(20);wmacro(21);wmacro(22);wmacro(23)
//   wmacro(24);wmacro(25);wmacro(26);wmacro(27);wmacro(28);wmacro(29);wmacro(30);wmacro(31)
//   dut_ptr->io_vld_i = !finish;
//   // printf("[EMU] %3d: %4x %4x\n", idx, dut_ptr->io_neuron, dut_ptr->io_weight);
//   last_res = res;
//   for (int i = 0; i < BATCH_SIZE; i++) {
//     res += (int)neuron[idx + i] * (int)weight[idx + i];
//   }
  
//   idx += BATCH_SIZE;
// }

// void get_data_sip_raise(VSimTop *dut_ptr) {
//   if (!(idx == SIZE)) {
//     if (idx != 0 && last_res != dut_ptr->io_result) {
//       // printf("[EMU] res:%8x sip:%8x\n", res, dut_ptr->io_result);
//       assert(0);
//     }
//   } else {
//     if (dut_ptr->io_vld_o) {
//       assert(result == dut_ptr->io_result);
//     }
//     finish = true;
//   }
// }

// void get_data_sip_fail(VSimTop *dut_ptr) {
//   dut_ptr->io_ctl_first = idx == 0;
//   dut_ptr->io_ctl_last  = idx == (SIZE - 1);
//   dut_ptr->io_neuron = neuron[idx];
//   dut_ptr->io_weight = weight[idx];
//   dut_ptr->io_vld_i = !finish;
//   // printf("[EMU] %3d: %4x %4x\n", idx, dut_ptr->io_neuron, dut_ptr->io_weight);
//   last_res = res;
//   res += (short)neuron[idx] * (short)weight[idx];
//   idx++;
// }

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

      // get_data_sip_raise(dut_ptr);
      // get_data_pip_raise(dut_ptr);
      get_data_mfu_raise(dut_ptr);

      dut_ptr->clock = 1;
      dut_ptr->eval();

      // get_data_sip_fail(dut_ptr);
      // get_data_pip_fail(dut_ptr);
      get_data_mfu_fail(dut_ptr);

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
      for (int i = 0; i < max_cycle && !finish; i++) {
        single_cycle();
      }
      single_cycle(); // one more cycle to print result
      return 0;
    }

    void execute() {
      finish = false;
      init_nw();
      reset_ncycles(10);
      execute_cycles((uint64_t)(200));
    }
};