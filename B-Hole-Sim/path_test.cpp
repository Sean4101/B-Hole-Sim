#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>

// Constants
const double G = 2.6; // Gravitational constant
const double c = 1; // Speed of light
const double M = 1; // Mass of black hole
const double Rs = 2; // Schwartzchild radius (Not accurate)

struct Ray {
    cv::Vec2d pos;
    cv::Vec2d vel;
};

double magnitude(const cv::Vec2d& v) {
    return sqrt(v[0] * v[0] + v[1] * v[1]);
}

void update_ray(Ray& ray, double dt) {
    double r = magnitude(ray.pos);
    cv::Vec2d accel = (-ray.pos / r) * (G * M / (r * r));
    ray.vel += accel * dt;
    ray.vel = ray.vel / magnitude(ray.vel) * c;

    ray.pos += ray.vel * dt;
}

void update_rays(std::vector<Ray>& rays, double dt) {
    for (int i = 0; i < rays.size(); ++i) {
        update_ray(rays[i], dt);
    }
}

int main() {
    // Convert Schwarzchild radius to pixels (use a scaling factor, e.g., 1 meter = 100 pixels)
    double scale_factor = 4e1;
    int radius_in_pixels = static_cast<int>(Rs * scale_factor);

    // Create a dark blue image
    int img_size = 800;
    cv::Mat img(img_size, img_size, CV_8UC3, cv::Scalar(139, 30, 30));

    // Draw a black circle representing the black hole
    cv::Point2d center(img_size / 2, img_size / 2);
    cv::circle(img, center, radius_in_pixels, cv::Scalar(0, 0, 0), -1); // Use -1 as thickness for a filled circle

    // Set up light rays
    int n_rays = 50;
    std::vector<Ray> rays;
    for (int i = 0; i < n_rays; ++i) {
        Ray _ray;
        double y_pos = 0.10022 * Rs * -i;
        _ray.pos = cv::Vec2d(6 * Rs, y_pos);
        _ray.vel = cv::Vec2d(-c, 0);
        rays.push_back(_ray);
    }

    // Draw rays
    double dt = 0.1; // Time step
    int total_steps = 1000;
    int image_update_steps = 20;

    for (int k = 0; k < total_steps; ++k) {
        update_rays(rays, dt);
        for (int i = 0; i < n_rays; ++i) {
            cv::Point2d offset(rays[i].pos[0], rays[i].pos[1]);
            cv::circle(img, center + offset * scale_factor, 1, cv::Scalar(255, 255, 255), -1);
        }
        if (k % image_update_steps == 0) {
            cv::imshow("Black Hole", img);
            cv::imwrite("images/path_test_" + std::to_string(k) + ".png", img);
            cv::waitKey(1);
        }
    }

    // Show the image
    cv::imshow("Black Hole", img);

    // Wait for a key press and close the window
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}