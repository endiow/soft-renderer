#include <vector> 
#include <iostream> 
#include "geometry.h"
#include "tgaimage.h" 
#include "model.h"
#include "shader.h"
#include "camera.h"
using namespace std;

const int WDITH = 800;
const int HEIGHT = 800;
const int DEPTH = 255;

TGAColor white(255, 255, 255, 255);

Vec3f light_dir(0, 0, -1);
Vec3f cameraPos(0, 0, 3);

Vec3f eye_position(0.5f, 0.5f, 1.f);//相机位置
Vec3f center(0.f, 0.f, 0.f);//相机中心指向center

Camera camera(eye_position, Vec3f(0.f, 1.f, 0.f), center - eye_position);

int main(int argc, char** argv) 
{
    TGAImage image(WDITH, HEIGHT, TGAImage::RGB);
    TGAImage zbimage(WDITH, HEIGHT, TGAImage::GRAYSCALE);
    Model* model = new Model("resources/obj/african_head.obj");

    //深度缓冲区，并赋值
    float* zbuffer = new float[WDITH * HEIGHT];
    for (int i = WDITH * HEIGHT; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    shader s(WDITH, HEIGHT, DEPTH, light_dir, cameraPos);

    //s.simpleShading(model, zbuffer, image);

    //s.textureShading(model, zbuffer, image);

    Mat4f model_ = Mat4f::identity();
    Mat4f view_ = Mat4f::identity();
    Mat4f r_inverse = Mat4f::identity();
    Mat4f t_inverse = Mat4f::identity();
    for (int i = 0; i < 3; i++)
    {
        r_inverse[0][i] = camera.Right[i];
        r_inverse[1][i] = camera.Up[i];
        r_inverse[2][i] = -camera.Front[i];

        t_inverse[i][3] = -camera.Position[i];
    }
    view_ = r_inverse * t_inverse;
    Mat4f projection_ = Mat4f::identity();
    projection_[3][2] = -1.0f / cameraPos.z;

    s.MVPShadering(model, zbuffer, image, model_, view_, projection_);

    image.flip_vertically(); // to place the origin in the bottom left corner of the image 
    image.write_tga_file("output/view_shader.tga");
    return 0;
}