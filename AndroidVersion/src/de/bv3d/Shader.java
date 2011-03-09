package de.bv3d;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import android.content.res.Resources;
import android.opengl.GLES20;
import android.util.Log;

// loadShader and loadProgram are copied from ESShader - http://code.google.com/p/opengles-book-samples/
// TODO: destructor with delete program?
public class Shader {
	// TODO: mapping of position, normal, samplers etc?
	
	public int ProgramObject;
	public int MVPMatrixLoc;
	public int PositionLoc;
	public int NormalLoc;
	public int TexCoordLoc;
	public int EyePosLoc;
	
	
	public Shader(Resources res, String vertexShaderFile, String fragmentShaderFile) {
		String vertexShaderSrc = loadFileIntoString(res, vertexShaderFile);
		String fragmentShaderSrc = loadFileIntoString(res, fragmentShaderFile);
		
		ProgramObject = loadProgram(vertexShaderSrc, fragmentShaderSrc);
	}

	private String loadFileIntoString(Resources res, String vertexShaderFile) {
		try {
			InputStream is = res.getAssets().open(vertexShaderFile);
			BufferedReader br = new BufferedReader(new InputStreamReader(is));
			StringBuilder sb = new StringBuilder();
			
			char[] buffer = new char[256];
			int len1;
			while ((len1=br.read(buffer)) != -1) {
				sb.append(buffer, 0, len1);
			}			
			
			return sb.toString();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return "";
	}
	
	///
	/// \brief Load a shader, check for compile errors, print error messages to
	/// output log
	/// \param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
	/// \param shaderSrc Shader source string
	/// \return A new shader object on success, 0 on failure
	//
	public static int loadShader(int type, String shaderSrc) {
		int shader;
		int[] compiled = new int[1];

		// Create the shader object
		shader = GLES20.glCreateShader(type);

		if (shader == 0)
			return 0;

		// Load the shader source
		GLES20.glShaderSource(shader, shaderSrc);

		// Compile the shader
		GLES20.glCompileShader(shader);

		// Check the compile status
		GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compiled, 0);

		if (compiled[0] == 0) {
			Log.e("Shader", "Could not compile "+(type==GLES20.GL_VERTEX_SHADER ? "vertex" : "fragment")+" shader: "+GLES20.glGetShaderInfoLog(shader));
			GLES20.glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

	//
	///
	/// \brief Load a vertex and fragment shader, create a program object, link
	///	 program.
	/// Errors output to log.
	/// \param vertShaderSrc Vertex shader source code
	/// \param fragShaderSrc Fragment shader source code
	/// \return A new program object linked with the vertex/fragment shader
	///	 pair, 0 on failure
	//
	public static int loadProgram(String vertShaderSrc, String fragShaderSrc) {
		int vertexShader;
		int fragmentShader;
		int programObject;
		int[] linked = new int[1];

		// Load the vertex/fragment shaders
		vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertShaderSrc);
		if (vertexShader == 0)
			return 0;

		fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragShaderSrc);
		if (fragmentShader == 0) {
			GLES20.glDeleteShader(vertexShader);
			return 0;
		}

		// Create the program object
		programObject = GLES20.glCreateProgram();

		if (programObject == 0)
			return 0;

		GLES20.glAttachShader(programObject, vertexShader);
		GLES20.glAttachShader(programObject, fragmentShader);

		// Link the program
		GLES20.glLinkProgram(programObject);

		// Check the link status
		GLES20.glGetProgramiv(programObject, GLES20.GL_LINK_STATUS, linked, 0);

		if (linked[0] == 0) {
			Log.e("Shader", "Error linking program:");
			Log.e("Shader", GLES20.glGetProgramInfoLog(programObject));
			GLES20.glDeleteProgram(programObject);
			return 0;
		}

		// Free up no longer needed shader resources
		GLES20.glDeleteShader(vertexShader);
		GLES20.glDeleteShader(fragmentShader);

		return programObject;
	}


}
