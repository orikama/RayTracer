#include <iostream>
#include <cstdint>
#include <limits>
#include <chrono>
#include <memory>

#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

#include "common/stb_image_write.h"

#include "common/vec3.hpp"
#include "common/ray.hpp"
#include "common/utility.hpp"
#include "common/camera.hpp"

#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"


const int g_ImageWidth = 1000;
const int g_ImageHeight = 500;
const int g_Channels = 3;

const int g_SamplesPerPixel = 40;
const int g_MaxDepth = 20;

const int g_NumThreads = 2;

using fp_type = float;


//class safe_cout
//{
//public:
//    safe_cout()
//    {
//        pixels_ready = 0;
//    }
//
//    void update_thread(int thread_index)
//    {
//        std::lock_guard lock(m);
//
//        pixels_ready += 400 + thread_index * 10;
//
//        std::cout << '\r' << std::flush;
//        std::cout << pixels_ready << '/' << g_ImageHeight * g_ImageWidth;
//        std::cout.flush();
//    }
//
//private:
//    int pixels_ready;
//    std::mutex m;
//};
//
//safe_cout g_Info;


const int g_A = 11;
const int g_B = 11;

rt::hittable_list<fp_type> random_scene()
{
    rt::random_generator<fp_type, std::minstd_rand> random_gen;
    std::uniform_real_distribution<fp_type> dist_0_05(0, 0.5);
    std::uniform_real_distribution<fp_type> dist_05_1(0.5, 1);

    rt::hittable_list<fp_type> world;

    world.add(std::make_shared<rt::sphere<fp_type>>(rt::vec3<fp_type>(0.0, -1000.0, 0.0),
                                                    1000,
                                                    std::make_shared<rt::lambertian<fp_type>>(rt::vec3<fp_type>(0.5, 0.5, 0.5))));

    int i = 1;
    for (int a = -g_A; a < g_A; ++a) {
        for (int b = -g_B; b < g_B; ++b) {
            fp_type choose_material = random_gen();
            rt::vec3<fp_type> center(a + fp_type(0.9) * random_gen(), 0.2, b + fp_type(0.9) * random_gen());

            if ((center - rt::vec3<fp_type>(4.0, 0.2, 0.0)).length() > fp_type(0.9)) {
                // diffuse
                if (choose_material < 0.5) {
                    rt::vec3<fp_type> albedo = random_gen.random_vec3() * random_gen.random_vec3();
                    world.add(std::make_shared<rt::sphere<fp_type>>(center,
                                                                    0.2,
                                                                    std::make_shared<rt::lambertian<fp_type>>(albedo)));
                }
                // metal
                else if (choose_material < fp_type(0.75)) {
                    rt::vec3<fp_type> albedo = random_gen.random_vec3(dist_05_1);
                    fp_type fuzz = random_gen(dist_0_05);
                    world.add(std::make_shared<rt::sphere<fp_type>>(center,
                                                                    0.2,
                                                                    std::make_shared<rt::metal<fp_type>>(albedo, fuzz)));
                }
                // glass
                else {
                    world.add(std::make_shared<rt::sphere<fp_type>>(center,
                                                                    0.2,
                                                                    std::make_shared<rt::dielectic<fp_type>>(1.5)));
                }
            }
        }
    }

    world.add(std::make_shared<rt::sphere<fp_type>>(rt::vec3<fp_type>(0.0, 1.0, 0.0),
                                                    1.0,
                                                    std::make_shared<rt::dielectic<fp_type>>(1.5)));

    world.add(std::make_shared<rt::sphere<fp_type>>(rt::vec3<fp_type>(-4.0, 1.0, 0.0),
                                                    1.0,
                                                    std::make_shared<rt::lambertian<fp_type>>(rt::vec3<fp_type>(0.4, 0.2, 0.1))));

    world.add(std::make_shared<rt::sphere<fp_type>>(rt::vec3<fp_type>(4.0, 1.0, 0.0),
                                                    1.0,
                                                    std::make_shared<rt::metal<fp_type>>(rt::vec3<fp_type>(0.7, 0.6, 0.5),
                                                                                         0.0)));

    return world;
}


rt::vec3<fp_type> ray_color(const rt::ray<fp_type>& r, const rt::hittable<fp_type>& world, int depth, int& ray_count)
{
    if (depth <= 0)
        return rt::vec3<fp_type>(0);

    ++ray_count;

    rt::hit_record<fp_type> record;

    // NOTE: 0.001 instead of 0.0, fixing "shadow acne" problem
    if (world.hit(r, fp_type(0.001), std::numeric_limits<fp_type>::infinity(), record)) {
        rt::ray<fp_type> scattered;
        rt::vec3<fp_type> attenuation;

        if (record.material_ptr->scatter(r, record, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1, ray_count);

        return rt::vec3<fp_type>(0);
    }

    auto unit_direction = rt::unit_vector(r.direction);
    fp_type t = fp_type(0.5) * (unit_direction.getY() + 1);

    //return (1 - t) * rt::vec3<fp_type>(1, 1, 1) + t * rt::vec3<fp_type>(0.5, 0.7, 1.0);

    return rt::lerp(rt::vec3<fp_type>(1), rt::vec3<fp_type>(0.5, 0.7, 1.0), t);
}


// TODO: random generator is not thread safe
void render(int shift, uint8_t* __restrict img, rt::hittable_list<fp_type>& world, rt::camera<fp_type>& cam, std::atomic<int>& ray_count)
{
    //int index = 1;
    int ray_count_t = 0;
    uint8_t* img_ptr = img + (shift * g_Channels);

    for (int j = 0; j < g_ImageHeight; ++j) {
        for (int i = shift; i < g_ImageWidth; i+=g_NumThreads) {
            rt::vec3<fp_type> color(0, 0, 0);

            for (int s = 0; s < g_SamplesPerPixel; ++s) {
                fp_type v = fp_type(j + rt::s_random_gen()) / g_ImageHeight;
                fp_type u = fp_type(i + rt::s_random_gen()) / g_ImageWidth;

                auto r = cam.get_ray(u, v);
                color += ray_color(r, world, g_MaxDepth, ray_count_t);
            }

            color /= g_SamplesPerPixel;

            auto final_color = rt::vector_sqrt(color) * static_cast<fp_type>(255.999);
            img_ptr[0] = final_color.getX();
            img_ptr[1] = final_color.getY();
            img_ptr[2] = final_color.getZ();
            img_ptr += g_NumThreads * g_Channels;

            /*++index;
            if (index == 400 + shift * 10) {
                index = 0;
                g_Info.update_thread(shift);
            }*/
        }
    }

    ray_count.fetch_add(ray_count_t, std::memory_order::memory_order_relaxed);
}

//void render(int shift, rt::vec3<uint8_t>* img, rt::hittable_list<fp_type>& world, rt::camera<fp_type>& cam, std::atomic<int>& ray_count)
//{
//    //int index = 1;
//    int ray_count_t = 0;
//
//    for (int f = 0; f < 100; ++f) {
//        rt::vec3<uint8_t>* img_ptr = img + shift;
//        const fp_type lerpFac = fp_type(f) / (f + 1);
//
//        for (int j = 0; j < g_ImageHeight; ++j) {
//            for (int i = shift; i < g_ImageWidth; i += g_NumThreads) {
//                rt::vec3<fp_type> color(0, 0, 0);
//
//                for (int s = 0; s < g_SamplesPerPixel; ++s) {
//                    fp_type v = fp_type(j + rt::s_random_gen()) / g_ImageHeight;
//                    fp_type u = fp_type(i + rt::s_random_gen()) / g_ImageWidth;
//
//                    auto r = cam.get_ray(u, v);
//                    color += ray_color(r, world, g_MaxDepth, ray_count_t);
//                }
//
//                color /= fp_type(g_SamplesPerPixel);
//                color = rt::vector_sqrt(color);// *static_cast<fp_type>(255.999);
//
//                rt::vec3<fp_type> prev(img_ptr->x, img_ptr->y, img_ptr->z);
//                prev /= static_cast<fp_type>(255.999);
//                color = (prev * lerpFac + color * (1 - lerpFac)) * static_cast<fp_type>(255.999);
//
//                *img_ptr = color;
//                img_ptr += g_NumThreads;
//
//                /*++index;
//                if (index == 400 + shift * 10) {
//                    index = 0;
//                    g_Info.update_thread(shift);
//                }*/
//            }
//        }
//    }
//
//    ray_count.fetch_add(ray_count_t, std::memory_order::memory_order_relaxed);
//}

int _cdecl main()
{
    auto look_from = rt::vec3<fp_type>(13.0, 2.0, 3.0);
    auto look_at = rt::vec3<fp_type>(0.0, 0.0, 0.0);
    auto up = rt::vec3<fp_type>(0.0, 1.0, 0.0);
    const auto dist_to_focus = static_cast<fp_type>(10.0);
    const auto aperture = static_cast<fp_type>(0.0);
    const auto aspect_ratio = fp_type(g_ImageWidth) / g_ImageHeight;

    rt::camera<fp_type> cam(look_from, look_at, up,
                            20.0, aspect_ratio,
                            aperture, dist_to_focus);

    auto world = random_scene();

    auto* img = new uint8_t[g_ImageHeight * g_ImageWidth * g_Channels];

    std::atomic<int> ray_count{ 0 };

    std::cout << "Pixels: " << g_ImageHeight * g_ImageWidth << std::endl;

    std::vector<std::thread> threads;

    auto start_t = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < g_NumThreads; ++i)
        threads.emplace_back(render, i, img, world, cam, std::ref(ray_count));
    for (auto& thread : threads)
        thread.join();

    auto end_t = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count();

    std::cout << "Rays: " << ray_count;
    std::cout << "\nTime: " << time << "ms\n";
    std::cout << "Rays\\s: " << (double(ray_count) / time * 1000);

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