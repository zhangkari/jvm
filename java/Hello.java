
public class Hello {
	public int add (int a, int b) {
		return a + b;
	}

	public static void main (String [] args) {
		int a = 5201310;
		int b = 4;
		Hello hello = new Hello ();
		int c = hello.add (a, b);
		System.out.println (c);
	}
}
