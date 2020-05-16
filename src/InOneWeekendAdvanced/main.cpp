#include <iostream>
#include <cstdint>
#include <limits>
#include <memory>

#include <thread>
#include <vector>
#include <mutex>


#include "common/vec3.hpp"
#include "common/ray.hpp"
#include "common/utility.hpp"
#include "common/camera.hpp"

#include "hittable_list.hpp"
#include "sphere.hpp"
#include "material.hpp"

#include "window.hpp"


using fp_type = float;

const int g_WindowWidth = 800;
const int g_WindowHeight = 400;
//const int g_Channels = 3;

const int g_SamplesPerPixel = 2;
const int g_MaxDepth = 40;

const int g_NumThreads = 4;



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
//        std::cout << pixels_ready << '/' << g_WindowWidth * g_WindowHeight;
//        std::cout.flush();
//    }
//
//private:
//    int pixels_ready;
//    std::mutex m;
//};

//safe_cout g_Info;

const int g_A = 4;
const int g_B = 4;

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
            rt::vec3<fp_type> center(a + 0.9 * random_gen(), 0.2, b + 0.9 * random_gen());

            if ((center - rt::vec3<fp_type>(4.0, 0.2, 0.0)).length() > 0.9) {
                // diffuse
                if (choose_material < 0.5) {
                    rt::vec3<fp_type> albedo = random_gen.random_vec3() * random_gen.random_vec3();
                    world.add(std::make_shared<rt::sphere<fp_type>>(center,
                                                                    0.2,
                                                                    std::make_shared<rt::lambertian<fp_type>>(albedo)));
                }
                // metal
                else if (choose_material < 0.75) {
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


rt::vec3<fp_type> ray_color(const rt::ray<fp_type>& r, const rt::hittable<fp_type>& world, int depth)
{
    if (depth <= 0)
        return rt::vec3<fp_type>(0, 0, 0);

    rt::hit_record<fp_type> record;

    // NOTE: 0.001 instead of 0.0, fixing "shadow acne" problem
    if (world.hit(r, 0.001, std::numeric_limits<fp_type>::infinity(), record)) {
        rt::ray<fp_type> scattered;
        rt::vec3<fp_type> attenuation;

        if (record.material_ptr->scatter(r, record, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);

        return rt::vec3<fp_type>(0, 0, 0);
    }

    auto unit_direction = rt::unit_vector(r.direction);
    fp_type t = fp_type(0.5) * (unit_direction.getY() + 1);

    return rt::lerp(rt::vec3<fp_type>(1), rt::vec3<fp_type>(0.5, 0.7, 1.0), t);
}


// TODO: random generator is not thread safe
void render(int shift, rt::hittable_list<fp_type>& world, rt::camera<fp_type>& cam,
            uint8_t* buffer, int rowStart, int rowEnd, int columnStart, int columnEnd,
            int frame_count)
{
    const fp_type lerpFac = fp_type(frame_count) / (frame_count + 1);

    //int index = 1;
    //for (int j = rowEnd - 1; j >= rowStart; --j) {
        //for (int i = shift; i < columnEnd; i += g_NumThreads) {
    for (int j = rowStart; j < rowEnd - 1; ++j) {
        for (int i = shift; i < columnEnd; i += g_NumThreads) {
            rt::vec3<fp_type> color(0, 0, 0);

            for (int s = 0; s < g_SamplesPerPixel; ++s) {
                fp_type v = fp_type(j + rt::s_random_gen()) / g_WindowHeight;
                fp_type u = fp_type(i + rt::s_random_gen()) / g_WindowWidth;

                auto r = cam.get_ray(u, v);
                color += ray_color(r, world, g_MaxDepth);
            }

            int index = (j * g_WindowWidth + i) * 4;

            color /= fp_type(g_SamplesPerPixel);
            color = rt::vector_sqrt(color);
            //color *= static_cast<fp_type>(255.999);

            rt::vec3<fp_type> prev(buffer[index + 2], buffer[index + 1], buffer[index]);
            prev /= static_cast<fp_type>(255.999);
            color = (prev * lerpFac + color * (1 - lerpFac)) * static_cast<fp_type>(255.999);

            /*buffer[index + 2] = color.x;
            buffer[index + 1] = color.y;
            buffer[index] = color.z;*/

            buffer[index + 2] = color.getX();
            buffer[index + 1] = color.getY();
            buffer[index] = color.getZ();

            /*++index;
            if (index == 400 + shift * 10) {
                index = 0;
                g_Info.update_thread(shift);
            }*/
        }
    }
}

auto look_from = rt::vec3<fp_type>(13.0, 2.0, 3.0);
auto look_at = rt::vec3<fp_type>(0.0, 0.0, 0.0);
auto up = rt::vec3<fp_type>(0.0, 1.0, 0.0);
const auto dist_to_focus = static_cast<fp_type>(10.0);
const auto aperture = static_cast<fp_type>(0.0);
const auto aspect_ratio = fp_type(g_WindowWidth) / g_WindowHeight;

rt::hittable_list<fp_type> g_world;
rt::camera<fp_type> g_cam(look_from, look_at, up,
                          20.0, aspect_ratio,
                          aperture, dist_to_focus);

void draw_callback(int width, int height, uint8_t* buffer, int frame_count)
{
    std::vector<std::thread> threads;

    for (int i = 0; i < g_NumThreads; ++i)
        threads.emplace_back(render, i, g_world, g_cam, buffer, 0, height, 0, width, frame_count);

    for (auto& thread : threads)
        thread.join();
}


int main()
{
    

    //g_cam = 

    g_world = random_scene();


    //stbi_flip_vertically_on_write(true);
    //stbi_write_png("image.png", g_ImageWidth, g_ImageHeight, g_Channels, img, g_ImageWidth * g_Channels);

    std::cout << "\nDone.\n";

    rt::window w("LOL", g_WindowWidth, g_WindowHeight, draw_callback);

    while (w.ShouldClose() == false) {
        w.PollEvents();
    }

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