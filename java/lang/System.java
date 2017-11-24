/***************************
 *
 * file name:	System.java
 * author:      Karim.zhang
 *
 **************************/

package java.lang;

import java.io.PrintStream;

public final class System {
    final static String TAG = "System";

    public static PrintStream out;

    // just for compiling correctly
    static {
        try {
            out = new PrintStream(TAG);
        } catch(Exception e) {

        }
    }

	public static native long currentTimeMillis();

	public static native void exit(int code);

	public static native void gc();

	public static void load(String pathName) {

	}

	public static void loadLibrary(String libName) {

	}
}
