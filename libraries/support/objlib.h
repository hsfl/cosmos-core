#ifndef OBJLIB_H
#define OBJLIB_H

#include "support/configCosmos.h"
#include "math/mathlib.h"
#include "math/vector.h"
using namespace Cosmos::Math::Vectors;
using namespace Cosmos::Math::Quaternions;


namespace Cosmos
{
    class wavefront
    {
    public:
        wavefront();

        struct vertex
        {
            size_t v=0;
            size_t vt=0;
            size_t vn=0;
        };

        struct point
        {
            vector <size_t> groups;
            size_t vertex;
        };

        struct line
        {
            vector <size_t> groups;
            vector <vertex> vertices;
            Vector centroid;
            double length;
        };

        struct face
        {
            vector <size_t> groups;
            vector <vertex> vertices;
            Vector com;
            Vector normal;
            double area;
        };

        struct group
        {
            string name;
            size_t materialidx;
            vector <size_t> pointidx;
            vector <size_t> lineidx;
            vector <size_t> faceidx;
            Vector com;
            double volume;
        };

        struct material
        {
            string name;
            float density;
            Vector ambient;
            Vector diffuse;
            Vector specular;
        };

        size_t add_geometric_vertex(Vector v);
        size_t add_texture_vertex(Vector v);
        size_t add_normal_vertex(Vector v);
        size_t add_parameter_vertex(Vector v);
        void add_material(material material);
        void add_point(point point);
        void add_line(line line);
        void add_face(face face);
        void update_line(line &cline);
        void update_face(face &cface);
        void update_group(group &cgroup);
        void modify_groups(vector <string> groups);
        void modify_groups(string group);
        void add_cuboid(string name, Vector size, Quaternion orientation, Vector offset);
        void add_1u(string basename, Vector offset);
        int32_t load_file(string loc, string name);
        int32_t save_file(string loc, string name);
        void rebase();
        vector<string> split(string str, char c = ' ');

        vector <Vector> Vg;
        vector <Vector> Vt;
        vector <Vector> Vn;
        vector <Vector> Vp;

        vector <material> Materials;
        vector <point> Points;
        vector <line> Lines;
        vector <face> Faces;
        vector <group> Groups;
        string name;

    private:
        vector <size_t> cGroups;
        void parseobj(string input);
        void parsemtl(string input);
        string location;
    };
}

#endif // OBJLIB_H
