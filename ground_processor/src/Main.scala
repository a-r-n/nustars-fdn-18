object Main extends App {
  private val EMPTY_BUFFER_DELAY = 100
  SerialReader.start()
  while (SerialReader.buffer.length < 1000) {
    if (SerialReader.buffer.nonEmpty) {
      println(SerialReader.buffer.head.toInt)
    } else {
      Thread.sleep(EMPTY_BUFFER_DELAY)
    }
  }
  var chl = new String(SerialReader.buffer.toArray)
  println(chl)
}