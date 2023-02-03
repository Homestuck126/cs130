// Student Name: Aaron Tam
// Student ID: 862241987
#include "sphere.h"
#include "ray.h"

Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
        Hit intersection;

    //from lecture
    //W = e-c 
    //u = ray direction
    //e = endpoint
    //c = center
    //T^2(u dot u ) + 2t ( wdot u) + (wdotw -r^2) = 0 
    vec3 w = ray.endpoint-center;
    double a = dot (ray.direction,ray.direction);
    double b = 2 * dot( w,ray.direction);
    double c = (dot(w,w) - (radius * radius));
    double discriminant = (b * b - (4*a*c));
    double sol1 =((-b + sqrt(discriminant))/(2*a ));
    double sol2 = ((-b -sqrt(discriminant))/(2*a));

    if(discriminant < 0 )
    {
        return intersection;
    }
    //determine which one is smaller
    if(sol1< sol2 && sol1>=small_t)
    {
        intersection.dist = sol1;
    }
    else if(sol1 > sol2 && sol2>=small_t)
    {
        intersection.dist = sol2;
    }
    return intersection;
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal;
    normal = vec3(1,1,1);
    return normal;
}

std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}
