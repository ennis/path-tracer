#pragma once
#include "vec.hpp"
#include "primitive.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

struct Vertex
{
	float x, y, z;
};

struct Face
{
	int vertices[3];
	int normals[3];
	int texCoords[2];
	Vec normal;
};

class Mesh : public Primitive
{
public:

	Mesh(
		Transform const *transform, 
		Material const *material,
		Vec const &emittance) :
		Primitive(transform, material, emittance)
	{}
		
	virtual ~Mesh() 
	{}

	virtual bool intersect(Ray const& R, Intersection& isect) const {
		static const float inf = 1e30f;
		float dist;
		float min_dist = inf;
		int face = -1;
		
		for (int i = 0; i < faces.size(); ++i) {
			if (testIntersection(R, i, dist)) {
				// confirmed hit
				if (face == -1 || dist < min_dist) {
					min_dist = dist;
					face = i;
				}
			}
		}

		if (face == -1) {
			return false;
		}

		isect.primitive = this;
		isect.P = R.O + min_dist * R.D;
		isect.N = faces[face].normal;
		isect.t = min_dist;
		genOrtho(isect.N, isect.T, isect.S);
		return true;
	}

	void loadFromFile(char const* fileName) {
		std::ifstream f;
		f.open(fileName);
		if (f.fail()) {
			std::clog << "Failed to open mesh " << fileName << "\n";
			return;
		}

		std::string line;
		std::stringstream ss;
		Face face;

		float x, y, z;

		while (!f.eof()) {
			std::getline(f, line);
			if (line.substr(0, 2) == "v ") {
				// vertex
				ss.str(line.substr(2));
				ss.seekg(0);
				ss >> x >> y >> z;
				std::clog << "v " << x << ' ' << y << ' ' << z << '\n';
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
			} else if (line.substr(0, 3) == "vn ") {
				// vertex normal
				ss.str(line.substr(3));
				ss.seekg(0);
				ss >> x >> y >> z;
				std::clog << "vn " << x << ' ' << y << ' ' << z << '\n';
				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);
			} else if (line.substr(0, 2) == "f ") {
				// face 
				Face face;
				ss.str(line.substr(2));
				ss.seekg(0);
				for (int i = 0; i < 3; ++i) {
					int a = 0;
					int b = 0;
					int c = 0;
					ss >> a;
					if (ss.get() == '/') {
						if (ss.peek() != '/') {
							ss >> b;
						} 
						if (ss.get() == '/') {
							ss >> c;
						}
					}
					face.vertices[i] = (a-1) * 3;
					face.texCoords[i] = (b-1) * 2;
					face.normals[i] = (c-1) * 3;
				}
				ss.clear();	// sh*t language
				std::clog << "f " << face.vertices[0] << '/' << face.texCoords[0] << '/' << face.normals[0] << ' ' <<
					face.vertices[1] << '/' << face.texCoords[1] << '/' << face.normals[1] << ' ' <<
					face.vertices[2] << '/' << face.texCoords[2] << '/' << face.normals[2] << '\n';
				faces.push_back(face);
			} else {
				// ignore
			}
		}
		
		// compute face normals
		for (int i = 0; i < faces.size(); ++i) {
			Point P1 = getFaceVertex(i, 0);
			Point P2 = getFaceVertex(i, 1);
			Point P3 = getFaceVertex(i, 2);
			faces[i].normal = cross(P2 - P1, P3 - P1).normalized();
		}

		std::clog << fileName << " : " << vertices.size() << " vertices, " <<
			normals.size() << " normals, " << faces.size() << " faces \n";		
	}

	bool testIntersection(Ray const& R, int face, float& dist) const
	{
		Point P1 = getFaceVertex(face, 0);
		Point P2 = getFaceVertex(face, 1);
		Point P3 = getFaceVertex(face, 2);
		Vec N = faces[face].normal;
		//Vec N2 = getFaceNormal(face, 1);
		//Vec N3 = getFaceNormal(face, 2);

		if (!rayPlaneIntersection(R, N, P1, dist)) {
			//std::clog << P1 << '\n';
			return false;
		}

		Point P = R.O + dist * R.D;
		// check if the intersection point lies inside the plane
		Vec U = P2 - P1;
		Vec V = P3 - P1;

		float uu, uv, vv, wu, wv, D;
		uu = dot(U,U);
		uv = dot(U,V);
		vv = dot(V,V);
		Vec W = P - P1;
		wu = dot(W,U);
		wv = dot(W,V);
		D = uv * uv - uu * vv;

		// get and test parametric coords
		float s, t;
		s = (uv * wv - vv * wu) / D;
		if (s < 0.0 || s > 1.0)         // I is outside T
			return false;
		t = (uv * wu - uu * wv) / D;
		if (t < 0.0 || (s + t) > 1.0)  // I is outside T
			return false;

		return true;
	}

	Point getFaceVertex(int face, int i) const {
		int base = faces[face].vertices[i];
		return Point(vertices[base + 0], vertices[base + 1], vertices[base + 2]);
	}
	
	Point getFaceNormal(int face, int i) const {
		int base = faces[face].normals[i];
		return Point(normals[base + 0], normals[base + 1], normals[base + 2]);
	}
	
	virtual void sample(Point const& O, Intersection &isect, float &pdf) const {
		// TODO
	}

private:
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<Face> faces;
};