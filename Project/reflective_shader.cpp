#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    
    Debug_Scope scope;
    vec3 viewVector = -ray.direction.normalized();
    //refelect = 2 (v dot n) * n - v
    vec3 reflectVector = 2 * dot (viewVector, normal) * normal - viewVector;
    Ray reflectionRay(intersection_point, reflectVector);
    Pixel_Print("call Shade_Surface with location, ", hit, "normal: ",normal);
    vec3 c0 = ((1-reflectivity) * shader->Shade_Surface (render_world, reflectionRay, hit, intersection_point, normal, recursion_depth+1));

    if(recursion_depth < render_world.recursion_depth_limit)
    {
        Pixel_Print("reflected ray; ray: ",reflectionRay,"refelected color: ", c0);
       c0 = c0+ reflectivity*render_world.Cast_Ray(reflectionRay,recursion_depth+1);

    }
    
    return c0;

}
