package de.bv3d;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

public class Mesh {
	public String Name;

	private FloatBuffer mVertices;
	private ShortBuffer mTriangleIndices;
	private FloatBuffer mNormals;
	
//	private FloatBuffer[] mTexCoords;    TODO: probably needed later...
//	private FloatBuffer mVertexColors;
	
	public FloatBuffer getVertices() { return mVertices;}
	public ShortBuffer getTriangleIndices() { return mTriangleIndices;}
	public FloatBuffer getNormals() { return mNormals;}
	
	public Mesh(int numVertices, int numTriangles) {
		// TODO: the numbers in the book samples are different, are they right??
        mVertices = ByteBuffer.allocateDirect(numVertices * 3 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mTriangleIndices = ByteBuffer.allocateDirect(numTriangles * 3 * 2).order(ByteOrder.nativeOrder()).asShortBuffer();
		mNormals = ByteBuffer.allocateDirect(numVertices * 3 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();        
	}
}
