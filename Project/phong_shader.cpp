#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color;
    vec3 diffusecomponenet;
    vec3 specularcomponent;
    vec3 tempcolor;
    vec2 temp;
    //find ambient light
    if(render_world.ambient_color == nullptr)
    {
        color = vec3(0,0,0);
    }
    else
    color = render_world.ambient_color->Get_Color(temp) * render_world.ambient_intensity * color_ambient->Get_Color(temp);
    Debug_Scope scope;
    Pixel_Print("ambient: ", color);
    //PhongLight = L_ambient*R_ambient + L_diffuse*R_diffuse* max(dot(normal,l),0) + R_specular* L_specular * pow(max(cos(phi),0),alpha) 

    for(unsigned i = 0; i< render_world.lights.size(); i++)
    {
        vec3 lightvector = render_world.lights[i]->position - intersection_point;
        vec3 reflectvector = ((2 * (dot (normal, lightvector)) * normal) - lightvector).normalized();

        std::pair<Shaded_Object, Hit>  temp;
        if(render_world.enable_shadows)
        {

		Ray Shadow_ray(intersection_point, lightvector);
		temp = render_world.Closest_Intersection(Shadow_ray);
		//if there is an object and the light source isnt in between your starting point and your object then return color without specular and diffuse.
		if(temp.first.object && temp.second.dist < lightvector.magnitude()){
            Pixel_Print("final color " , color);
			return color;
		}
        }

        diffusecomponenet = (render_world.lights[i]->Emitted_Light(lightvector) * color_diffuse->Get_Color(hit.uv) * std::max(dot(normal,lightvector.normalized()),0.0));
        specularcomponent = render_world.lights[i]->Emitted_Light(lightvector) * color_specular->Get_Color(hit.uv) * pow(std::max(dot(reflectvector,-ray.direction),0.0),specular_power);
        Pixel_Print("shading for light " , render_world.lights[i]->name, ": diffuse: ", diffusecomponenet, "; specular: ",specularcomponent);
    color = color + diffusecomponenet + specularcomponent;
	}


    Pixel_Print("final color " , color);
    return color;
}
