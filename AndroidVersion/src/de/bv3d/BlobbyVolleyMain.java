// basis for this class taken from http://code.google.com/p/opengles-book-samples/ 
// Ch8_SimpleVertexShader TODO- proper license attribution

package de.bv3d;

import de.bv3d.loader.Max3DSParser;
import de.bv3d.loader.ObjParser;
import min3d.core.Object3dContainer;
import min3d.parser.IParser;
import min3d.parser.Parser;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;

public class BlobbyVolleyMain extends Activity {
	private GLSurfaceView mGLSurfaceView;
	
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
            mGLSurfaceView.setRenderer(new Renderer(this));
        } 
        else
        {
        	Log.e("BlobbyVolleyMain", "OpenGL ES 2.0 not supported on device.  Exiting...");
        	finish();
           
        }
        setContentView(mGLSurfaceView);
        
//        String s = getResources().getResourceName(R.raw.blobb1);
//        Log.d("main", s);
        
		try {
			Log.d("main", "loading Blobb");
			ObjParser parser = new ObjParser(getResources(), "de.bv3d:raw/blobb_new_obj", false);
			parser.parse();
			
//			Object3dContainer blobb = parser.getParsedObject();
//			Log.e("main", blobb.name());
			
			Mesh blobb = parser.getParsedObjectAsMesh();
			Log.e("main", blobb.Name);
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