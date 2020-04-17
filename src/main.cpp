#include <iostream>
#include <cstdint>
#include <cmath>
#include <limits>
#include <chrono>

#include <stb_image_write.h>

#include "vec3.hpp"
#include "ray.hpp"
#include "utility.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "random_generator.hpp"
#include "camera.hpp"

const int g_ImageWidth = 800;
const int g_ImageHeight = 400;
const int g_Channels = 3;

const int g_SamplesPerPixel = 100;
const int g_MaxDepth = 50;

rt::random_generator<double, std::minstd_rand> random_gen;

rt::vec3d ray_color(const rt::ray& r, const rt::hittable& world, int depth)
{
    if (depth <= 0)
        return rt::vec3(0.0, 0.0, 0.0);

    rt::hit_record record;

    // NOTE: 0.001 instead of 0.0, fixing "shadow acne" problem
    if (world.hit(r, 0.001, std::numeric_limits<double>::infinity(), record)) {
        // NOTE: diffuse reflection
        //auto target = record.p + record.normal + random_gen.random_vec3_in_unit_sphere();
        // NOTE: hemispherical scattering
        //auto target = record.p + random_gen.random_vec3_in_hemisphere(record.normal);
        // NOTE: lambertian reflection
        auto target = record.p + record.normal + random_gen.random_vec3_lambertian();

        return 0.5 * ray_color(rt::ray(record.p, target - record.p), world, depth - 1);
        //return 0.5 * (record.normal + 1.0);
    }

    auto unit_direction = rt::unit_vector(r.direction);
    double t = 0.5 * (unit_direction.y + 1.0);

    return (1.0 - t) * rt::vec3(1.0, 1.0, 1.0) + t * rt::vec3(0.5, 0.7, 1.0);
}


int main()
{
    auto* img = new rt::vec3<uint8_t>[g_ImageHeight * g_ImageWidth];

    rt::hittable_list world;
    world.add(std::make_shared<rt::sphere>(rt::vec3(0.0, 0.0, -1.0), 0.5));
    world.add(std::make_shared<rt::sphere>(rt::vec3(0.0, -100.5, -1.0), 100));

    rt::camera cam;

    auto start_t = std::chrono::high_resolution_clock::now();

    int index = 0;

    for (int j = g_ImageHeight - 1; j >= 0; --j) {
        std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < g_ImageWidth; ++i) {
            rt::vec3 color(0.0, 0.0, 0.0);

            for (int s = 0; s < g_SamplesPerPixel; ++s) {
                double v = double(j + random_gen()) / g_ImageHeight;
                double u = double(i + random_gen()) / g_ImageWidth;

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
