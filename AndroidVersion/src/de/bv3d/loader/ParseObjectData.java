package de.bv3d.loader;

import java.util.ArrayList;
import java.util.HashMap;

import android.util.Log;

import min3d.animation.AnimationObject3d;
import min3d.animation.KeyFrame;
import min3d.core.Object3d;
import min3d.vos.Color4;
import min3d.vos.Face;
import min3d.vos.Number3d;
import min3d.vos.Uv;

import de.bv3d.Mesh;
import de.bv3d.loader.AParser.BitmapAsset;
import de.bv3d.loader.AParser.Material;
import de.bv3d.loader.AParser.TextureAtlas;


/**
 * orginally taken from min3d - http://code.google.com/p/min3d/
 * 
 * changed by Benjamin Wasty
 */
public class ParseObjectData {
	protected ArrayList<ParseObjectFace> faces;
	protected int numFaces = 0;
	protected ArrayList<Number3d> vertices;
	protected ArrayList<Uv> texCoords;
	protected ArrayList<Number3d> normals;
	
	public String name;
	
	public ParseObjectData()
	{
		this.vertices = new ArrayList<Number3d>();
		this.texCoords = new ArrayList<Uv>();
		this.normals = new ArrayList<Number3d>();
		this.name = "";
		faces = new ArrayList<ParseObjectFace>();
	}

	public ParseObjectData(ArrayList<Number3d> vertices, ArrayList<Uv> texCoords, ArrayList<Number3d> normals)
	{
		this.vertices = vertices;
		this.texCoords = texCoords;
		this.normals = normals;
		this.name = "";
		faces = new ArrayList<ParseObjectFace>();
	}
	
	public AnimationObject3d getParsedObject(TextureAtlas textureAtlas, HashMap<String, Material> materialMap, KeyFrame[] frames)
	{
		AnimationObject3d obj = new AnimationObject3d(numFaces * 3, numFaces, frames.length);
		obj.name(name);
		obj.setFrames(frames);
		
		parseObject(obj, materialMap, textureAtlas);

		return obj;
	}
	
	public Object3d getParsedObject(HashMap<String, Material> materialMap, TextureAtlas textureAtlas) {
		Object3d obj = new Object3d(numFaces * 3, numFaces);
		obj.name(name);
		
		parseObject(obj, materialMap, textureAtlas);
		
		return obj;
	}
	
	public Mesh getParsedObjectAsMesh() {
		Mesh mesh = new Mesh(vertices.size(), numFaces);
		mesh.setName(name);
		
		parseObject(mesh);
		
		return mesh;
	}
	
	private void parseObject(Mesh mesh) {		
		for (int i=0; i<vertices.size(); ++i) {
			Number3d v = vertices.get(i);
			mesh.getVertices().put(v.x);
			mesh.getVertices().put(v.y);
			mesh.getVertices().put(v.z);
		}
		
		if (normals.size() < vertices.size()) Log.w("parseObject", "no normals loaded (at least for some faces)");
		
		for (int i=0; i<normals.size(); ++i) {
			Number3d v = normals.get(i);
			mesh.getNormals().put(v.x);
			mesh.getNormals().put(v.y);
			mesh.getNormals().put(v.z);
		}
		
		for (int i=0; i<faces.size(); ++i) {
			ParseObjectFace f = faces.get(i);
			if (f.faceLength != 3) Log.e("BV3D Object Loader", "Facelength is not 3, only triangles are supported");
			mesh.getTriangleIndices().put((short) f.v[0]);
			mesh.getTriangleIndices().put((short) f.v[1]);
			mesh.getTriangleIndices().put((short) f.v[2]);
		}
	}

	private void parseObject(Object3d obj, HashMap<String, Material> materialMap, TextureAtlas textureAtlas)
	{
		int numFaces = faces.size();
		int faceIndex = 0;
		boolean hasBitmaps = textureAtlas.hasBitmaps();

		for (int i = 0; i < numFaces; i++) {
			ParseObjectFace face = faces.get(i);
			BitmapAsset ba = textureAtlas
					.getBitmapAssetByName(face.materialKey);

			for (int j = 0; j < face.faceLength; j++) {
				Number3d newVertex = vertices.get(face.v[j]);
				
//				 TODO: outofbounds exception bei texCoords.get
				Uv newUv = /*face.hasuv ? texCoords.get(face.uv[j]).clone()
						:*/ new Uv();
				Number3d newNormal = face.hasn ? normals.get(face.n[j])
						: new Number3d();
				Material material = materialMap.get(face.materialKey);
				
				Color4 newColor = new Color4(255, 255, 0, 255);
				if(material != null && material.diffuseColor != null)
				{
					newColor.r = material.diffuseColor.r;
					newColor.g = material.diffuseColor.g;
					newColor.b = material.diffuseColor.b;
					newColor.a = material.diffuseColor.a;
				}

				if(hasBitmaps && (ba != null))
				{
					newUv.u = ba.uOffset + newUv.u * ba.uScale;
					newUv.v = ba.vOffset + ((newUv.v + 1) * ba.vScale) - 1;
				}
				obj.vertices().addVertex(newVertex, newUv, newNormal, newColor);
			}

			if (face.faceLength == 3) {
				obj.faces().add(
						new Face(faceIndex, faceIndex + 1, faceIndex + 2));
			} else if (face.faceLength == 4) {
				obj.faces().add(
						new Face(faceIndex, faceIndex + 1, faceIndex + 3));
				obj.faces().add(
						new Face(faceIndex + 1, faceIndex + 2, faceIndex + 3));
			}

			faceIndex += face.faceLength;
		}

		if (hasBitmaps) {
			obj.textures().addById(textureAtlas.getId());
		}

		cleanup();
	}
	
	public void calculateFaceNormal(ParseObjectFace face)
	{
		Number3d v1 = vertices.get(face.v[0]);
		Number3d v2 = vertices.get(face.v[1]);
		Number3d v3 = vertices.get(face.v[2]);
		
		Number3d vector1 = Number3d.subtract(v2, v1);
		Number3d vector2 = Number3d.subtract(v3, v1);
		
		Number3d normal = new Number3d();
		normal.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
		normal.y = -((vector2.z * vector1.x) - (vector2.x * vector1.z));
		normal.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
		
		double normFactor = Math.sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
		
		normal.x /= normFactor;
		normal.y /= normFactor;
		normal.z /= normFactor;
		
        normals.add(normal);
        
        int index = normals.size() - 1;
        face.n = new int[3];
        face.n[0] = index;
        face.n[1] = index;
        face.n[2] = index;
        face.hasn = true;
	}

	
	protected void cleanup() {
		faces.clear();
	}
}
