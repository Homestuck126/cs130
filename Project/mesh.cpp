#include "mesh.h"
#include "plane.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse* parse, std::istream& in)
{
    std::string file;
    in>>name>>file;
    Read_Obj(file.c_str());
}

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }

        if(sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        if(sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
            for(int i=0;i<3;i++) t[i]--;
            triangle_texture_index.push_back(t);
        }
    }
    num_parts=triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
//If part>=0, intersect only against part of the
    // primitive.  This is only used for meshes, where part is the triangle
    // index.  If part<0, intersect against all triangles.  For other
    // primitives, part is ignored.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    Hit hit;
    if (part>=0)
    {
    hit = Intersect_Triangle(ray,part);
    }
    else
    {
        hit.dist = std::numeric_limits<int>::max();
        for(int i = 0; i<triangles.size(); i++)
        {
            Hit temp = Intersect_Triangle(ray,i);
            if(temp.dist > -1 && temp.dist<hit.dist)
            {
                hit.uv = temp.uv;
                hit.dist = temp.dist;
                hit.triangle = temp.triangle;
            }
        }
    }
    if(hit.dist<std::numeric_limits<int>::max())
    return hit;
    else
    {
        Hit temp;
        return temp;
    }
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    double temp = hit.triangle;
    //assert(temp>=0);
	vec3 A = vertices[triangles[temp][0]];
	vec3 B = vertices[triangles[temp][1]];
	vec3 C = vertices[triangles[temp][2]];
    vec3 B_A = B-A;
    vec3 C_A = C-A;
    vec3 norm = cross(B_A,C_A).normalized();
    return norm;
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    
    Hit hit;
    Debug_Scope scope; 

	vec3 A = vertices[triangles[tri][0]];
	vec3 B = vertices[triangles[tri][1]];
	vec3 C = vertices[triangles[tri][2]];
    //area of triangle is 1/2 || (B-A) X (C-A)||
    vec3 B_A = B-A;
    vec3 C_A = C-A;


    vec3 norm = cross(B_A,C_A).normalized();

    double area = .5 * dot( cross(B_A,C_A), norm);
    Plane p(A,norm);
	hit = p.Intersection(ray, tri);
    if(!hit.Valid()){
    return hit;
    }

if(hit.Valid())
{
    //f(x) = (X-A) dot n = 0
    vec3 intersectionpoint = ray.Point(hit.dist);
    double Alpha =.5 * dot(cross(B-intersectionpoint,C-intersectionpoint ),norm)/area;
    double Beta = .5 *dot(cross(intersectionpoint-A,C-A ),norm)/area;
    double Gamma = 1- Alpha - Beta;
    Pixel_Print("Mesh ",dot(cross(B-intersectionpoint,C-intersectionpoint ),norm), " triangle " , tri, " weights: (" ,Alpha, " ", Beta, " ", Gamma, "); dist ", hit.dist);
    if(Alpha >= -small_t && Beta >= -small_t && Gamma >= -small_t)
    {
        vec2 uA = uvs[triangle_texture_index[tri][0]];
        vec2 uB = uvs[triangle_texture_index[tri][1]];
        vec2 uC = uvs[triangle_texture_index[tri][2]];
        vec2 vA = uvs[triangle_texture_index[tri][0]];
        vec2 vB = uvs[triangle_texture_index[tri][1]];
        vec2 vC = uvs[triangle_texture_index[tri][2]];
        vec2 UV;
        UV[0] = Alpha * uA[0] + Beta * uB[0] + Gamma * uC[0];
        UV[1] = Alpha * vA[1] + Beta * vB[1] + Gamma * vC[1];
        hit.uv = UV;
        hit.triangle= tri;
    }
    else
    {
    Hit temp;
    return temp;
    }
}
else
{
    Hit temp;
    return temp;
}

    return hit;
}

std::pair<Box,bool> Mesh::Bounding_Box(int part) const
{
    if(part<0)
    {
        Box box;
        box.Make_Empty();
        for(const auto& v:vertices)
            box.Include_Point(v);
        return {box,false};
    }

    ivec3 e=triangles[part];
    vec3 A=vertices[e[0]];
    Box b={A,A};
    b.Include_Point(vertices[e[1]]);
    b.Include_Point(vertices[e[2]]);
    return {b,false};
}
