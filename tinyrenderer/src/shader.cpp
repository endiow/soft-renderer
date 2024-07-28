#include "shader.h"

//���ȶ��б���ĳ��ɨ�����ϵ�xֵ
priority_queue<float, vector<float>, greater<float>>s;

//�߱� ��¼ÿ��ˮƽ����
struct AET
{
	float xmin, ymax, ymin, dx;
};

//N�����������
struct node
{
	float x, y;
};

//ɨ���߻������
void shader::Scan_line(int n, const float p_point[], TGAImage& image, TGAColor color)
{
	//��������
	int N = n;
	node* point = new node[1024];
	AET* pAET = new AET[1024];
	//tΪpAET[]������±�
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

	//min_yΪ���ж���y�������Сֵ��max_yΪ���ж���y��������ֵ
	int min_y = 0x3fffffff, max_y = -1;
	for (int i = 0; i < N; i++)
	{
		//�������ж���y�������ֵ
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
				//����ʱ�룬ĳ�����ǰһ������
				if (point[(j - 1 + N) % N].y > point[j].y)
				{
					struct AET p;
					p.xmin = point[j].x;
					p.ymax = point[(j - 1 + N) % N].y;
					p.ymin = point[j].y;
					p.dx = (point[(j - 1 + N) % N].x - point[j].x) / (point[(j - 1 + N) % N].y - point[j].y);
					//�ж��Ƿ�Ϊ�ֲ���ֵ
					if (point[(j + 1 + N) % N].y <= point[j].y)
						p.ymin++;
					pAET[t++] = p;
				}
				//����ʱ�룬ĳ����ĺ�һ������
				if (point[(j + 1 + N) % N].y > point[j].y)
				{
					struct AET p;
					p.xmin = point[j].x;
					p.ymax = point[(j + 1 + N) % N].y;
					p.ymin = point[j].y;
					p.dx = (point[(j + 1 + N) % N].x - point[j].x) / (point[(j + 1 + N) % N].y - point[j].y);
					//�ж��Ƿ�Ϊ�ֲ���ֵ
					if (point[(j - 1 + N) % N].y <= point[j].y)
						p.ymin++;
					pAET[t++] = p;
				}
			}
		}
	}
	//����ɨ���߽���ɨ��
	for (int i = min_y; i <= max_y; i++)
	{
		for (int j = 0; j < t; j++)
		{
			//��ǰɨ������ĳ����֮��
			if (pAET[j].ymin <= i && pAET[j].ymax >= i)
			{
				s.push(pAET[j].xmin);
				//����xmin����
				pAET[j].xmin += pAET[j].dx;
			}
		}
		while (!s.empty())
		{	//ÿ����һ��,a,b��Ϊɨ�����ϵ�x����
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

//��������float����
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
	//ǰ��Ķ����䣬���б��

	//dx,dy��ʾx,y����ı仯��
	//ֻҪ���þ���Ϊ�˼���derror
	int dx = x1 - x0;
	int dy = y1 - y0;

	//�൱��ֱ�ߵ�б��
	float derror = std::abs(dy / float(dx));

	float error = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		//��һ����֮ǰһ��
		if (steep) {
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}

		//��ǰ��֪ʶ��˵
		error += derror;
		if (error > .5) {
			// y1>y0����Ҫ���Ƶ�ֱ���ǳ����������Ƶģ�����Ҫy+1
			y += (y1 > y0 ? 1 : -1);
			//��error����[0,1]��Χ��
			error -= 1.;
		}
	}
}

//��������Ƿ�����������
Vec3f shader::barycentric(Vec3i A, Vec3i B, Vec3i C, Vec3f P)
{
	//���ò������P�����������
	Vec3f k = Vec3f(C.x - A.x, B.x - A.x, A.x - P.x) ^ Vec3f(C.y - A.y, B.y - A.y, A.y - P.y);
	//���㹲��ʱ
	if (abs(k.z) < 1)
		return Vec3f(-1, 1, 1);
	//���ع�һ������������
	return Vec3f(1.f - (k.x + k.y) / k.z, k.y / k.z, k.x / k.z);
}

//��Χ�л�������
void shader::triangle(Vec3i* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
	//������Χ�У��������ʼֵ
	Vec2i bboxmin(numeric_limits<int>::max(), numeric_limits<int>::max());
	Vec2i bboxmax(-numeric_limits<int>::max(), -numeric_limits<int>::max());
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; ++i)
	{
		//ȷ����Χ��
		bboxmin.x = max(0, min(bboxmin.x, pts[i].x));
		bboxmin.y = max(0, min(bboxmin.y, pts[i].y));
		//��һ��max��minֻ��Ϊ��ȷ����Χ�еĺϷ���
		bboxmax.x = min(clamp.x, max(bboxmax.x, pts[i].x));
		bboxmax.y = min(clamp.y, max(bboxmax.y, pts[i].y));
	}
	Vec3f p;//��ʱ�����Χ�����ÿһ����������
	for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			//������Χ��
			//��ȡP����������
			Vec3f u = barycentric(pts[0], pts[1], pts[2], p);
			//�ж��Ƿ����������ڣ����ھͲ����������ھͽ�������Ⱦɫ
			if (u.x < 0 || u.y < 0 || u.z < 0) continue;
			
			//ͨ����������������ֵ
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i].z * u.z;
			if (zbuffer[int(p.x + p.y * width)] < p.z)
			{
				//�������ֵ
				zbuffer[int(p.x + p.y * width)] = p.z;
				draw(p, image, color);
			}
		}
	}
}

void shader::triangle_texture(Model* model, Vec3i* pts, Vec2i* uvs, float* zbuffer, TGAImage& image)
{
	//������Χ�У��������ʼֵ
	Vec2i bboxmin(numeric_limits<int>::max(), numeric_limits<int>::max());
	Vec2i bboxmax(-numeric_limits<int>::max(), -numeric_limits<int>::max());
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; ++i)
	{
		//ȷ����Χ��
		bboxmin.x = max(0, min(bboxmin.x, pts[i].x));
		bboxmin.y = max(0, min(bboxmin.y, pts[i].y));
		//��һ��max��minֻ��Ϊ��ȷ����Χ�еĺϷ���
		bboxmax.x = min(clamp.x, max(bboxmax.x, pts[i].x));
		bboxmax.y = min(clamp.y, max(bboxmax.y, pts[i].y));
	}
	Vec3f p;//��ʱ�����Χ�����ÿһ����������
	for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			//������Χ��
			//��ȡP����������
			Vec3f u = barycentric(pts[0], pts[1], pts[2], p);
			//�ж��Ƿ����������ڣ����ھͲ����������ھͽ�������Ⱦɫ
			if (u.x < 0 || u.y < 0 || u.z < 0) continue;

			//������������
			Vec2i Ptex;
			Ptex = uvs[0] * u.x + uvs[1] * u.y + uvs[2] * u.z;
			//cout << uvs[0].x << " " << uvs[0].y << " " << endl;

			//ͨ����������������ֵ
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i].z * u.z;
			if (zbuffer[int(p.x + p.y * width)] < p.z)
			{
				//cout << Ptex.x << " " << Ptex.y << endl;
				TGAColor color = model->diffuse(Ptex); // ���������ɫ
				//�������ֵ
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

	//������Χ�У��������ʼֵ
	Vec2i bboxmin(numeric_limits<int>::max(), numeric_limits<int>::max());
	Vec2i bboxmax(-numeric_limits<int>::max(), -numeric_limits<int>::max());
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; ++i)
	{
		//ȷ����Χ��
		bboxmin.x = max(0, min(bboxmin.x, int(pts[i].x + 0.5f)));
		bboxmin.y = max(0, min(bboxmin.y, int(pts[i].y+0.5f)));
		//��һ��max��minֻ��Ϊ��ȷ����Χ�еĺϷ���
		bboxmax.x = min(clamp.x, max(bboxmax.x, int(pts[i].x + 0.5f)));
		bboxmax.y = min(clamp.y, max(bboxmax.y, int(pts[i].y + 0.5f)));
	}
	Vec3f p;//��ʱ�����Χ�����ÿһ����������
	for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
	{
		for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
		{
			//������Χ��
			//��ȡP����������
			Vec3f u = barycentric(ptss[0], ptss[1], ptss[2], p);
			//�ж��Ƿ����������ڣ����ھͲ����������ھͽ�������Ⱦɫ
			if (u.x < 0 || u.y < 0 || u.z < 0) continue;

			//������������
			Vec2i Ptex;
			Ptex = uvs[0] * u.x + uvs[1] * u.y + uvs[2] * u.z;
			//cout << uvs[0].x << " " << uvs[0].y << " " << endl;

			//ͨ����������������ֵ
			p.z = 0;
			for (int i = 0; i < 3; i++) p.z += pts[i].z * u.z;
			if (zbuffer[int(p.x + p.y * width)] < p.z)
			{
				//cout << Ptex.x << " " << Ptex.y << endl;
				TGAColor color = model->diffuse(Ptex); // ���������ɫ
				//�������ֵ
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

//�򵥵���Ⱦ��ģ��0,0��-1����
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
		//��ȡƽ�淨����
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		//���㷨�����͹��յĵ��
		float intensity = n * light_dir;
		//���Intensity < 0��˵����Ƭ���ڱ��棨�������������λ�ã���ֱ��discard��������Ⱦ��
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
		//��ȡƽ�淨����
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		//���㷨�����͹��յĵ��
		float intensity = n * light_dir;
		//���Intensity < 0��˵����Ƭ���ڱ��棨�������������λ�ã���ֱ��discard��������Ⱦ��
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
		//��ȡƽ�淨����
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		//���㷨�����͹��յĵ��
		float intensity = n * light_dir;
		//���Intensity < 0��˵����Ƭ���ڱ��棨�������������λ�ã���ֱ��discard��������Ⱦ��
		if (intensity > 0)
		{
			triangle_mvp(model, screen_coords, uv_coords, zbuffer, image);
		}
	}
}








