/******************************
 * file name:	Exception.java
 * description:	
 * author:		Kari.Zhang
 * date:		2015-11-29
 *
 *****************************/

package java.lang;

public class Exception extends Throwable {
	public Exception () {
        super();
	}

    public Exception(String message) {
        super(message);
    }

    public Exception(Throwable cause) {
        super(cause);
    }

    public Exception(String message, Throwable cause) {
        super(message, cause);
    }
}
