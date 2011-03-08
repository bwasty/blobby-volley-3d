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
import android.util.Log;

public class Renderer implements GLSurfaceView.Renderer {
	private Context mContext;
	private ArrayList<Entity> mScene; // enough for scene? or vector/class?
    private float[] mViewMatrix = new float[16];
    public void setViewMatrix(float[] matrix) {mViewMatrix = matrix; }
    
    private float[] mProjMatrix = new float[16];
    
 // TODO: cache VPMatrix?
    
	public Renderer(Context context) {// TODO-: context necessary?
		mContext = context;
		mScene = new ArrayList<Entity>(10);
		
		Matrix.setLookAtM(mViewMatrix, 0, /*eye*/ 0, 2, 2.5f, /*center*/ 0, 1, 0, /*up*/ 0, 1, 0);
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
		ObjParser parser = new ObjParser(mContext.getResources(), "de.bv3d:raw/blobb_new_obj", false);
		parser.parse();
		
		Mesh blobb = parser.getParsedObjectAsMesh();
		blobb.resetPosition(); // TODO: if works put in proper place.. -> it does work...every frame?
		
		final String vertexShader =
	        "uniform mat4 uMVPMatrix;\n" +
	        "attribute vec4 aPosition;\n" +
//            "attribute vec3 a_normal;     \n" +
//            "varying vec3 v_normal;       \n" +
	        "void main() {\n" +
	        "  gl_Position = uMVPMatrix * aPosition;\n" +
	        //"  gl_Position = aPosition;\n" +
	        "}\n";

	    final String fragmentShader =
	        "precision mediump float;\n" +
	        "void main() {\n" +
	        "  gl_FragColor = vec4 ( 0.0, 0.0, 1.0, 1.0 );\n" +
	        "}\n";
	    
	    Shader shader = new Shader(vertexShader, fragmentShader);
	    shader.PositionLoc = GLES20.glGetAttribLocation(shader.ProgramObject, "aPosition");
	    shader.MVPMatrixLoc = GLES20.glGetUniformLocation(shader.ProgramObject, "uMVPMatrix");
	    //Entity.checkGlError("glGetUniformLocation");
	    
	    Entity ent = new Entity(blobb, shader); 
	    
	    // check indices of mesh
//	    int maxVertexIndex = blobb.getVertices().capacity() - 1;
//	    float lastVertex = blobb.getVertices().get(maxVertexIndex);
//	    for (int i=0; i < 9/*blobb.getTriangleIndices().capacity()*/; ++i) {
////	    	if (blobb.getTriangleIndices().get(i) > maxVertexIndex / 3)
////	    		throw new Exception();
//	    	Log.d("BV3D", Short.toString(blobb.getTriangleIndices().get(i)));
//	    }
	    
	    addEntity(ent);
	}

	// TODO!: fps counting
	@Override
	public void onDrawFrame(GL10 gl10) {
		// TODO: iterate over scene, build mvp matrix
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES20.glClear( GLES20.GL_DEPTH_BUFFER_BIT | GLES20.GL_COLOR_BUFFER_BIT);
        
//        GLES20.glCullFace(GLES20.GL_BACK);
//        GLES20.glEnable(GLES20.GL_CULL_FACE);
        
        float[] viewProjMatrix=new float[16];
        Matrix.multiplyMM(viewProjMatrix, 0, mProjMatrix, 0, mViewMatrix, 0);
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
        Matrix.frustumM(mProjMatrix, 0, -ratio, ratio, -1, 1, 1, 20);
	}

}
