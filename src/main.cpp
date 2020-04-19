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

#include <thread>
#include <vector>
#include <mutex>

const int g_ImageWidth = 800;
const int g_ImageHeight = 400;
const int g_Channels = 3;

const int g_SamplesPerPixel = 100;
const int g_MaxDepth = 50;

const int g_NumThreads = 4;


class safe_cout
{
public:
    safe_cout()
    {
        pixels_ready = 0;
    }

    void update_thread(int thread_index)
    {
        std::lock_guard lock(m);

        pixels_ready += 400 + thread_index * 10;

        std::cout << '\r' << std::flush;
        std::cout << pixels_ready << '/' << g_ImageHeight * g_ImageWidth;
        std::cout.flush();
    }

private:
    int pixels_ready;
    std::mutex m;
};

safe_cout g_Info;


rt::hittable_list random_scene()
{
    rt::hittable_list world;

    world.add(std::make_shared<rt::sphere>(rt::vec3(0.0, -1000.0, 0.0),
                                           1000,
                                           std::make_shared<rt::lambertian>(rt::vec3(0.5, 0.5, 0.5))));

    int i = 1;
    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            auto choose_material = rt::s_random_gen();
            rt::vec3 center(a + 0.9 * rt::s_random_gen(), 0.2, b + 0.9 * rt::s_random_gen());

            if ((center - rt::vec3(4.0, 0.2, 0.0)).length() > 0.9) {
                // diffuse
                if (choose_material < 0.5) {
                    auto albedo = rt::s_random_gen.random_vec3() * rt::s_random_gen.random_vec3();
                    world.add(std::make_shared<rt::sphere>(center,
                                                           0.2,
                                                           std::make_shared<rt::lambertian>(albedo)));
                }
                // metal
                else if (choose_material < 0.75) {
                    auto albedo = rt::s_random_gen.random_vec3() / 2.0 + 0.5; // interval [0.5, 1.0]
                    auto fuzz = rt::s_random_gen() / 2.0; // interval [0.0, 0.5]
                    world.add(std::make_shared<rt::sphere>(center,
                                                           0.2,
                                                           std::make_shared<rt::metal>(albedo, fuzz)));
                }
                // glass
                else {
                    world.add(std::make_shared<rt::sphere>(center,
                                                           0.2,
                                                           std::make_shared<rt::dielectic>(1.5)));
                }
            }
        }
    }

    world.add(std::make_shared<rt::sphere>(rt::vec3(0.0, 1.0, 0.0),
                                           1.0,
                                           std::make_shared<rt::dielectic>(1.5)));

    world.add(std::make_shared<rt::sphere>(rt::vec3(-4.0, 1.0, 0.0),
                                           1.0,
                                           std::make_shared<rt::lambertian>(rt::vec3(0.4, 0.2, 0.1))));

    world.add(std::make_shared<rt::sphere>(rt::vec3(4.0, 1.0, 0.0),
                                           1.0,
                                           std::make_shared<rt::metal>(rt::vec3(0.7, 0.6, 0.5),
                                                                       0.0)));

    return world;
}

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

// TODO: random generator is not thread safe
void render(int shift, rt::vec3<uint8_t>* img, rt::hittable_list& world, rt::camera& cam)
{
    int index = 1;
    for (int j = g_ImageHeight - 1; j >= 0; --j) {
        //std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = shift; i < g_ImageWidth; i+=g_NumThreads) {
            rt::vec3 color(0.0, 0.0, 0.0);

            for (int s = 0; s < g_SamplesPerPixel; ++s) {
                double v = double(j + rt::s_random_gen()) / g_ImageHeight;
                double u = double(i + rt::s_random_gen()) / g_ImageWidth;

                rt::ray r = cam.get_ray(u, v);
                color += ray_color(r, world, g_MaxDepth);
            }

            color /= g_SamplesPerPixel;

            //img[index++] = rt::vector_sqrt(color) * 255.999;
            img[j * g_ImageWidth + i] = rt::vector_sqrt(color) * 255.999;

            ++index;
            if (index == 400 + shift * 10) {
                index = 0;
                g_Info.update_thread(shift);
            }
        }
    }
}


int main()
{
    constexpr auto look_from = rt::vec3(13.0, 2.0, 3.0);
    constexpr auto look_at = rt::vec3(0.0, 0.0, 0.0);
    constexpr auto up = rt::vec3(0.0, 1.0, 0.0);
    const auto dist_to_focus = 10.0;
    const double aperture = 0.1;
    const double aspect_ratio = double(g_ImageWidth) / g_ImageHeight;
    rt::camera cam(look_from, look_at, up,
                   20, aspect_ratio,
                   aperture, dist_to_focus);

    auto world = random_scene();

    auto* img = new rt::vec3<uint8_t>[g_ImageHeight * g_ImageWidth];


    std::cout << "Pixels: " << g_ImageHeight * g_ImageWidth << std::endl;

    std::vector<std::thread> threads;

    auto start_t = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < g_NumThreads; ++i)
        threads.emplace_back(render, i, img, world, cam);

    for (auto& thread : threads)
        thread.join();


    auto end_t = std::chrono::high_resolution_clock::now();
    std::cout << '\n' << std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count() << '\n';

    stbi_flip_vertically_on_write(true);
    stbi_write_png("image.png", g_ImageWidth, g_ImageHeight, g_Channels, img, g_ImageWidth * g_Channels);

    std::cout << "\nDone.\n";

    delete[] img;

    return 0;
}

/*omp_set_num_threads(2);
#pragma omp parallel for collapse(2)
    for (int j = g_ImageHeight - 1; j >= 0; --j) {
        //std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < g_ImageWidth; ++i) {
            rt::vec3 color(0.0, 0.0, 0.0);

            for (int s = 0; s < g_SamplesPerPixel; ++s) {
                double v = double(j + rt::s_random_gen()) / g_ImageHeight;
                double u = double(i + rt::s_random_gen()) / g_ImageWidth;

                rt::ray r = cam.get_ray(u, v);
                color += ray_color(r, world, g_MaxDepth);
            }

            color /= g_SamplesPerPixel;

            //img[index++] = rt::vector_sqrt(color) * 255.999;
            img[j * g_ImageWidth + i] = rt::vector_sqrt(color) * 255.999;
        }
    }*/