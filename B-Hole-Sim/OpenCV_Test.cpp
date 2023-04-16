#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

const double G = 1.0;
const double c = 1.0;
const double M = 1.0;

void update_geodesics(double& t, double& r, double& theta, double& phi, double& dt, double& dr, double& dtheta, double& dphi, double E, double L, double time_step) {
    double r3 = r * r * r;

    // Calculate accelerations
    double ddt = 0;  // d²t/dλ² = 0
    double ddr = -(2 * M / r3) * E * E + L * L / r3 * (1 - 2 * M / r);
    double ddtheta = 0;  // d²θ/dλ² = 0
    double ddphi = 2 * M * E * L / (r3 * (r - 2 * M));

    // Update velocities
    dt += ddt * time_step;
    dr += ddr * time_step;
    dtheta += ddtheta * time_step;
    dphi += ddphi * time_step;

    // Update positions
    t += dt * time_step;
    r += dr * time_step;
    theta += dtheta * time_step;
    phi += dphi * time_step;
}

int main() {
    double t = 0, r = 10, theta = M_PI / 2, phi = 0;
    double dt = 1, dr = 0, dtheta = 0, dphi = 0.1;
    double E = 1, L = 4;
    double time_step = 0.01;
    int num_iterations = 1000;

    for (int i = 0; i < num_iterations; ++i) {
        update_geodesics(t, r, theta, phi, dt, dr, dtheta, dphi, E, L, time_step);
        std::cout << "Step " << i << ": t = " << t << ", r = " << r << ", theta = " << theta << ", phi = " << phi << std::endl;
    }

    return 0;
}