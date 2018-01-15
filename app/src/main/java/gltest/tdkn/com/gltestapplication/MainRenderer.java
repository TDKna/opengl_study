package gltest.tdkn.com.gltestapplication;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by tsuzuki.naoki on 2017/08/17.
 */

public class MainRenderer implements GLSurfaceView.Renderer {

    static native void nativeOnDrawFrame();
    static native void nativeOnSurfaceCreated();
    static native void nativeOnSurfaceChanged(int width, int height);
    static native void nativeMoveCamera(float diffX, float diffY);
    static native void nativeRotateCamera(float rotation);

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        nativeOnSurfaceCreated();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int width, int height) {
        nativeOnSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl10) {
        nativeOnDrawFrame();
    }
}
