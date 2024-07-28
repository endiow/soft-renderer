#pragma once

#include <queue>
#include <iostream>

#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

using namespace std;

class shader
{
private:
	int width, height, depth;
	Vec3f light_dir, cameraPos;

	static void draw(Vec3f P, TGAImage& image, TGAColor color);
	
	static Vec3f barycentric(Vec3i A, Vec3i B, Vec3i C, Vec3f P);

	void triangle(Vec3i* pts, float* zbuffer, TGAImage& image, TGAColor color);
	void triangle_texture(Model* model, Vec3i* pts, Vec2i* uvs, float* zbuffer, TGAImage& image);
	void triangle_mvp(Model* model, Vec3f* pts, Vec2i* uvs, float* zbuffer, TGAImage& image);

public:
	shader(int w, int h, int d, Vec3f light, Vec3f camera);

	static void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);

	static void Scan_line(int n, const float point[], TGAImage& image, TGAColor color);
	
	void simpleShading(Model* model, float* zbuffer, TGAImage& image);
	
	void textureShading(Model* model, float* zbuffer, TGAImage& image);

	Vec4f local2homo(Vec3f v);
	Vec4f projectionDivision(Vec4f m);
	Mat4f viewportMat4f(int x, int y, int w, int h);
	Vec3f homo2vertices(Vec4f m);
	void MVPShadering(Model* model, float* zbuffer, TGAImage& image, Mat4f& model_, Mat4f& view_, Mat4f& projection_);
	
	
	
	
	
};