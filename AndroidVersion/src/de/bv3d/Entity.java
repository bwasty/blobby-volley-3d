package de.bv3d;

import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;

// TODO!: add cached MVP matrix, eye pos?
public class Entity {
	private Mesh mMesh;
	public Mesh getMesh() {return mMesh;}
	public void setMesh(Mesh mesh) {mMesh = mesh;}
	
	private Shader mShader;	
	public Shader getShader() {return mShader;	}
	public void setShader(Shader shader) {this.mShader = shader;	}

	private float[] mTransformMatrix;
	public float[] getTransformMatrix() { return mTransformMatrix;	}
	public void setTransformMatrix(float[] transformMatrix) {	this.mTransformMatrix = transformMatrix;}
	
	public Entity(Mesh mesh, Shader shader) {
		mMesh = mesh;
		mShader = shader;
		mTransformMatrix = new float[16];
		Matrix.setIdentityM(mTransformMatrix, 0);
	}

	public void draw(float[] eyePos, float[] viewProjMatrix) {
		float[] mvpMatrix = new float[16];
		Matrix.multiplyMM(mvpMatrix, 0, viewProjMatrix, 0, mTransformMatrix, 0);
		
		GLES20.glUseProgram(mShader.ProgramObject);
		checkGlError("glUseProgram");
		
		GLES20.glUniform3f(mShader.EyePosLoc, eyePos[0], eyePos[1], eyePos[2]);
		
		GLES20.glUniformMatrix4fv(mShader.MVPMatrixLoc, 1, false, mvpMatrix, 0);
		//checkGlError("glUniformMatrix4fv");
		
		GLES20.glVertexAttribPointer (mShader.PositionLoc, 3, GLES20.GL_FLOAT, 
                						false, 0, mMesh.getVertices());
		GLES20.glEnableVertexAttribArray(mShader.PositionLoc);
		//checkGlError("glVertexAttribPointer");
		
//		if (mShader.NormalLoc != -1) {
		GLES20.glVertexAttribPointer (mShader.NormalLoc, 3, GLES20.GL_FLOAT,
		                				false, 0, mMesh.getNormals());
		GLES20.glEnableVertexAttribArray(mShader.NormalLoc);
//		}
		
	

		checkGlError("glEnableVertexAttribArray");
		
		GLES20.glDrawElements (GLES20.GL_TRIANGLES, mMesh.getTriangleIndices().capacity(), GLES20.GL_UNSIGNED_SHORT, mMesh.getTriangleIndices());
		checkGlError("glDrawElements");
	}
	
	// TODO: put in better place
    public static void checkGlError(String op) {
        int error;
        while ((error = GLES20.glGetError()) != GLES20.GL_NO_ERROR) {
            Log.e("BV3D-Renderer", op + ": glError " + error);
            throw new RuntimeException(op + ": glError " + error);
        }
    }
	
	// TODO: collision geometry...?

}
