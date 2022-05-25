#ifndef VEC3_H
#define VEC3_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include <cmath>
#include <iostream>
#include <omp.h>

using std::sqrt;
using std::fabs;

extern thread_local unsigned int seed;

class vec3 {
    public:
        vec3() : e{0,0,0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(const double t) {
            return *this *= 1/t;
        }

        double length() const {
            return sqrt(length_squared());
        }

        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        bool near_zero() const {
            // Return true if the vector is close to zero in all dimensions.
            const auto s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        }

        inline static vec3 random() {
            auto thread_id = omp_get_thread_num();
            auto seedp = &seed;
            return vec3(random_double_r(seedp), random_double_r(seedp), random_double_r(seedp));
        }

        inline static vec3 random(double min, double max) {
            unsigned int *seedp = &seed;
            return vec3(random_double_r(min,max,seedp), random_double_r(min,max,seedp), random_double_r(min,max, seedp));
        }

        // construct vector from spherical coordinates
        // theta: azimuth (angle with +ve z-axis)
        // phi: polar (angle of projection on x-y plane with +ve x-axis)
        inline static vec3 from_spherical(double r, double theta, double phi) {
            auto sin_theta = sin(theta);
            auto cos_theta = cos(theta);
            auto cos_phi = cos(phi);
            auto sin_phi = sin(phi);
            auto r_sin_theta = r * sin_theta;
            auto x = r_sin_theta * cos_phi;
            auto y = r_sin_theta * sin_phi;
            auto z = r * cos_theta;
            return vec3(x, y, z);
        }

        // construct unit vector from spherical coordinates
        // theta: azimuth (angle with +ve z-axis)
        // phi: polar (angle of projection on x-y plane with +ve x-axis)
        inline static vec3 unit_from_spherical(double theta, double phi) {
            auto sin_theta = sin(theta);
            auto cos_theta = cos(theta);
            auto cos_phi = cos(phi);
            auto sin_phi = sin(phi);
            auto x = sin_theta * cos_phi;
            auto y = sin_theta * sin_phi;
            auto z = cos_theta;
            return vec3(x, y, z);
        }

    public:
        double e[3];
};


// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color


// vec3 Utility Functions

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline vec3 operator/(double t, vec3 v) {
    return vec3(t/v.e[0], t/v.e[1], t/v.e[2]);
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    auto thread_id = omp_get_thread_num();
    unsigned int *seedp = &seed;
    while (true) {
        auto p = vec3(random_double_r(-1,1, seedp), random_double_r(-1,1, seedp), 0);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        return p;
    }
}


// NOT REENTRANT
inline vec3 random_in_unit_sphere2() {
    auto theta = random_double(0, M_PI);
    auto phi = random_double(0, 2.0l*M_PI);
    return vec3::unit_from_spherical(theta, phi);
}


inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

inline vec3 random_in_hemisphere2(const vec3& normal) {

}


inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}


#endif
