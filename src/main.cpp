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

const int g_SamplesPerPixel = 50;
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


template<typename FloatType>
rt::hittable_list<FloatType> random_scene()
{
    rt::hittable_list<FloatType> world;

    world.add(std::make_shared<rt::sphere<FloatType>>(rt::vec3<FloatType>(0.0, -1000.0, 0.0),
                                           1000,
                                           std::make_shared<rt::lambertian<FloatType>>(rt::vec3<FloatType>(0.5, 0.5, 0.5))));

    int i = 1;
    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            FloatType choose_material = rt::s_random_gen();
            rt::vec3<FloatType> center(a + 0.9 * rt::s_random_gen(), 0.2, b + 0.9 * rt::s_random_gen());

            if ((center - rt::vec3<FloatType>(4.0, 0.2, 0.0)).length() > 0.9) {
                // diffuse
                if (choose_material < 0.5) {
                    rt::vec3<FloatType> albedo = rt::s_random_gen.random_vec3() * rt::s_random_gen.random_vec3();
                    world.add(std::make_shared<rt::sphere<FloatType>>(center,
                                                           0.2,
                                                           std::make_shared<rt::lambertian<FloatType>>(albedo)));
                }
                // metal
                else if (choose_material < 0.75) {
                    rt::vec3<FloatType> albedo = rt::s_random_gen.random_vec3() / static_cast<FloatType>(2.0 + 0.5); // interval [0.5, 1.0]
                    FloatType fuzz = rt::s_random_gen() / 2.0; // interval [0.0, 0.5]
                    world.add(std::make_shared<rt::sphere<FloatType>>(center,
                                                           0.2,
                                                           std::make_shared<rt::metal<FloatType>>(albedo, fuzz)));
                }
                // glass
                else {
                    world.add(std::make_shared<rt::sphere<FloatType>>(center,
                                                           0.2,
                                                           std::make_shared<rt::dielectic<FloatType>>(1.5)));
                }
            }
        }
    }

    world.add(std::make_shared<rt::sphere<FloatType>>(rt::vec3<FloatType>(0.0, 1.0, 0.0),
                                           1.0,
                                           std::make_shared<rt::dielectic<FloatType>>(1.5)));

    world.add(std::make_shared<rt::sphere<FloatType>>(rt::vec3<FloatType>(-4.0, 1.0, 0.0),
                                           1.0,
                                           std::make_shared<rt::lambertian<FloatType>>(rt::vec3<FloatType>(0.4, 0.2, 0.1))));

    world.add(std::make_shared<rt::sphere<FloatType>>(rt::vec3<FloatType>(4.0, 1.0, 0.0),
                                           1.0,
                                           std::make_shared<rt::metal<FloatType>>(rt::vec3<FloatType>(0.7, 0.6, 0.5),
                                                                       0.0)));

    return world;
}

template<typename FloatType>
rt::vec3<FloatType> ray_color(const rt::ray<FloatType>& r, const rt::hittable<FloatType>& world, int depth)
{
    if (depth <= 0)
        return rt::vec3<FloatType>(0, 0, 0);

    rt::hit_record<FloatType> record;

    // NOTE: 0.001 instead of 0.0, fixing "shadow acne" problem
    if (world.hit(r, 0.001, std::numeric_limits<FloatType>::infinity(), record)) {
        rt::ray<FloatType> scattered;
        rt::vec3<FloatType> attenuation;

        if (record.material_ptr->scatter(r, record, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);

        return rt::vec3<FloatType>(0, 0, 0);
    }

    auto unit_direction = rt::unit_vector(r.direction);
    FloatType t = 0.5 * (unit_direction.y + 1);

    return (1 - t) * rt::vec3<FloatType>(1, 1, 1) + t * rt::vec3<FloatType>(0.5, 0.7, 1.0);
}

// TODO: random generator is not thread safe
template<typename FloatType>
void render(int shift, rt::vec3<uint8_t>* img, rt::hittable_list<FloatType>& world, rt::camera<FloatType>& cam)
{
    int index = 1;
    for (int j = g_ImageHeight - 1; j >= 0; --j) {
        //std::cout << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = shift; i < g_ImageWidth; i+=g_NumThreads) {
            rt::vec3<FloatType> color(0, 0, 0);

            for (int s = 0; s < g_SamplesPerPixel; ++s) {
                FloatType v = FloatType(j + rt::s_random_gen()) / g_ImageHeight;
                FloatType u = FloatType(i + rt::s_random_gen()) / g_ImageWidth;

                rt::ray<FloatType> r = cam.get_ray(u, v);
                color += ray_color(r, world, g_MaxDepth);
            }

            color /= g_SamplesPerPixel;

            //img[index++] = rt::vector_sqrt(color) * 255.999;
            img[j * g_ImageWidth + i] = rt::vector_sqrt(color) * static_cast<FloatType>(255.999);

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
    constexpr auto look_from = rt::vec3<float>(13.0, 2.0, 3.0);
    constexpr auto look_at = rt::vec3<float>(0.0, 0.0, 0.0);
    constexpr auto up = rt::vec3<float>(0.0, 1.0, 0.0);
    const auto dist_to_focus = 10.0f;
    const auto aperture = 0.1f;
    const auto aspect_ratio = float(g_ImageWidth) / g_ImageHeight;
    rt::camera<float> cam(look_from, look_at, up,
                   20.0, aspect_ratio,
                   aperture, dist_to_focus);

    auto world = random_scene<float>();

    auto* img = new rt::vec3<uint8_t>[g_ImageHeight * g_ImageWidth];


    std::cout << "Pixels: " << g_ImageHeight * g_ImageWidth << std::endl;

    std::vector<std::thread> threads;

    auto start_t = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < g_NumThreads; ++i)
        threads.emplace_back(render<float>, i, img, world, cam);

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