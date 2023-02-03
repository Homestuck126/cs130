// Student Name: Aaron Tam
// Student ID: 862241987
#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the plane.  The plane's normal points outside.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    Hit temp;

    double number = dot(ray.direction , normal);
    if(number == 0)
    {
        return temp;
    }
    double t = (dot((x -ray.endpoint), normal))/( dot(ray.direction , normal));

    if (t < small_t)
    {
        return temp;
    }
    temp.dist = t;
    return temp;
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}
