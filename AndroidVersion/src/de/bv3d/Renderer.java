// basis for this class taken from http://code.google.com/p/opengles-book-samples/ 
// Ch8_SimpleVertexShader TODO- proper license attribution

package de.bv3d;

import java.util.ArrayList;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;

public class Renderer implements GLSurfaceView.Renderer {
	private ArrayList<Entity> mScene; // enough for scene? or vector/class?
    private float[] mViewMatrix = new float[16];
    public void setViewMatrix(float[] matrix) {mViewMatrix = matrix; }
    
    private float[] mProjMatrix = new float[16];
    
 // TODO: cache VPMatrix?
    
	public Renderer(Context context) {// TODO-: context necessary?
		mScene = new ArrayList<Entity>(10);
		
		Matrix.setLookAtM(mViewMatrix, 0, /*eye*/ 0, 5, 0, /*center*/ 0, 0, 0, /*up*/ 0, 1, 0);
	}
	

	public void addEntity(Entity entity) {
		mScene.add(entity);
	}
	
	public void removeEntity(Entity entity) {
		mScene.remove(entity);
	}


	@Override
	public void onSurfaceCreated(GL10 gl10, EGLConfig config) {
		//TODO?
	}

	// TODO!: fps counting
	@Override
	public void onDrawFrame(GL10 gl10) {
		// TODO: iterate over scene, build mvp matrix
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES20.glClear( GLES20.GL_DEPTH_BUFFER_BIT | GLES20.GL_COLOR_BUFFER_BIT);
        
        GLES20.glCullFace(GLES20.GL_BACK);
        GLES20.glEnable(GLES20.GL_CULL_FACE);
        
        float[] viewProjMatrix=new float[16];
        Matrix.multiplyMM(viewProjMatrix, 0, mViewMatrix, 0, mProjMatrix, 0);
        for (Entity ent : mScene) {
        	ent.draw(viewProjMatrix);
        }
        
        
	}

	@Override
	public void onSurfaceChanged(GL10 gl10, int width, int height) {
        // Ignore the passed-in GL10 interface, and use the GLES20
        // class's static methods instead.
        GLES20.glViewport(0, 0, width, height);
        float ratio = (float) width / height;
        Matrix.frustumM(mProjMatrix, 0, -ratio, ratio, -1, 1, 3, 7);
	}

}
