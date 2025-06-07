#ifndef MT_PLANE_HPP_
#define MT_PLANE_HPP_

#include <algorithm>
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "mtdefs.hpp"

namespace mt {

    inline f32 snap_full(f32 x, u32 n, f32 inv_n) { return std::round(x * n) * inv_n; }

#define snap(x, n)   snap_full(x, n, 1.f / n)

#define snapv4(v, n) glm::vec4(snap(v.x, n), snap(v.y, n), v.z, v.w)

    class Plane {
      public:

        virtual bool inside(const glm::vec4& p) const = 0;

        glm::vec4    line_intersection(const glm::vec4& p0, const glm::vec4& p1) const {
            f32 t = line_intersection_scalar(p0, p1);
            return p0 + std::clamp(t, 0.f, 1.f) * (p1 - p0);
        }

      protected:

        virtual f32 line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const = 0;
    };

    using frustum_t = std::vector<Plane*>;

    class GeneralPlane : public Plane {
      public:

        GeneralPlane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);

        GeneralPlane(const glm::vec3& p, const glm::vec3& n);

        bool inside(const glm::vec4& p) const override;

      private:

        f32 line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const override;

      private:

        glm::vec3 m_normal;
        f32       m_d;
    };

    class ClipPlaneNear : public Plane {
      public:

        bool inside(const glm::vec4& p) const override { return -p.w <= p.z; }

      private:

        f32 line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const override {
            f32 wpz = p0.w + p0.z;
            return wpz / (wpz - p1.w - p1.z);
        }
    };

    class ClipPlaneFar : public Plane {
      public:

        bool inside(const glm::vec4& p) const override { return p.w >= p.z; }

      private:

        f32 line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const override {
            f32 wmz = p0.w - p0.z;
            return wmz / (wmz - p1.w + p1.z);
        }
    };

    class ClipPlaneLeft : public Plane {
      public:

        bool inside(const glm::vec4& p) const override { return -p.w <= p.x; }

        f32  line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const override {
            f32 wpx = p0.w + p0.x;
            return wpx / (wpx - p1.w - p1.x);
        }
    };

    class ClipPlaneRight : public Plane {
      public:

        bool inside(const glm::vec4& p) const override { return p.w >= p.x; }

      private:

        f32 line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const override {
            f32 wmx = p0.w - p0.x;
            return wmx / (wmx - p1.w + p1.x);
        }
    };

    class ClipPlaneDown : public Plane {
      public:

        bool inside(const glm::vec4& p) const override { return -p.w <= p.y; }

      private:

        f32 line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const override {
            f32 wpy = p0.w + p0.y;
            return wpy / (wpy - p1.w - p1.y);
        }
    };

    class ClipPlaneUp : public Plane {
      public:

        bool inside(const glm::vec4& p) const override { return p.w >= p.y; }

      private:

        f32 line_intersection_scalar(const glm::vec4& p0, const glm::vec4& p1) const override {
            f32 wmy = p0.w - p0.y;
            return wmy / (wmy - p1.w + p1.y);
        }
    };

} // namespace mt

#endif
