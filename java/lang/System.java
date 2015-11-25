/***************************
 *
 * file name:	System.java
 *
 **************************/

package java.lang;

public final class System {
	// public static final PrintStream err = null;
	// public static final PrintStream in = null;
	// public static final PrintStream out = null;

	/*
	public static void setIn (InputStream newIn) {
		in = newIn;
	}
	*/

	/*
	public static void setOut (PrintStream newOut) {
		out = newOut;
	}
	*/

	/*
	public static void setErr (PrintStream newErr) {
		err = newErr;
	}
	*/

	public static native long currentTimeMillis();

	public static native void exit(int code);

	public static native void gc();

	public static void load(String pathName) {

	}

	public static void loadLibrary(String libName) {

	}
}
