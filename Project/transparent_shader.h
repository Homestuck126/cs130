#ifndef __TRANSPARENT_SHADER_H__
#define __TRANSPARENT_SHADER_H__

#include <algorithm>
#include "shader.h"

class Transparent_Shader : public Shader
{
public:
    const Shader* shader = nullptr;
    //how much color pass through
    double opacity = 0;
    //how much it has been refracted. 
    //n0/n1 air to object. Invert for object to air. 
    double index_of_refraction = 1;

    Transparent_Shader(const Parse* parse,std::istream& in);

    // we assume that any intersection is between this material and air
    virtual vec3 Shade_Surface(const Render_World& render_world,const Ray& ray,
        const Hit& hit,const vec3& intersection_point,const vec3& normal,
        int recursion_depth) const override;
    
    static constexpr const char* parse_name = "transparent_shader";
};
#endif
