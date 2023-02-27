// Student Name: Aaron Tam
// Student ID: 862241987
#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"
//
extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    Debug_Scope scope; 

    double min_t = std::numeric_limits<double>::max();
    Shaded_Object object;
    Hit hit;
    std::string objname;
    for(auto o:objects){
        objname = o.object->name;
        Hit temp = o.object->Intersection(ray, -1);
        if(temp.Valid())
        Pixel_Print("intersection test with " ,o.object->name, "; hit: ",hit);
        else
        Pixel_Print("no intersection with " ,o.object->name);
        if((temp.dist >= small_t) && temp.dist < min_t) 
        {
            min_t = temp.dist;
            object = o;
            hit = temp;
        }

    }
    Pixel_Print("closest intersection; obj: " ,objname, "; hit: ",hit);
    return {object,hit};
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    vec3 dir = camera.World_Position(pixel_index) - camera.position;
    dir = dir.normalized();
    Ray ray(camera.position, dir);
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
        {
            Render_Pixel(ivec2(i,j));
        }


    
}

// Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,const vec3& intersection_point,const vec3& normal,int recursion_depth)

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    if(recursion_depth > recursion_depth_limit)
    {
        Hit temporary;
        if(background_shader)
        return background_shader->Shade_Surface(*this,ray,temporary,vec3(0,0,0),vec3(0,0,0),recursion_depth);
        else
        return vec3(0,0,0);
    }
    Debug_Scope scope; 
        vec3 color(0,0,0);
    Pixel_Print("cast ray; ray: ",ray);
    std::pair<Shaded_Object, Hit>  temp = Closest_Intersection(ray);
    if ( temp.second.Valid()) {
        vec3 point = ray.Point(temp.second.dist);
        Pixel_Print("call Shade_Surface with location, ", point, "normal: ",temp.first.object->Normal(ray,temp.second));
        color =  temp.first.shader->Shade_Surface(*this,ray, temp.second,point,temp.first.object->Normal(ray,temp.second),recursion_depth);
    }
    else if(background_shader)
    {
    Hit temp;
    color = background_shader->Shade_Surface(*this,ray,temp,vec3(0,0,0),vec3(0,0,0),recursion_depth);
    }
    return color;
}
