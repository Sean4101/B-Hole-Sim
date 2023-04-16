#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>
#include <opencv2/core.hpp>

void write_color(cv::Vec3b& pixel, color pixel_color) {
    pixel[0] = static_cast<int>(255.999 * pixel_color.z());
    pixel[1] = static_cast<int>(255.999 * pixel_color.y());
    pixel[2] = static_cast<int>(255.999 * pixel_color.x());
}

#endif
