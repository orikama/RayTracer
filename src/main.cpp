#include <iostream>
#include <cstdint>
#include <cmath>
#include <limits>
#include <chrono>
#include <memory>

#include <stb_image_write.h>

#include "vec3.hpp"
#include "ray.hpp"
#include "utility.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "material.hpp"


const int g_ImageWidth = 800;
const int g_ImageHeight = 400;
const int g_Channels = 3;

const int g_SamplesPerPixel = 100;
const int g_MaxDepth = 5;


rt::vec3d ray_color(const rt::ray& r, const rt::hittable& world, int depth)
{
    if (depth <= 0)
        return rt::vec3(0.0, 0.0, 0.0);

    rt::hit_record record;

    // NOTE: 0.001 instead of 0.0, fixing "shadow acne" problem
    if (world.hit(r, 0.001, std::numeric_limits<double>::infinity(), record)) {
        rt::ray scattered;
        rt::vec3d attenuation;

        if (record.material_ptr->scatter(r, record, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);

        return rt::vec3(0.0, 0.0, 0.0);
    }

    auto unit_direction = rt::unit_vector(r.direction);
    double t = 0.5 * (unit_direction.y + 1.0);

    return (1.0 - t) * rt::vec3(1.0, 1.0, 1.0) + t * rt::vec3(0.5, 0.7, 1.0);
}


int main()
{
    auto* img = new rt::vec3<uint8_t>[g_ImageHeight * g_ImageWidth];

    rt::hittable_list world;
    world.add(std::make_shared<rt::sphere>(rt::vec3(0.0, 0.0, -1.0),
                                           0.5,
                                           std::make_shared<rt::lambertian>(rt::vec3(0.7, 0.3, 0.3))));
    world.add(std::make_shared<rt::sphere>(rt::vec3(0.0, -100.5, -1.0),
                                           100,
                                           std::make_shared<rt::lambertian>(rt::vec3(0.8, 0.8, 0.0))));

    world.add(std::make_shared<rt::sphere>(rt::vec3(1.0, 0.0, -1.0),
                                           0.5,
                                           std::make_shared<rt::metal>(rt::vec3(0.8, 0.6, 0.2),
                                                                       1.0)));
    world.add(std::make_shared<rt::sphere>(rt::vec3(-1.0, 0.0, -1.0),
                                           0.5,
                                           std::make_shared<rt::metal>(rt::vec3(0.8, 0.8, 0.8),
                                                                       0.3)));


    rt::camera cam;

    auto start_t = std::chrono::high_resolution_clock::now();

    int index = 0;

    for (int j = g_ImageHeight - 1; j >= 0; --j) {
        std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < g_ImageWidth; ++i) {
            rt::vec3 color(0.0, 0.0, 0.0);

            for (int s = 0; s < g_SamplesPerPixel; ++s) {
                double v = double(j + rt::s_random_gen()) / g_ImageHeight;
                double u = double(i + rt::s_random_gen()) / g_ImageWidth;

                rt::ray r = cam.get_ray(u, v);
                color += ray_color(r, world, g_MaxDepth);
            }

            color /= g_SamplesPerPixel ;

            img[index] = rt::vector_sqrt(color) * 255.999;
            ++index;
        }
    }

    auto end_t = std::chrono::high_resolution_clock::now();
    std::cout << '\n' << std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count() << '\n';

    stbi_write_png("image.png", g_ImageWidth, g_ImageHeight, g_Channels, img, g_ImageWidth * g_Channels);

    std::cout << "\nDone.\n";

    delete[] img;

    return 0;
}
