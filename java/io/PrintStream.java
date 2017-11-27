package java.io;

/**
  * author: Kari.zhang
  *
  * this implement is just for jvm demo and 
  * not compatible with oracle standard jre
  *
  */
public class PrintStream {
    public PrintStream(String tag) {

    }

    public native void println(int value);
    public native void println(String str);
}
