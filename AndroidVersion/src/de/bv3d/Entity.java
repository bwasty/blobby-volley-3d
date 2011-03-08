package de.bv3d;

import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;

// TODO!: add cached MVP matrix?
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

	public void draw(float[] viewProjMatrix) {
		float[] mvpMatrix = new float[16];
		Matrix.multiplyMM(mvpMatrix, 0, mTransformMatrix, 0, viewProjMatrix, 0);
		
		GLES20.glUseProgram(mShader.ProgramObject);
		checkGlError("glUseProgram");
		
		GLES20.glVertexAttribPointer (mShader.PositionLoc, 3, GLES20.GL_FLOAT, 
                						false, 0, mMesh.getVertices());
		
		if (mShader.NormalLoc != 0) {
			GLES20.glVertexAttribPointer (mShader.NormalLoc, 3, GLES20.GL_FLOAT,
			                				false, 0, mMesh.getNormals());
			GLES20.glEnableVertexAttribArray(mShader.NormalLoc);
		}
	
		GLES20.glEnableVertexAttribArray(mShader.PositionLoc);
		
		GLES20.glUniformMatrix4fv(mShader.MVPMatrixLoc, 1, false, mvpMatrix, 0);
		
		// TODO: divide second argument by 3 or not? or index-concept totally misunderstood?
		int count = mMesh.getTriangleIndices().capacity();
		GLES20.glDrawElements (GLES20.GL_TRIANGLES, count, GLES20.GL_UNSIGNED_SHORT, mMesh.getTriangleIndices());
		
		checkGlError("glDrawArrays");
	}
	
	
    private void checkGlError(String op) {
        int error;
        while ((error = GLES20.glGetError()) != GLES20.GL_NO_ERROR) {
            Log.e("BV3D-Renderer", op + ": glError " + error);
            throw new RuntimeException(op + ": glError " + error);
        }
    }
	
	// TODO: collision geometry...?

}
