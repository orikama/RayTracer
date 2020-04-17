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
const int g_SamplesPerPixel = 50;
const int g_Channels = 3;


rt::vec3d ray_color(const rt::ray& r, const rt::hittable& world)
{
    rt::hit_record record;
    if (world.hit(r, 0.0, std::numeric_limits<double>::infinity(), record)) {
        return 0.5 * (record.normal + 1.0);
    }

    auto unit_direction = rt::unit_vector(r.direction);
    double t = 0.5 * (unit_direction.y + 1.0);

    return (1.0 - t) * rt::vec3(1.0, 1.0, 1.0) + t * rt::vec3(0.5, 0.7, 1.0);
}


int main()
{
    rt::random_generator<double, std::minstd_rand> random_double;

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
                double v = double(j + random_double()) / g_ImageHeight;
                double u = double(i + random_double()) / g_ImageWidth;

                rt::ray r = cam.get_ray(u, v);
                color += ray_color(r, world);
            }

            color /= g_SamplesPerPixel;

            img[index] = color * 255.999;
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
