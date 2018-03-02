#ifndef OBJLIB_H
#define OBJLIB_H

#include "support/configCosmos.h"
#include "math/mathlib.h"
#include "math/vector.h"
using Cosmos::Math::Vector;


namespace Cosmos
{
    class wavefront
    {
    public:
        wavefront();

        struct vertex
        {
            size_t v;
            size_t vt;
            size_t vn;
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
            Math::Vector centroid;
            double length;
        };

        struct face
        {
            vector <size_t> groups;
            vector <vertex> vertices;
            Math::Vector com;
            double area;
        };

        struct group
        {
            string name;
            vector <size_t> pointidx;
            vector <size_t> lineidx;
            vector <size_t> faceidx;
            Math::Vector com;
            double volume;
        };

        void add_geometric_vertex(Math::Vector v);
        void add_texture_vertex(Math::Vector v);
        void add_normal_vertex(Math::Vector v);
        void add_parameter_vertex(Math::Vector v);
        void add_point(point point);
        void add_line(line line);
        void add_face(face face);
        void add_groups(vector <string> groups);
        int32_t load_file(string path);
        vector<string> split(string str, char c = ' ');

        vector <Math::Vector> Vg;
        vector <Math::Vector> Vt;
        vector <Math::Vector> Vn;
        vector <Math::Vector> Vp;

        vector <point> Points;
        vector <line> Lines;
        vector <face> Faces;
        vector <group> Groups;
        string name;

    private:
        vector <size_t> cGroups;
        void parse(string input);
    };
}

#endif // OBJLIB_H
