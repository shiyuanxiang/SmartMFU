package smart

import chisel3._
import chisel3.util._

class SerialInnerProduct extends Module {
  val io = IO(new Bundle {
    val neuron = Input(SInt(16.W))
    val weight = Input(SInt(16.W))
    val vld_i = Input(Bool())
    val ctl = Input(new Bundle {
      val first = Bool()
      val last = Bool()
    })

    val result = Output(SInt(32.W))
    val vld_o = Output(Bool())
  })

  val psum = Reg(SInt(32.W))

  val mul = io.neuron * io.weight
  val add = mul + psum

  when (io.vld_i) {
    psum := Mux(io.ctl.first, mul, add)
  }
  when (reset.asBool) {
    psum := 0.S
  }

  io.result := psum
  io.vld_o := RegNext(io.ctl.last, init = false.B)

  // log
  val timer = RegInit(0.U(64.W))
  timer := timer + 1.U

  when (io.ctl.first) {
    printf(p"${timer} [SIP FIRST]: neuron ${Hexadecimal(io.neuron)} weight ${Hexadecimal(io.weight)}\n")
  }
  when (!io.ctl.first && io.vld_i && !io.vld_o) {
    printf(p"${timer} [SIP  WORK]: psum ${Hexadecimal(psum)} neuron ${Hexadecimal(io.neuron)} weight ${Hexadecimal(io.weight)}\n")
  }
  when (io.vld_o) {
    printf(p"${timer} [SIP  LAST]: psum ${Hexadecimal(psum)}\n")
  }
}

class ParallelInnerProduct extends Module {
  val io = IO(new Bundle {
    val neuron = Input(Vec(32, SInt(16.W)))
    val weight = Input(Vec(32, SInt(16.W)))
    val vld_i = Input(Bool())
    val ctl = Input(new Bundle {
      val first = Bool()
      val last = Bool()
    })

    val result = Output(SInt(32.W))
    val vld_o = Output(Bool())
  })

  val psum = Reg(SInt(32.W))

  val mul = io.neuron.zip(io.weight).map{
    case (n, w) => n * w
  }
  val mul_sum = mul.reduce(_ + _)
  val add = mul_sum + psum

  when (io.vld_i) {
    psum := Mux(io.ctl.first, mul_sum, add)
  }
  when (reset.asBool) {
    psum := 0.S
  }

  io.result := psum
  io.vld_o := RegNext(io.ctl.last && io.vld_i, init = false.B)

  // log
  val timer = RegInit(0.U(64.W))
  timer := timer + 1.U

  when (io.ctl.first) {
    printf(p"${timer} [PIP FIRST]: neuron ${io.neuron} weight ${io.weight}\n")
  }
  when (!io.ctl.first && io.vld_i && !io.vld_o) {
    printf(p"${timer} [PIP  WORK]: psum 0x${Hexadecimal(psum)} neuron ${io.neuron} weight ${io.weight}\n")
  }
  when (io.vld_o) {
    printf(p"${timer} [PIP  LAST]: psum 0x${Hexadecimal(psum)}\n")
  }
}

class Control extends Module {
  val io = IO(new Bundle {
    val uop = Flipped(Decoupled(UInt(10.W)))
    val data_ready = Input(Bool())

    val vld_i = Output(Bool())
    val ctl = Output(new Bundle {
      val first = Bool()
      val last = Bool()
    })
  })

  val counter = Reg(UInt(10.W))
  when (io.vld_i) {
    counter := Mux(counter === 0.U, counter, counter - 32.U)
  }
  when (io.uop.valid && io.uop.ready) {
    counter := io.uop.bits
  }

  io.vld_i := counter =/= 0.U && io.data_ready
  io.uop.ready := counter === 0.U
  io.ctl.first := RegNext(io.uop.fire(), init = false.B)
  io.ctl.last  := counter === 32.U

  // log
  val timer = RegInit(0.U(64.W))
  timer := timer + 1.U

  when (io.uop.fire()) {
    printf(p"${timer} [CTL INPUT]: uop:${io.uop}\n")
  }
  when (io.vld_i) {
    printf(p"${timer} [CTL OUTPU]: counter:${counter} first:${io.ctl.first} last:${io.ctl.last}\n")
  }
}

class MatrixFunctionUnit extends Module {
  val io = IO(new Bundle {
    val uop = Flipped(Decoupled(UInt(10.W)))
    val neuron = Flipped(Decoupled(Vec(32, SInt(16.W))))
    val weight = Flipped(Decoupled(Vec(32, SInt(16.W))))

    val vld_o = Output(Bool())
    val result = Output(SInt(32.W))
  })

  val control = Module(new Control)
  val pip = Module(new ParallelInnerProduct)
  val neuron = Reg(Vec(32, SInt(16.W)))
  val weight = Reg(Vec(32, SInt(16.W)))
  val v_n = Reg(Bool())
  val v_w = Reg(Bool())

  control.io.uop <> io.uop
  control.io.data_ready := v_n && v_w
  pip.io.neuron := neuron
  pip.io.weight := weight
  pip.io.vld_i := control.io.vld_i
  pip.io.ctl <> control.io.ctl

  when (pip.io.vld_i) {
    v_w := false.B
    v_n := false.B
  }
  when (io.neuron.valid && io.neuron.ready) {
    v_n := true.B
    neuron := io.neuron.bits
  }
  when (io.weight.valid && io.weight.ready) {
    v_w := true.B
    weight := io.weight.bits
  }

  io.vld_o := pip.io.vld_o
  io.result := pip.io.result
  io.neuron.ready := !v_n
  io.weight.ready := !v_w

  when (reset.asBool) {
    v_n := false.B
    v_w := false.B
  }

  // log
  val timer = RegInit(0.U(64.W))
  timer := timer + 1.U

  // when (io.neuron.fire()) {
  //   printf(p"${timer} [MFU NEURN]: neuron:${io.neuron.bits}\n")
  // }
  // when (io.weight.fire()) {
  //   printf(p"${timer} [MFU WEIGH]: weight:${io.weight.bits}\n")
  // }
}