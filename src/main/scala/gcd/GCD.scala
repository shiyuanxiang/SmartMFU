// See README.md for license details.

package gcd

import chisel3._

/**
  * Compute GCD using subtraction method.
  * Subtracts the smaller from the larger until register y is zero.
  * value in register x is then the GCD
  */
class GCD extends Module {
  val io = IO(new Bundle {
    val value1        = Input(UInt(32.W))
    val value2        = Input(UInt(32.W))
    val loadingValues = Input(Bool())

    val ready         = Output(Bool())
    val outputGCD     = Output(UInt(32.W))
    val outputValid   = Output(Bool())
  })

  val x  = Reg(UInt())
  val y  = Reg(UInt())
  val hasOne = RegInit(false.B)
  io.ready := !hasOne

  when(x > y) { x := x - y }
    .otherwise { y := y - x }

  when(io.loadingValues && io.ready) {
    x := io.value1
    y := io.value2
    hasOne := true.B
  }
  when (io.outputValid) {
    hasOne := false.B
  }

  io.outputGCD := x
  io.outputValid := y === 0.U && hasOne
}