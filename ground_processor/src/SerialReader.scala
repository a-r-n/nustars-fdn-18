import com.fazecast.jSerialComm._
import scala.collection.mutable.ListBuffer

object SerialReader extends Thread {
  private val NO_READ_DELAY = 500

  val buffer: ListBuffer[Byte] = new ListBuffer[Byte]()

  override def run(): Unit = {
    val comPort: SerialPort = SerialPort.getCommPorts()(0)
    comPort.openPort()
    while (true) {
      if (comPort.bytesAvailable() <= 0) {
        Thread.sleep(NO_READ_DELAY)
      } else {
        var readBuffer: Array[Byte] = new Array[Byte](comPort.bytesAvailable)
        comPort.readBytes(readBuffer, readBuffer.length)
        for (x <- readBuffer) {
          buffer += x
        }
      }
    }
  }
}
