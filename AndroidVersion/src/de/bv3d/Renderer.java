// basis for this class taken from http://code.google.com/p/opengles-book-samples/ 
// Ch8_SimpleVertexShader TODO- proper license attribution

package de.bv3d;

import java.util.ArrayList;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import de.bv3d.loader.ObjParser;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.os.SystemClock;
import android.util.Log;

public class Renderer implements GLSurfaceView.Renderer {
	private Context mContext;
	private ArrayList<Entity> mScene; // enough for scene? or vector/class?
	private float[] mEyePos = {0, 2, 2.5f};
    private float[] mViewMatrix = new float[16];
    public void setViewMatrix(float[] matrix) {mViewMatrix = matrix; }
    
    private float[] mProjMatrix = new float[16];
    
    private int mFrameCount=0;
    private long mFrameTime;
    
 // TODO!: cache VPMatrix?
    
	public Renderer(Context context) {
		mContext = context;
		mScene = new ArrayList<Entity>(10);
		
		Matrix.setLookAtM(mViewMatrix, 0, /*eye*/ mEyePos[0], mEyePos[1], mEyePos[2], /*center*/ 0, 1, 0, /*up*/ 0, 1, 0);
	}
	

	public void addEntity(Entity entity) {
		mScene.add(entity);
	}
	
	public void removeEntity(Entity entity) {
		mScene.remove(entity);
	}


	@Override
	public void onSurfaceCreated(GL10 gl10, EGLConfig config) {
		Log.d("main", "loading Blobb");
		// TODO!: change ObjParser to use Assets...
		ObjParser parser = new ObjParser(mContext.getResources(), "de.bv3d:raw/blobb_new_obj", false);
		parser.parse();
		
		Mesh blobb = parser.getParsedObjectAsMesh();
	    
	    Shader shader = new Shader(mContext.getResources(), "shaders/simpleLighting.vert", "shaders/simpleLighting.frag");
	    shader.PositionLoc = GLES20.glGetAttribLocation(shader.ProgramObject, "aPosition");
	    shader.NormalLoc = GLES20.glGetAttribLocation(shader.ProgramObject, "aNormal");
	    shader.MVPMatrixLoc = GLES20.glGetUniformLocation(shader.ProgramObject, "uMVPMatrix");
	    shader.EyePosLoc = GLES20.glGetUniformLocation(shader.ProgramObject, "eyePos");
	    //Entity.checkGlError("glGetUniformLocation");
	    
	    Entity ent = new Entity(blobb, shader);
	    
	    addEntity(ent);
	    
	    mFrameTime=SystemClock.uptimeMillis();
	}

	@Override
	public void onDrawFrame(GL10 gl10) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES20.glClear( GLES20.GL_DEPTH_BUFFER_BIT | GLES20.GL_COLOR_BUFFER_BIT);
        
        GLES20.glCullFace(GLES20.GL_BACK);
        GLES20.glEnable(GLES20.GL_CULL_FACE);
        
        float[] viewProjMatrix=new float[16];
        Matrix.multiplyMM(viewProjMatrix, 0, mProjMatrix, 0, mViewMatrix, 0);
        for (Entity ent : mScene) {
        	ent.draw(mEyePos, viewProjMatrix);
        } 
        
        mFrameCount++;
        long currentTime = SystemClock.uptimeMillis();
        long diff;
        if ((diff=currentTime - mFrameTime) >= 10000) {
        	Log.d("BV3D-Renderer", "FPS: "+((float)mFrameCount/diff*1000));
        	mFrameCount=0;
        	mFrameTime=currentTime;
        }
        
	}

	@Override
	public void onSurfaceChanged(GL10 gl10, int width, int height) {
        // Ignore the passed-in GL10 interface, and use the GLES20
        // class's static methods instead.
        GLES20.glViewport(0, 0, width, height);
        float ratio = (float) width / height;
        Matrix.frustumM(mProjMatrix, 0, -ratio, ratio, -1, 1, 1, 20);
	}

	public static void checkGlError(String op) {
	    int error;
	    while ((error = GLES20.glGetError()) != GLES20.GL_NO_ERROR) {
	        Log.e("BV3D-Renderer", op + ": glError " + error);
	        throw new RuntimeException(op + ": glError " + error);
	    }
	}

}
