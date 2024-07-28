#include "shader.h"

//优先队列保存某条扫描线上的x值
priority_queue<float, vector<float>, greater<float>>s;

//边表 记录每根水平画线
struct AET
{
	float xmin, ymax, ymin, dx;
};

//N个顶点的坐标
struct node
{
	float x, y;
};

//扫描线画多边形
void shader::Scan_line(int n, const float p_point[], TGAImage& image, TGAColor color)
{
	//处理输入
	int N = n;
	node* point = new node[1024];
	AET* pAET = new AET[1024];
	//t为pAET[]数组的下标
	int t = 0;
	cout << N << endl;
	int j = 0;
	for (int i = 0; i < N; i++)
	{
		point[i].x = p_point[j];
		point[i].y = p_point[j + 1];
		j += 2;
		cout << point[i].x << " " << point[i].y << endl;
	}

	//min_y为所有顶点y坐标的最小值，max_y为所有顶点y坐标的最大值
	int min_y = 0x3fffffff, max_y = -1;
	for (int i = 0; i < N; i++)
	{
		//查找所有顶点y坐标的最值
		if (point[i].y > max_y)
			max_y = point[i].y;
		if (point[i].y < min_y)
			min_y = point[i].y;
	}
	for (int i = min_y; i <= max_y; i++)
	{
		for (j = 0; j < N; j++)
		{
			if (point[j].y == i)
			{
				//按逆时针，某顶点的前一个顶点
				if (point[(j - 1 + N) % N].y > point[j].y)
				{
					struct AET p;
					p.xmin = point[j].x;
					p.ymax = point[(j - 1 + N) % N].y;
					p.ymin = point[j].y;
					p.dx = (point[(j - 1 + N) % N].x - point[j].x) / (point[(j - 1 + N) % N].y - point[j].y);
					//判断是否为局部最值
					if (point[(j + 1 + N) % N].y <= point[j].y)
						p.ymin++;
					pAET[t++] = p;
				}
				//按逆时针，某顶点的后一个顶点
				if (point[(j + 1 + N) % N].y > point[j].y)
				{
					struct AET p;
					p.xmin = point[j].x;
					p.ymax = point[(j + 1 + N) % N].y;
					p.ymin = point[j].y;
					p.dx = (point[(j + 1 + N) % N].x - point[j].x) / (point[(j + 1 + N) % N].y - point[j].y);
					//判断是否为局部最值
					if (point[(j - 1 + N) % N].y <= point[j].y)
						p.ymin++;
					pAET[t++] = p;
				}
			}
		}
	}
	//所有扫描线进行扫描
	for (int i = min_y; i <= max_y; i++)
	{
		for (int j = 0; j < t; j++)
		{
			//当前扫描线在某条边之间
			if (pAET[j].ymin <= i && pAET[j].ymax >= i)
			{
				s.push(pAET[j].xmin);
				//更新xmin坐标
				pAET[j].xmin += pAET[j].dx;
			}
		}
		while (!s.empty())
		{	//每两个一对,a,b均为扫描线上的x坐标
			float a = s.top(); s.pop();
			float b = s.top(); s.pop();
			cout << a << " " << b << " " << i << endl;
			for (int x = a + 0.5; x <= b + 0.5; x++)
			{
				image.set(x, i, color);
			}
		}
	}
	delete[] point;
	delete[] pAET;
}

//四舍五入float画点
void shader::draw(Vec3f P, TGAImage& image, TGAColor color)
{
	int x = (int)(P.x + 0.5);
	int y = (int)(P.y + 0.5);
	image.set(x, y, color);
}

void shader::line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	//前面的都不变，检查斜率

	//dx,dy表示x,y方向的变化量
	//只要作用就是为了计算derror
	int dx = x1 - x0;
	int dy = y1 - y0;

	//相当于直线的斜率
	float derror = std::abs(dy / float(dx));

	float error = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		//这一步和之前一样
		if (steep) {
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}

		//如前置知识所说
		error += derror;
		if (error > .5) {
			// y1>y0代表要绘制的直线是呈现上升趋势的，所以要y+1
			y += (y1 > y0 ? 1 : -1);
			//让error处于[0,1]范围内
			error -= 1.;
		}
	}
}

//重心求点是否在三角形内
Vec3f shader::barycentric(Vec3i A, Vec3i B, Vec3i C, Vec3f P)
{
	//利用叉积计算P点的重心坐标
	Vec3f k = Vec3f(C.x - A.x, B.x - A.x, A.x - P.x) ^ Vec3f(C.y - A.y, B.y - A.y, A.y - P.y);
	//三点共线时
	if (abs(k.z) < 1)
		return Vec3f(-1, 1, 1);
	//返回归一化的重心坐标
	return Vec3f(1.f - (k.x + k.y) / k.z, k.y / k.z, k.x / k.z);
}

//包围盒画三角形
void shader::triangle(Vec3i* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
	//声明包围盒，并给予初始值
	Vec2i bboxmin(numeric_limits<int>::max(), numeric_limits<int>::max());
	Vec2i bboxmax(-numeric_limits<int>::max(), -numeric_limits<int>::max());
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; ++i)
	{
		//确定包围盒
		bboxmin.x = max(0, min(bboxmin.x, pts[i].x));
		bboxmin.y = max(0, min(bboxmin.y, pts[i].y));
		//第一个max和min只是为了确保包围盒的合法性
		bboxmax.x = min(clamp.x, max(bboxmax.x, pts[i].x));
		bboxmax.y = min(clamp.y, max(bboxmax.y, pts[i].y));
	}
	Vec3f p;//临时储存包围盒里的每一个像素坐标
	for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			//遍历包围盒
			//获取P的重心坐标
			Vec3f u = barycentric(pts[0], pts[1], pts[2], p);
			//判断是否在三角形内，不在就不做操作，在就将其像素染色
			if (u.x < 0 || u.y < 0 || u.z < 0) continue;
			
			//通过重心坐标计算深度值
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i].z * u.z;
			if (zbuffer[int(p.x + p.y * width)] < p.z)
			{
				//更新深度值
				zbuffer[int(p.x + p.y * width)] = p.z;
				draw(p, image, color);
			}
		}
	}
}

void shader::triangle_texture(Model* model, Vec3i* pts, Vec2i* uvs, float* zbuffer, TGAImage& image)
{
	//声明包围盒，并给予初始值
	Vec2i bboxmin(numeric_limits<int>::max(), numeric_limits<int>::max());
	Vec2i bboxmax(-numeric_limits<int>::max(), -numeric_limits<int>::max());
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; ++i)
	{
		//确定包围盒
		bboxmin.x = max(0, min(bboxmin.x, pts[i].x));
		bboxmin.y = max(0, min(bboxmin.y, pts[i].y));
		//第一个max和min只是为了确保包围盒的合法性
		bboxmax.x = min(clamp.x, max(bboxmax.x, pts[i].x));
		bboxmax.y = min(clamp.y, max(bboxmax.y, pts[i].y));
	}
	Vec3f p;//临时储存包围盒里的每一个像素坐标
	for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			//遍历包围盒
			//获取P的重心坐标
			Vec3f u = barycentric(pts[0], pts[1], pts[2], p);
			//判断是否在三角形内，不在就不做操作，在就将其像素染色
			if (u.x < 0 || u.y < 0 || u.z < 0) continue;

			//计算纹理坐标
			Vec2i Ptex;
			Ptex = uvs[0] * u.x + uvs[1] * u.y + uvs[2] * u.z;
			//cout << uvs[0].x << " " << uvs[0].y << " " << endl;

			//通过重心坐标计算深度值
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i].z * u.z;
			if (zbuffer[int(p.x + p.y * width)] < p.z)
			{
				//cout << Ptex.x << " " << Ptex.y << endl;
				TGAColor color = model->diffuse(Ptex); // 获得纹理颜色
				//更新深度值
				zbuffer[int(p.x + p.y * width)] = p.z;
				draw(p, image, color);
			}
		}
	}
}

void shader::triangle_mvp(Model* model, Vec3f* pts, Vec2i* uvs, float* zbuffer, TGAImage& image)
{
	Vec3i ptss[3];
	for (int i = 0; i < 3; i++)
	{
		ptss[i].x = int(pts[i].x + 0.5f);
		ptss[i].y = int(pts[i].y + 0.5f);
		ptss[i].z = int(pts[i].z + 0.5f);
	}

	//声明包围盒，并给予初始值
	Vec2i bboxmin(numeric_limits<int>::max(), numeric_limits<int>::max());
	Vec2i bboxmax(-numeric_limits<int>::max(), -numeric_limits<int>::max());
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; ++i)
	{
		//确定包围盒
		bboxmin.x = max(0, min(bboxmin.x, int(pts[i].x + 0.5f)));
		bboxmin.y = max(0, min(bboxmin.y, int(pts[i].y+0.5f)));
		//第一个max和min只是为了确保包围盒的合法性
		bboxmax.x = min(clamp.x, max(bboxmax.x, int(pts[i].x + 0.5f)));
		bboxmax.y = min(clamp.y, max(bboxmax.y, int(pts[i].y + 0.5f)));
	}
	Vec3f p;//临时储存包围盒里的每一个像素坐标
	for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			//遍历包围盒
			//获取P的重心坐标
			Vec3f u = barycentric(ptss[0], ptss[1], ptss[2], p);
			//判断是否在三角形内，不在就不做操作，在就将其像素染色
			if (u.x < 0 || u.y < 0 || u.z < 0) continue;

			//计算纹理坐标
			Vec2i Ptex;
			Ptex = uvs[0] * u.x + uvs[1] * u.y + uvs[2] * u.z;
			//cout << uvs[0].x << " " << uvs[0].y << " " << endl;

			//通过重心坐标计算深度值
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i].z * u.z;
			if (zbuffer[int(p.x + p.y * width)] < p.z)
			{
				//cout << Ptex.x << " " << Ptex.y << endl;
				TGAColor color = model->diffuse(Ptex); // 获得纹理颜色
				//更新深度值
				zbuffer[int(p.x + p.y * width)] = p.z;
				draw(p, image, color);
			}
		}
	}
}

shader::shader(int w, int h, int d, Vec3f light, Vec3f camera)
{
	width = w;
	height = h;
	depth = d;
	light_dir = light;
	cameraPos = camera;
}

//简单的渲染，模拟0,0，-1光照
void shader::simpleShading(Model* model, float* zbuffer, TGAImage& image)
{
	for (int i = 0; i < model->nfaces(); i++)
	{
		vector<int> face = model->face(i);
		Vec3i screen_coords[3];
		Vec3f world_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			int x = (int)((v.x + 1.) * width / 2. + 0.5);
			int y = (int)((v.y + 1.) * height / 2. + 0.5);
			int z = (int)(v.z + 0.5);
			screen_coords[j] = Vec3i(x, y, z);
			world_coords[j] = v;
		}
		//获取平面法向量
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		//计算法向量和光照的点乘
		float intensity = n * light_dir;
		//如果Intensity < 0，说明面片处于背面（摄像机看不到的位置），直接discard（不做渲染）
		if (intensity > 0)
		{
			triangle(screen_coords, zbuffer, image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
		}
	}
}

void shader::textureShading(Model* model, float* zbuffer, TGAImage& image)
{
	for (int i = 0; i < model->nfaces(); i++)
	{
		vector<int> face = model->face(i);
		Vec3i screen_coords[3];
		Vec3f world_coords[3];
		Vec2i uv_coords[3];
		Vec3f normal_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			Vec2i tex = model->uv(i,j);
			int x = (int)((v.x + 1.) * width / 2. + 0.5);
			int y = (int)((v.y + 1.) * height / 2. + 0.5);
			int z = (int)(v.z + 0.5);
			screen_coords[j] = Vec3i(x, y, z);
			//cout << screen_coords[0] << " " << screen_coords[1] << " " << screen_coords[2] << endl;
			world_coords[j] = v;
			uv_coords[j] = tex;
		}
		//获取平面法向量
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		//计算法向量和光照的点乘
		float intensity = n * light_dir;
		//如果Intensity < 0，说明面片处于背面（摄像机看不到的位置），直接discard（不做渲染）
		if (intensity > 0)
		{
			triangle_texture(model, screen_coords, uv_coords, zbuffer, image);
		}
	}
}

Vec4f shader::local2homo(Vec3f v)
{
	Vec4f m;
	m[0] = v.x;
	m[1] = v.y;
	m[2] = v.z;
	m[3] = 1.0f;
	return m;
}

Vec4f shader::projectionDivision(Vec4f m)
{
	Vec4f mm;
	mm[0] = m[0] / m[3];
	mm[1] = m[1] / m[3];
	mm[2] = m[2] / m[3];
	mm[3] = 1.0f;
	return mm;
}

Mat4f shader::viewportMat4f(int x, int y, int w, int h) {
	Mat4f m = Mat4f::identity();
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;

	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;
	return m;
}

Vec3f shader::homo2vertices(Vec4f m)
{
	return Vec3f(m[0], m[1], m[2]);
}

void shader::MVPShadering(Model* model, float* zbuffer, TGAImage& image, Mat4f& model_, Mat4f& view_, Mat4f& projection_)
{
	Mat4f viewport_ = viewportMat4f(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

	for (int i = 0; i < model->nfaces(); i++)
	{
		vector<int> face = model->face(i);
		Vec3f screen_coords[3];
		Vec3f world_coords[3];
		Vec2i uv_coords[3];
		Vec3f normal_coords[3];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v = model->vert(face[j]);
			Vec2i tex = model->uv(i, j);
			Vec3f n = model->norm(i, j);
			int x = (int)((v.x + 1.) * width / 2. + 0.5);
			int y = (int)((v.y + 1.) * height / 2. + 0.5);
			int z = (int)(v.z + 0.5);
			//screen_coords[j] = Vec3i(x, y, z);
			world_coords[j] = v;
			uv_coords[j] = tex;
			normal_coords[j] = n;
			//cout << screen_coords[0] << " " << screen_coords[1] << " " << screen_coords[2] << endl;
			Vec4f lh = local2homo(world_coords[j]);
			Mat4f mvp = projection_ * view_ * model_;
			Vec4f pd = projectionDivision(mvp * lh);
			screen_coords[j] = homo2vertices(viewport_ * pd);
			//cout << screen_coords[0] << " " << screen_coords[1] << " " << screen_coords[2] << endl;
		}
		//获取平面法向量
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		//计算法向量和光照的点乘
		float intensity = n * light_dir;
		//如果Intensity < 0，说明面片处于背面（摄像机看不到的位置），直接discard（不做渲染）
		if (intensity > 0)
		{
			triangle_mvp(model, screen_coords, uv_coords, zbuffer, image);
		}
	}
}








