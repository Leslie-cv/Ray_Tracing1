// Ray Tracing.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<memory>
#include"rtweekend.h"
#include"hittable_list.h"
#include"sphere.h"
#include"camera.h"
#include"vec3.h"
#include"material.h"
using namespace std;

double hit_sphere(const vec3& center, double radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b*half_b -  a * c;
   if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-half_b - sqrt(discriminant)) / a;
    }
   
}
vec3 ray_color(const ray& r,const hittable& world,int depth)
{
    hit_record rec;
    if (depth <= 0)
    {
        return vec3(0, 0, 0);
    }
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        vec3 attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return vec3(0, 0, 0);

    }

    vec3 unit_direction = unit_vector(r.direction());//射线单位化
     auto t = 0.5 * (unit_direction.y() + 1.0);//将y从（-1，1）映射到（0，1）
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);//线性插值

}
hittable_list random_scene() {
    hittable_list world;
    world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, make_shared<lambertian>(vec3(0.5, 0.5, 0.5))));
    int i = 1;
    for (int a = -11;a < 11;a++) {
        for (int b = -11;b < 11;b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((center - vec3(4, 0.2, 0)).length() > 0.9)
            {
                if (choose_mat < 0.8)
                {
                    auto albedo = vec3::random() * vec3::random();
                    world.add(make_shared<sphere>(center, 0.2, make_shared<lambertian>(albedo)));
                }
                else if (choose_mat < 0.95) {
                    auto albedo = vec3::random(.5, 1);
                    auto fuzz = random_double(0, .5);
                    world.add(make_shared<sphere>(center, 0.2, make_shared<metal>(albedo, fuzz)));
                }
                else {
                    world.add(make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
                }
            }
        }
    }
    world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(vec3(-4, 1, 0), 1.0, make_shared<lambertian>(vec3(0.4,0.2,0.1))));
    world.add(make_shared<sphere>(vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7,0.6,0.5),0.0)));


    return world;
}
int main()
{
    const int image_width = 200;
    const int image_height = 100;
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    const auto aspect_ratio = double(image_width) / image_height;
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
   /* vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);*/

    auto world = random_scene();
    //auto R = cos(pi / 4);
    //world.add(make_shared<sphere>(
    //    vec3(0, 0, -1),0.5,make_shared<lambertian>(vec3(0.1,0.2,0.5))));
    //world.add(make_shared<sphere>(vec3(0,-100.5, -1),100,make_shared<lambertian>(vec3(0.8,0.8,0))));
    // 
    //world.add(make_shared<sphere>(vec3(1, 0, -1), 0.5, make_shared<metal>(vec3(0.8, 0.6, 0.2),0.3)));
    //world.add(make_shared<sphere>(vec3(-1, 0, -1), 0.5, make_shared<dielectric>(1.5))); //把一个小球套在大球里，光线发生两次折射，将小球的半径设为负值，法相转移到内部
    //world.add(make_shared<sphere>(vec3(-1, 0, -1), -0.45, make_shared<dielectric>(1.5)));
    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    //auto dist_to_focus = (lookfrom - lookat).length();
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom,lookat,vup,20,aspect_ratio,aperture,dist_to_focus);
    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining:" << j << " " << std::flush;
        for (int i = 0; i < image_width; ++i) {
            vec3 color(0, 0, 0);
            for (int s = 0;s < samples_per_pixel;++s)
            {
                auto u = (i + random_double()) / image_width;
                auto v = (j + random_double()) / image_height;
                ray r = cam.get_ray(u, v);
                color += ray_color(r, world,max_depth);
            }
           /* auto u = double(i) / image_width;
            auto v = double(j) / image_height;
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            vec3 color = ray_color(r,world);
            color.write_color(cout);*/
            color.write_color(std::cout,samples_per_pixel);
        }
    }
    cerr << "\nDone.\n";
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
