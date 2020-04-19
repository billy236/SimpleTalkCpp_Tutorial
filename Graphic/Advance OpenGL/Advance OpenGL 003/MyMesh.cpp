#include "precompiledHeader.h"
#include "MyMesh.h"

void MyMesh::createRect(float w, float h) {
	vertices.resize(4);

	float x = w / 2;
	float y = h / 2;

	auto* v = vertices.data();

	v->pos.set(-x,-y, 0); v->color.set(255, 255, 255, 255); v->uv.set(0,1); v->normal.set(0,0,1); v++;
	v->pos.set( x,-y, 0); v->color.set(255, 255, 255, 255); v->uv.set(1,1); v->normal.set(0,0,1); v++;
	v->pos.set( x, y, 0); v->color.set(255, 255, 255, 255); v->uv.set(1,0); v->normal.set(0,0,1); v++;
	v->pos.set(-x, y, 0); v->color.set(255, 255, 255, 255); v->uv.set(0,0); v->normal.set(0,0,1); v++;

	indices.assign({
		0,1,2, 
		0,2,3,
	});

	updateVBO();
}

void MyMesh::createCube(float w, float h, float d) {
	vertices.resize(8);

	float x = w / 2;
	float y = h / 2;
	float z = d / 2;

	auto* v = vertices.data();

	v->pos.set(-x,-y, z); v->color.set(255, 255, 255, 255); v->uv.set(0,1); v++;
	v->pos.set( x,-y, z); v->color.set(255, 255, 255, 255); v->uv.set(1,1); v++;
	v->pos.set( x, y, z); v->color.set(255, 255, 255, 255); v->uv.set(1,0); v++;
	v->pos.set(-x, y, z); v->color.set(255, 255, 255, 255); v->uv.set(0,0); v++;
	v->pos.set(-x,-y,-z); v->color.set(255, 255, 255, 255); v->uv.set(0,1); v++;
	v->pos.set( x,-y,-z); v->color.set(255, 255, 255, 255); v->uv.set(1,1); v++;
	v->pos.set( x, y,-z); v->color.set(255, 255, 255, 255); v->uv.set(1,0); v++;
	v->pos.set(-x, y,-z); v->color.set(255, 255, 255, 255); v->uv.set(0,0); v++;

	indices.assign({
		0,1,2, 
		0,2,3,
		2,1,5,
		2,5,6,
		3,2,6,
		3,6,7,
		0,3,7,
		0,7,4,
		1,0,4,
		1,4,5,
		4,7,6,
		4,6,5,
	});

	updateVBO();
}

void MyMesh::createGrid(int w, int h, float cellSize, const MyColor4b& color) {
	drawMode = GL_LINES;

	indices.clear();
	vertices.resize((w+1 + h+1) * 2);

	auto* v = vertices.data();

	auto offset = MyVec3f(static_cast<float>(w), 0, static_cast<float>(h)) * (cellSize / -2);

	for (int x = 0; x <= w; x++) {
		v->pos = MyVec3f(static_cast<float>(x), 0,                    0 ) * cellSize + offset; v++;
		v->pos = MyVec3f(static_cast<float>(x), 0, static_cast<float>(h)) * cellSize + offset; v++;
	}

	for (int y = 0; y <= h; y++) {
		v->pos = MyVec3f(                   0 , 0, static_cast<float>(y)) * cellSize + offset; v++;
		v->pos = MyVec3f(static_cast<float>(w), 0, static_cast<float>(y)) * cellSize + offset; v++;
	}

	assert(v == vertices.data() + vertices.size());
	setColor(color);
	updateVBO();
}

void MyMesh::createOrigin() {
	drawMode = GL_LINES;

	indices.clear();
	vertices.resize(6);
	auto* v = vertices.data();
	v->pos.set(0,0,0); v->color.set(255, 0, 0, 255); v++;
	v->pos.set(1,0,0); v->color.set(255, 0, 0, 255); v++;

	v->pos.set(0,0,0); v->color.set(0, 255, 0, 255); v++;
	v->pos.set(0,1,0); v->color.set(0, 255, 0, 255); v++;

	v->pos.set(0,0,0); v->color.set(0, 0, 255, 255); v++;
	v->pos.set(0,0,1); v->color.set(0, 0, 255, 255); v++;

	assert(v == vertices.data() + vertices.size());
	updateVBO();
}

void MyMesh::draw() {
	if (indices.size() <= 0)
		return;

	auto stride = static_cast<GLsizei>(sizeof(vertices[0]));

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, stride, vertices[0].pos.data);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, stride, vertices[0].color.data);
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, stride, vertices[0].uv.data);

	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, stride, vertices[0].normal.data);

	//---- set render state
	if (!doubleSided)
		glEnable(GL_CULL_FACE);

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//-- draw
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, indices.data());

	//---- restore render state
	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (!doubleSided)
		glDisable(GL_CULL_FACE);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void MyMesh::clear() {
	vertices.clear();
	indices.clear();

	vertexBuffer.destroy();
	indexBuffer.destroy();
}

void MyMesh::setColor(const MyColor4b& c) {
	for (auto& v : vertices) {
		v.color = c;
	}
}

void MyMesh::updateVBO() {
	vertexBuffer.create(vertices.data(), vertices.size());
	indexBuffer.create(indices.data(), indices.size());
}

class WavefrontObjReader {
public:
	void loadFile(const char* filename, MyMesh* mesh) {
		mesh->clear();

		m_mesh = mesh;

		std::string buf;
		my_loadfile(buf, filename);

		auto* p = buf.c_str();

		while (p) {
			std::string line;
			p = my_getline(line, p);
			m_lineNumber++;

			//printf("> %s\n", line.c_str());

			_readLine(line.c_str());
		}
	}

private:
	void _readLine(const char* p) {
		std::string token;
		p = my_gettoken(token, p);

		//printf("token = %s\n", token.c_str());

		if (token == "v") {
			MyVec3f v;
			float w = 1;
			int ret = sscanf(p, "%f %f %f %f", &v.x, &v.y, &v.z, &w);
			if (ret < 3)
				throw MyError("error read v");

			v /= w;
			m_v.push_back(v);
			return;
		}

		if (token == "vt") {
			MyVec2f vt;
			int ret = sscanf(p, "%f %f", &vt.x, &vt.y);
			if (ret < 2)
				throw MyError("error read vt");

			m_vt.push_back(vt);
			return;
		}

		if (token == "vn" ) {
			MyVec3f vn;
			int ret = sscanf(p, "%f %f %f", &vn.x, &vn.y, &vn.z);
			if (ret < 3)
				throw MyError("error read vn");

			m_vn.push_back(vn);
			return;
		}

		if (token == "f") {
			_readFace(p);
			return;
		}
	}

	void _readFace(const char* p) {
		p = _readFaceVertex(p);
		p = _readFaceVertex(p);
		p = _readFaceVertex(p);
	}

	const char* _readFaceVertex(const char* p) {
		std::string token;
		p = my_gettoken(token, p);

		int v  = 0;
		int vt = 0;
		int vn = 0;

		auto* c = token.c_str();

		std::string tmp;
		c = my_gettoken(tmp, c, '/');

		if (sscanf(tmp.c_str(), "%d", &v) < 1)
			throw MyError("error read f");

		if (c) {
			c = my_gettoken(tmp, c, '/');
			if (tmp.size()) {
				if (sscanf(tmp.c_str(), "%d", &vt) < 1)
					throw MyError("error read f");
			}

			c = my_gettoken(tmp, c, '/');
			if (tmp.size()) {
				if (sscanf(tmp.c_str(), "%d", &vn) < 1)
					throw MyError("error read f");
			}
		}

		//---------
		MyVertex_PosColorUv vtx;
		vtx.pos.set(0,0,0);
		vtx.normal.set(0,0,0);
		vtx.uv.set(0,0);
		vtx.color.set(255, 255, 255, 255);

		//---------
		if (v == 0)
			throw MyError("error read f v = 0");

		if (v < 0) {
			if (static_cast<size_t>(-v) > m_v.size())
				throw MyError("error read f v");

			vtx.pos = m_v[m_v.size() - v];
		} else {
			if (static_cast<size_t>(v) > m_v.size())
				throw MyError("error read f v");
			vtx.pos = m_v[v-1];
		}

		//---------
		if (vt == 0) {
			vtx.uv.set(0,0);
		} else if (vt < 0) {
			if (static_cast<size_t>(-vt) > m_vt.size()) 
				throw MyError("error read f vt");

			vtx.uv = m_vt[m_vt.size() - vt];
		} else {
			if (static_cast<size_t>(vt) > m_vt.size())
				throw MyError("error read f vt");
			vtx.uv = m_vt[vt-1];
		}

		//---------
		if (vn == 0) {
			vtx.normal.set(0,0,0);
		} else if (vn < 0) {
			if (static_cast<size_t>(-vn) > m_vn.size()) 
				throw MyError("error read f vn");

			vtx.normal = m_vn[m_vn.size() - vn];
		} else {
			if (static_cast<size_t>(vn) > m_vn.size())
				throw MyError("error read f vn");
			vtx.normal = m_vn[vn-1];
		}

		//---------
		m_mesh->vertices.push_back(vtx);
		auto vi = m_mesh->vertices.size() - 1;

		if (vi > std::numeric_limits<MyMesh::IndexType>::max())
			throw MyError("error read f max index excess");

		m_mesh->indices.push_back(static_cast<MyMesh::IndexType>(vi));
		return p;
	}

	std::vector<MyVec3f> m_v;
	std::vector<MyVec2f> m_vt;
	std::vector<MyVec3f> m_vn;

	int m_lineNumber = 0;

	MyMesh* m_mesh = nullptr;
};

void MyMesh::loadObjFile(const char* filename) {
	WavefrontObjReader r;
	r.loadFile(filename, this);
	updateVBO();
}

void MyVertexBuffer::create(const MyVertex_PosColorUv* p, size_t n) {
	destroy();
	if (!p || !n)
		return;
	glGenBuffers(1, &m_p);
	glBindBuffer(GL_ARRAY_BUFFER, m_p);
	glBufferData(GL_ARRAY_BUFFER, sizeof(*p) * n, p, GL_DYNAMIC_DRAW);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyVertexBuffer::destroy() {
	if (m_p) {
		glDeleteBuffers(1, &m_p);
		m_p = 0;
	}
}

void MyIndexBuffer::create(const uint16_t* p, size_t n) {
	destroy();
	if (!p || !n)
		return;
	glGenBuffers(1, &m_p);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_p);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*p) * n, p, GL_DYNAMIC_DRAW);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MyIndexBuffer::destroy() {
	if (m_p) {
		glDeleteBuffers(1, &m_p);
		m_p = 0;
	}
}
