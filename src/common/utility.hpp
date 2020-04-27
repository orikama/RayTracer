#pragma once

#include <cmath>
#include <limits>
#include <numbers>

#include "vec3.hpp"
#include "random_generator.hpp"


namespace rt
{

static thread_local rt::random_generator<double, std::minstd_rand> s_random_gen;





} // namespace rt
