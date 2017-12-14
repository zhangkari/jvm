
public class Thread implements Runnable {

    private Runnable target;

    public Thread() {

    }

    public Thread(Runnable r) {
        target = r;
    }

    private native void nativeCreate(Thread t);

    public synchronized void start() {
        nativeCreate(this);
    }

    @Override
    public void run() {
        if (target != null) {
            target.run();
        }
    }
}
