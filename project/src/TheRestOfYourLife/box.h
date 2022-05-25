#ifndef BOX_H
#define BOX_H
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

#include "rtweekend.h"

#include "aarect.h"
#include "hittable_list.h"

class obox : public hittable  {
    public:
        obox() {}
        obox(const point3& p0, const point3& p1, material* ptr);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            output_box = aabb(box_min, box_max);
            return true;
        }

    public:
        point3 box_min;
        point3 box_max;
        hittable_list sides;
};


obox::obox(const point3& p0, const point3& p1, material* ptr) {
    box_min = p0;
    box_max = p1;

    sides.add(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides.add(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

    sides.add(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

    sides.add(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
}

bool obox::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return sides.hit(r, t_min, t_max, rec);
}



class box : public hittable  {
    public:
        box() {}
        box(const point3& p0, const point3& p1, material* ptr);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
            output_box = aabb(box_min, box_max);
            return true;
        }

    public:
        point3 box_min;
        point3 box_max;
        point3 box_center;
        point3 box_halfsize;
        point3 box_min_halfsize;
        material* mp;
};


box::box(const point3& p0, const point3& p1, material* ptr) {
    box_min = p0;
    box_max = p1;
    mp = ptr;
    box_center = (p0 + p1) / 2.0;
    box_halfsize = box_center - p0;
    box_min_halfsize = box_halfsize * -1;
}

bool box::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    point3 ro = r.origin() - box_center;
    vec3 rd = r.direction();
    vec3 m = 1.0 / rd;

    vec3 s = vec3(
      (rd.x()<0.0)?1.0:-1.0, 
      (rd.y()<0.0)?1.0:-1.0, 
      (rd.z()<0.0)?1.0:-1.0
    );

    vec3 t1 = m * (-ro + s * box_halfsize);
    vec3 t2 = m * (-ro - s * box_halfsize);

    float tN = fmax(fmax(t1.x(), t1.y()), t1.z());
    float tF = fmin(fmin(t2.x(), t2.y()), t2.z());
  
    if(tN > tF || tF < 0.0) return false;
  
    double t;
    double u, v;
    point3 outward_normal;
    if(t1.x() > t1.y() && t1.x() > t1.z()) { 
      outward_normal = vec3(s[0], 0, 0);
      t = t1.x();
      u = ro.y() + rd.y() * t;
      v = ro.z() + rd.z() * t;
      
    } else if(t1.y() > t1.z()) { 
      outward_normal = vec3(0, s[1], 0);
      t = t1.y();
      u = ro.z() + rd.z() * t;
      v = ro.x() + rd.x() * t;
      
    } else { 
      outward_normal = vec3(0, 0, s[2]); 
      t = t1.z();
      u = ro.x() + rd.x() * t;
      v = ro.y() + rd.y() * t;
    }
  
    // only search within bounds
    if (t < t_min || t > t_max) return false;
  
    rec.u = u;
    rec.v = v;
    rec.t = t;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mp;
    rec.p = r.at(t);  
    return true;
}


#endif
