/*******************************************
 * file name:	ClassNotFoundException.java
 * description:	
 * author:		Kari.Zhang
 * date:		2015-11-29
 *
 ******************************************/

package java.lang;

public class ClassNotFoundException extends Throwable {
	public ClassNotFoundException () {
        super();
	}

    public ClassNotFoundException(String message) {
        super(message);
    }

    public ClassNotFoundException(Throwable cause) {
        super(cause);
    }

    public ClassNotFoundException(String message, Throwable cause) {
        super(message, cause);
    }
}
