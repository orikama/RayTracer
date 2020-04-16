#include <iostream>
#include <cstdint>
#include <cmath>
#include <limits>

#include <stb_image_write.h>

#include "vec3.hpp"
#include "ray.hpp"
#include "utility.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"

const int g_ImageWidth = 800;
const int g_ImageHeight = 400;
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
    auto* img = new rt::vec3<uint8_t>[g_ImageHeight * g_ImageWidth];

    const auto lower_left_corner = rt::vec3(-2.0, -1.0, -1.0);
    const auto horizontal = rt::vec3(4.0, 0.0, 0.0);
    const auto vertical = rt::vec3(0.0, 2.0, 0.0);
    const auto origin = rt::vec3(0.0, 0.0, 0.0);

    rt::hittable_list world;
    world.add(std::make_shared<rt::sphere>(rt::vec3(0.0, 0.0, -1.0), 0.5));
    world.add(std::make_shared<rt::sphere>(rt::vec3(0.0, -100.5, -1.0), 100));
    
    rt::ray r(origin, rt::vec3(0.0, 0.0, 0.0));

    int index = 0;
    for (int j = g_ImageHeight - 1; j >= 0; --j) {
        std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
        double v = double(j) / g_ImageHeight;

        for (int i = 0; i < g_ImageWidth; ++i) {
            double u = double(i) / g_ImageWidth;

            r.direction = lower_left_corner + u * horizontal + v * vertical;
            rt::vec3 color = ray_color(r, world);

            img[index] = color * 255.999;
            ++index;
        }
    }

    stbi_write_png("image.png", g_ImageWidth, g_ImageHeight, g_Channels, img, g_ImageWidth * g_Channels);

    std::cout << "\nDone.\n";

    delete[] img;

    return 0;
}
