// import Mill dependency
import mill._
import os.Path
// import mill.define.Sources
// import mill.modules.Util
// import mill.scalalib.TestModule.ScalaTest
import scalalib._
// // support BSP
// import mill.bsp._

trait CommonModule extends ScalaModule {
  override def scalaVersion = "2.12.10"

  override def scalacOptions = Seq("-Xsource:2.11")

  private val macroParadise = ivy"org.scalamacros:::paradise:2.1.0"

  override def compileIvyDeps = Agg(macroParadise)

  override def scalacPluginIvyDeps = Agg(macroParadise)
}

val chisel = Agg(
  ivy"edu.berkeley.cs::chisel3:3.4.3"
)

object chiseltest extends CommonModule with SbtModule {
  override def ivyDeps = super.ivyDeps() ++ Agg(
    ivy"edu.berkeley.cs::treadle:1.3.0",
    ivy"org.scalatest::scalatest:3.2.0",
    ivy"com.lihaoyi::utest:0.7.4"
  ) ++ chisel
  object test extends Tests {
    def ivyDeps = Agg(ivy"org.scalacheck::scalacheck:1.14.3")
    def testFrameworks = Seq("org.scalatest.tools.Framework")
  }
}

object Smart extends CommonModule with SbtModule {
  override def millSourcePath = millOuterCtx.millSourcePath

  override def forkArgs = Seq("-Xmx10G")

  override def ivyDeps = super.ivyDeps() ++ chisel
  override def moduleDeps = super.moduleDeps ++ Seq(
    chiseltest
  )

  object test extends Tests {
    override def ivyDeps = super.ivyDeps() ++ Agg(
      ivy"org.scalatest::scalatest:3.2.0"
    )

    def testFrameworks = Seq(
      "org.scalatest.tools.Framework"
    )

    def testOnly(args: String*) = T.command {
      super.runMain("org.scalatest.tools.Runner", args: _*)
    }
  }

}