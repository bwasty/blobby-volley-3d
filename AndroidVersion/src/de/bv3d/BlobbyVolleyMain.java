// basis for this class taken from http://code.google.com/p/opengles-book-samples/ 
// Ch8_SimpleVertexShader TODO- proper license attribution
// some code also taken from http://developer.android.com/resources/samples/ApiDemos/src/com/example/android/apis/graphics/GLES20TriangleRenderer.html

package de.bv3d;

import de.bv3d.loader.ObjParser;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;

public class BlobbyVolleyMain extends Activity {
	private GLSurfaceView mGLSurfaceView;
	private Renderer mRenderer;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.main); // TODO- remove main layout
        
        mGLSurfaceView = new GLSurfaceView(this);
        
        
        if (detectOpenGLES20()) 
        {
            // Tell the surface view we want to create an OpenGL ES 2.0-compatible
            // context, and set an OpenGL ES 2.0-compatible renderer.
            mGLSurfaceView.setEGLContextClientVersion(2);
            mRenderer = new Renderer(this);
            mGLSurfaceView.setRenderer(mRenderer);
        } 
        else
        {
        	Log.e("BlobbyVolleyMain", "OpenGL ES 2.0 not supported on device.  Exiting...");
        	finish();
           
        }
        setContentView(mGLSurfaceView);
        
		try {
			Log.d("main", "loading Blobb");
			ObjParser parser = new ObjParser(getResources(), "de.bv3d:raw/blobb_new_obj", false);
			parser.parse();
			
			Mesh blobb = parser.getParsedObjectAsMesh();
			
			final String vertexShader =
		        "uniform mat4 uMVPMatrix;\n" +
		        "attribute vec4 aPosition;\n" +
//	            "attribute vec3 a_normal;     \n" +
//	            "varying vec3 v_normal;       \n" +
		        "void main() {\n" +
		        "  gl_Position = uMVPMatrix * aPosition;\n" +
		        "}\n";

		    final String fragmentShader =
		        "precision mediump float;\n" +
		        "void main() {\n" +
		        "  gl_FragColor = vec4 ( 0.0, 0.0, 1.0, 1.0 );\n" +
		        "}\n";
		    
		    Shader shader = new Shader(vertexShader, fragmentShader);
		    shader.PositionLoc = GLES20.glGetAttribLocation(shader.ProgramObject, "aPosition");
		    shader.MVPMatrixLoc = GLES20.glGetUniformLocation(shader.ProgramObject, "uMVPMatrix");
		    
		    Entity ent = new Entity(blobb, shader);  
		    
		    mRenderer.addEntity(ent);
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    
    private boolean detectOpenGLES20() 
    {
        ActivityManager am =
            (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo info = am.getDeviceConfigurationInfo();
        return (info.reqGlEsVersion >= 0x20000);
    }
    
    @Override
    protected void onResume() 
    {
        // Ideally a game should implement onResume() and onPause()
        // to take appropriate action when the activity looses focus
        super.onResume();
        mGLSurfaceView.onResume();
    }

    @Override
    protected void onPause() 
    {
        // Ideally a game should implement onResume() and onPause()
        // to take appropriate action when the activity looses focus
        super.onPause();
        mGLSurfaceView.onPause();
    }

}