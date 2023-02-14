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
    Debug_Scope scope;
                if(recursion_depth == render_world.recursion_depth_limit)
    {
        return vec3(0,0,0);
    }
    vec3 color;
    double n1;
    double n2;
    bool enter = true;
    double R0;
     double R;
     double entertemp;
     vec3 b;
     vec3 t;
    vec3 direction = ray.direction;
    vec3 viewVector = -direction.normalized();
    double cos1 = dot(viewVector,normal);;
    if(dot(direction, normal) < 0 )
    {
    n1 = 1;
    n2 = index_of_refraction;
    R0 = pow((n1-n2)/(n1+n2),2);
    R = R0 + (1-R0)* pow((1-cos1),5);
    }
    else
    {
        n1 = index_of_refraction;
        n2 = 1;
        double power = pow(n1/n2,2);
         entertemp = dot(viewVector,normal);
        vec3 t = (n1/n2) * direction + ((n1/n2) * entertemp - sqrt(power * pow(entertemp,2) - (power -1)))*normal;
        entertemp = dot (t, -normal);
        R0 = pow((n1-n2)/(n1+n2),2);
        R = R0 + (1-R0)* pow((1-entertemp),5);
        enter = false;
    }

    double sin1 = sqrt(1 - (cos1 * cos1) );
    double sin2 = n1/n2 * sin1;
    double cos2 = sqrt(1-(n1*n1/(n2*n2)) * (1- (cos1 * cos1))) ;
if(enter)
{
    b= (viewVector - cos1 * normal) / sin1;
    t= -cos2 * normal + -sin2 * b;
}
else
{
     b = (viewVector - cos1 * normal) / sin1;
     t = cos2 * normal + -sin2 * b;
}


    vec3 c0 = shader->Shade_Surface (render_world, ray, hit, intersection_point, normal, recursion_depth);


    Ray transparentRay(intersection_point, t);

    vec3 reflectVector = 2 * dot (viewVector, normal) * normal - viewVector;
    Ray reflectionRay(intersection_point, reflectVector);

    vec3 cr = render_world.Cast_Ray(reflectionRay,recursion_depth++);
    Pixel_Print("reflected ray; ray: ",reflectionRay,"refelected color: ", cr);


   if (cos2<=1)
    {

    }
    else{
        Pixel_Print("complete internal reflection");
        return cr;
    }
    
    vec3 ct = render_world.Cast_Ray(transparentRay,recursion_depth++);
    color = opacity * c0 + (1-opacity) * R * cr + (1-opacity)*(1-R) * ct;
        Pixel_Print("transmitted ray; ray: ",transparentRay,"transmitted color: ", ct ,"; Schlick reflectivity: ", R);

    Pixel_Print("final color " , color);
    
    return color;
}
