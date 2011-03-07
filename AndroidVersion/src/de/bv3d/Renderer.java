// basis for this class taken from http://code.google.com/p/opengles-book-samples/ 
// Ch8_SimpleVertexShader TODO- proper license attribution

package de.bv3d;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class Renderer implements GLSurfaceView.Renderer {
	private Entity[] mScene; // enough for scene? or vector/class?

	public Renderer(Context context) {// TODO: context necessary?
	}

	@Override
	public void onSurfaceCreated(GL10 gl10, EGLConfig config) {
		//TODO
		
	}

	@Override
	public void onDrawFrame(GL10 gl10) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onSurfaceChanged(GL10 gl10, int arg1, int arg2) {
		// TODO Auto-generated method stub
		
	}

}
