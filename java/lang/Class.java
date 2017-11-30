/**********************
 * name:    Class.java
 * desc:    
 * authro:  Karic.Zhang
 * date:    2017-11-29
 **********************/

package java.lang;

public final class Class<T> {

    /** 
      * define class loader, 
      * or null for the "booststrap" system loader
      */
    private transient ClassLoader classLoader;

    /**
     * For array classes, the component class object for
     * instanceof/checkcast .
     * (for String[][][], this will be String[][]).
     * null for non-aray classes
     */
    private transient Class<?> componentType;

    private transient String name;

    /**
     * The superclass, or null if this is java.lang.Object, 
     * an interface or primitive type.
     */
    private transient Class<? super T> superClass;

    private Class() {}

    public String toString() {
        return getName();
    }

    public String getName() {
        String name = this.name;
        if (name == null) {
            this.name = name = getNameNative();
        }        
        return name;
    }

    public ClassLoader getClassLoader() {
        if (isPrimive()) {
            return null;
        }
        return null;
    }

    public boolean isPrimive() {
        // TODO
        return true;
    }

    private native String getNameNative();
}

