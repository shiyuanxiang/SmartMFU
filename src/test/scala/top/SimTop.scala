package top

import chisel3.stage.ChiselGeneratorAnnotation
// import chisel3.stage.Stage
import chisel3._
// import sim.SimTop
import gcd._

class SimTop extends Module {
  val io = IO(new Bundle {
    val value1        = Input(UInt(16.W))
    val value2        = Input(UInt(16.W))
    val loadingValues = Input(Bool())

    val ready         = Output(Bool())
    val outputGCD     = Output(UInt(16.W))
    val outputValid   = Output(Bool())
  })

  val gcd = Module(new GCD)
  io <> gcd.io

  val timer = RegInit(0.U(64.W))
  timer := timer + 1.U

  when (io.loadingValues) {
    printf(p"${timer}: [SimTop] loading  values:  1:${io.value1} 2:${io.value2}\n")
  }

  when (io.outputValid) {
    printf(p"${timer}: [SimTop] output value: ${io.outputGCD}\n")
  }
}

object SimTop extends App {

  override def main(args: Array[String]): Unit = {
    Driver.execute(args, () => new SimTop)
  }

}