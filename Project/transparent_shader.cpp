#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 direction = ray.direction;
    vec3 viewVector = -direction.normalized();
    double cos1 = dot(viewVector,normal);
    double sin1 = sqrt(1 - cos1 * cos1 );
    double sin2 = 1/index_of_refraction * sin1;
    double cos2 = sqrt(1-1/(index_of_refraction*index_of_refraction) * (1- cos1 * cos1)) ;


    vec3 b = viewVector - cos1 * normal / sin1;
    vec3 t = cos2 * -normal + sin2 * -b;
    Ray transparentRay(intersection_point, t);

    vec3 reflectVector = 2 * dot (viewVector, normal) * normal - viewVector;
    Ray reflectionRay(intersection_point, reflectVector);

    vec3 c0 = shader->Shade_Surface (render_world, ray, hit, intersection_point, normal, recursion_depth);
    vec3 cr = render_world.Cast_Ray(reflectionRay,recursion_depth--);
    vec3 ct = render_world.Cast_Ray(transparentRay,recursion_depth--);

    double R0 = pow(1-index_of_refraction/(1+index_of_refraction),2);
    double R = R0 + (1-R0)* pow((1-cos1),5);
    vec3 color = opacity * c0 + (1-opacity) * R * cr + (1-opacity)*(1-R) * ct;
    return color;
}
