/***************************
 * name:    ClassLoader.java
 * desc:    
 * authro:  Karic.Zhang
 * date:    2017-11-29
 ***************************/

package java.lang;

public final class ClassLoader<T> {
    private final ClassLoader parent;

    protected ClassLoader(ClassLoader parent) {
        this.parent = parent;
    }

    protected ClassLoader() {
        parent = null;
    }

    public static ClassLoader getSystemClassLoader() {
        // TODO
        throw new NullPointerException("not implemented !");
    }

    public Class<?> loadClass(String name) throws ClassNotFoundException {
        return loadClass(name, false);
    }

    protected Class<?> loadClass(String name, boolean resolve)
        throws ClassNotFoundException {
        
        // TODO
        throw new NullPointerException("not implemented !");
    }
}
