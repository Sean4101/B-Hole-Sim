#include "color.h"
#include "ray.h"
#include "vec3.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>

const double G = 1; // Gravitational constant 
const double c = 1; // Speed of light
const double M = 1; // Mass of the object
const double Rs = 1; // Schwarzschild radius of the object  (Not accurate)
const point3 Pb(0, 0, -15); // Position of the object

const double dt = 0.1; // Light ray time step

bool hit_sphere(const point3& center, double radius, const ray& r) {
	vec3 oc = r.origin() - center;
	auto a = dot(r.direction(), r.direction());
	auto b = 2.0 * dot(oc, r.direction());
	auto c = dot(oc, oc) - radius * radius;
	auto discriminant = b * b - 4 * a * c;
	return (discriminant > 0);
}

color sample_hdri(cv::Mat hdri, const vec3& ray_dir, double phi_offset) {
	// Convert the ray vector to spherical coordinates
	float theta = std::acos(ray_dir.e[1]);
	float phi = std::atan2(ray_dir.e[2], ray_dir.e[0]);
	phi = (phi + phi_offset);
	if (phi > (2 * M_PI)) phi -= (2 * M_PI);

	// Map the spherical coordinates to 2D texture coordinates
	float u = phi / (2 * M_PI) + 0.5;
	float v = theta / M_PI;

	// Sample the HDRI image using bilinear interpolation
	cv::Point2f pt(u * hdri.cols, v * hdri.rows);
	cv::Vec3f _color = cv::Vec3f(0, 0, 0);
	cv::Vec3f tl, tr, bl, br;
	cv::Vec3f* data = hdri.ptr<cv::Vec3f>();
	int x = static_cast<int>(std::floor(pt.x));
	int y = static_cast<int>(std::floor(pt.y));
	float u_ratio = pt.x - x;
	float v_ratio = pt.y - y;
	float u_inv = 1 - u_ratio;
	float v_inv = 1 - v_ratio;
	tl = data[y * hdri.cols + x];
	tr = data[y * hdri.cols + x + 1];
	bl = data[(y + 1) * hdri.cols + x];
	br = data[(y + 1) * hdri.cols + x + 1];
	_color += tl * u_inv * v_inv;
	_color += tr * u_ratio * v_inv;
	_color += bl * u_inv * v_ratio;
	_color += br * u_ratio * v_ratio;

	return color(_color[2], _color[1], _color[0]);
}

color ray_color(cv::Mat hdri, const ray& received_ray, bool print, double phi_offset) {
	//if (hit_sphere(Pb, Rs, received_ray))
		//return color(0, 0, 0);
	//return sample_hdri(hdri, received_ray.direction(), phi_offset);

	
	vec3 pos = received_ray.orig;
	vec3 vel = received_ray.dir * c;

	while (true) {
		double r = (pos - Pb).length();
		if (r < Rs) return color(0, 0, 0);
		else if (r > 20 * Rs) return sample_hdri(hdri, vel / vel.length(), phi_offset);

		vec3 accel = (-(pos - Pb) / r) * (G * M / (r * r));
		vel += accel * dt;
		vel = vel / vel.length() * c;
		pos += vel * dt;
	}
}

int main1()
{
	// Image
	const int image_width = 800;
	const int image_height = 800;
	const double aspect_ratio = static_cast<double>(image_width) / static_cast<double>(image_height);
	cv::Mat output(image_height, image_width, CV_8UC3);

	// Camera
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto top_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

	// Scene
	cv::Mat hdri_bg = cv::imread("space-galaxy-background.hdr", cv::IMREAD_UNCHANGED);

	// Render
	int n_frames = 60;
	for (int n = 0; n < n_frames; ++n) {
		// Render for the n-th frame
		for (int j = image_height; j > 0; --j) {
			for (int i = image_width - 1; i >= 0; --i) {

				cv::Vec3b& pixel = output.at<cv::Vec3b>(image_height - j, i);

				auto u = double(i) / (double(image_width) - 1);
				auto v = double(j) / (double(image_height) - 1);
				ray r(origin, top_left_corner + u * horizontal + v * vertical - origin);

				bool print = (i == 1 && j == 127);
				color pixel_color = ray_color(hdri_bg, r, print, (2 * M_PI) / n_frames * n);

				write_color(pixel, pixel_color);
			}
			//std::cout << "Finished " << image_height - j + 1 << "/" << image_height << std::endl;
		}
		cv::imshow("Output", output);
		cv::imwrite("images/test_" + std::to_string(n) + ".png", output);
		std::cout << "Finished " << n + 1 << "/" << n_frames << std::endl;
		cv::waitKey(1);
	}

	cv::imshow("Display window", output);
	cv::waitKey(0);

	return 0;
}