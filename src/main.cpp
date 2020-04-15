#include <iostream>
#include <cstdint>

#include <stb_image_write.h>

#include "vec3.hpp"
#include "ray.hpp"
#include "utility.hpp"

const int g_ImageWidth = 200;
const int g_ImageHeight = 100;
const int g_Channels = 3;


bool hit_sphere(const rt::vec3d& center, double radius, const rt::ray& r)
{
    rt::vec3d oc = r.origin - center;
    auto a = rt::dot(r.direction, r.direction);
    auto b = 2.0 * rt::dot(oc, r.direction);
    auto c = rt::dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4.0 * a * c;

    return discriminant > 0;
}

auto ray_color(const rt::ray& r)
{
    if (hit_sphere(rt::vec3(0.0, 0.0, -1.0), 0.5, r)) {
        return rt::vec3(1.0, 0.0, 0.0);
    }

    rt::vec3d unit_direction = rt::unit_vector(r.direction);
    auto t = 0.5 * (unit_direction.y + 1.0);

    return (1.0 - t) * rt::vec3(1.0, 1.0, 1.0) + t * rt::vec3(0.5, 0.7, 1.0);
}


int main()
{
    auto* img = new rt::vec3<uint8_t>[g_ImageHeight * g_ImageWidth];

    const auto lower_left_corner = rt::vec3(-2.0, -1.0, -1.0);
    const auto horizontal = rt::vec3(4.0, 0.0, 0.0);
    const auto vertical = rt::vec3(0.0, 2.0, 0.0);
    const auto origin = rt::vec3(0.0, 0.0, 0.0);

    rt::ray r(origin, rt::vec3(0.0, 0.0, 0.0));

    int index = 0;
    for (int j = g_ImageHeight - 1; j >= 0; --j) {
        std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;
        double v = double(j) / g_ImageHeight;

        for (int i = 0; i < g_ImageWidth; ++i) {
            double u = double(i) / g_ImageWidth;

            r.direction = rt::vec3(lower_left_corner + u * horizontal + v * vertical);
            rt::vec3 color = ray_color(r);

            img[index] = rt::vec3(255.999 * color.x,
                                  255.999 * color.y,
                                  255.999 * color.z);
            ++index;
        }
    }

    stbi_write_png("image.png", g_ImageWidth, g_ImageHeight, g_Channels, img, g_ImageWidth * g_Channels);

    std::cout << "\nDone.\n";

    delete[] img;

    return 0;
}
