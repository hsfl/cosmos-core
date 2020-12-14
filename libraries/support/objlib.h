#ifndef OBJLIB_H
#define OBJLIB_H

#include "support/configCosmos.h"
#include "math/mathlib.h"
#include "math/vector.h"
using namespace Cosmos::Math::Vectors;
using namespace Cosmos::Math::Quaternions;

//* Maximum number of elements in cosmosstruc vectors
//* TEMPORARY! Make sure to replace, SCOTTNOTE
#define MAX_NUMBER_VEC_ELEM_TEMP 10


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

            // Convert class contents to JSON object
            json11::Json to_json() const {
                return json11::Json::object {
                    { "v"  , static_cast<double>(v) },
                    { "vt" , static_cast<double>(vt) },
                    { "vn" , static_cast<double>(vn) }
                };
            }

            // Set class contents from JSON string
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["v"].is_null()) v = static_cast<size_t>(p["v"].number_value());
                    if(!p["vt"].is_null()) vt = static_cast<size_t>(p["vt"].number_value());
                    if(!p["vn"].is_null()) vn = static_cast<size_t>(p["vn"].number_value());
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        struct point
        {
            vector <size_t> groups;
            size_t vertex = 0;

            point() {
                groups.reserve(MAX_NUMBER_VEC_ELEM_TEMP);
            }

            // Convert class contents to JSON object
            json11::Json to_json() const {
                vector<double> d_groups(groups.begin(), groups.end());
                return json11::Json::object {
                    { "groups" , d_groups },
                    { "vertex" , static_cast<double>(vertex) }
                };
            }

            // Set class contents from JSON string
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    for(size_t i = 0; i < groups.size(); ++i)	{
                        if(!p["groups"][i].is_null())
                            groups[i] = static_cast<size_t>(p["groups"][i].number_value());
                    }
                    if(!p["vertex"].is_null()) vertex = static_cast<size_t>(p["vertex"].number_value());
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        struct line
        {
            vector <size_t> groups;
            vector <vertex> vertices;
            Vector centroid;
            double length = 0.;

            line() {
                groups.reserve(MAX_NUMBER_VEC_ELEM_TEMP);
                vertices.reserve(MAX_NUMBER_VEC_ELEM_TEMP);
            }

            // Convert class contents to JSON object
            json11::Json to_json() const {
                vector<double> d_groups(groups.begin(), groups.end());
                return json11::Json::object {
                    { "groups" , d_groups },
                    { "vertices" , vertices },
                    { "centroid" , centroid },
                    { "length"   , length }
                };
            }

            // Set class contents from JSON string
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    for(size_t i = 0; i < groups.size(); ++i)	{
                        if(!p["groups"][i].is_null())
                            groups[i] = static_cast<size_t>(p["groups"][i].number_value());
                    }
                    for(size_t i = 0; i < vertices.size(); ++i)	{
                        if(!p["vertices"][i].is_null())	vertices[i].from_json(p["vertices"][i].dump());
                    }
                    if(!p["centroid"].is_null()) centroid.from_json(p["centroid"].dump());
                    if(!p["length"].is_null()) length = p["length"].number_value();
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        struct face
        {
            vector <size_t> groups;
            vector <vertex> vertices;
            Vector com;
            Vector normal;
            double area = 0.;

            face() {
                groups.reserve(MAX_NUMBER_VEC_ELEM_TEMP);
                vertices.reserve(MAX_NUMBER_VEC_ELEM_TEMP);
            }

            // Convert class contents to JSON object
            json11::Json to_json() const {
                vector<double> d_groups(groups.begin(), groups.end());
                return json11::Json::object {
                    { "groups" , d_groups },
                    { "vertices" , vertices },
                    { "com"    , com },
                    { "normal" , normal },
                    { "area"   , area }
                };
            }

            // Set class contents from JSON string
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    for(size_t i = 0; i < groups.size(); ++i)	{
                        if(!p["groups"][i].is_null())
                            groups[i] = static_cast<size_t>(p["groups"][i].number_value());
                    }
                    for(size_t i = 0; i < vertices.size(); ++i)	{
                        if(!p["vertices"][i].is_null())	vertices[i].from_json(p["vertices"][i].dump());
                    }
                    if(!p["com"].is_null()) com.from_json(p["com"].dump());
                    if(!p["normal"].is_null()) normal.from_json(p["normal"].dump());
                    if(!p["area"].is_null()) area = p["area"].number_value();
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        struct group
        {
            string name = "";
            size_t materialidx = 0;
            vector <size_t> pointidx;
            vector <size_t> lineidx;
            vector <size_t> faceidx;
            Vector com;
            double volume = 0;

            group() {
                pointidx.reserve(MAX_NUMBER_VEC_ELEM_TEMP);
                lineidx.reserve(MAX_NUMBER_VEC_ELEM_TEMP);
                faceidx.reserve(MAX_NUMBER_VEC_ELEM_TEMP);
            }

            // Convert class contents to JSON object
            json11::Json to_json() const {
                vector<double> d_pointidx(pointidx.begin(), pointidx.end());
                vector<double> d_lineidx(lineidx.begin(), lineidx.end());
                vector<double> d_faceidx(faceidx.begin(), faceidx.end());
                return json11::Json::object {
                    { "name"  , name },
                    { "materialidx" , static_cast<double>(materialidx) },
                    { "pointidx" , d_pointidx },
                    { "lineidx"  , d_lineidx },
                    { "faceidx"  , d_faceidx },
                    { "com"   , com },
                    { "volume", volume }
                };
            }

            // Set class contents from JSON string
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["name"].is_null()) name = p["name"].string_value();
                    if(!p["materialidx"].is_null()) materialidx = static_cast<size_t>(p["materialidx"].number_value());
                    for(size_t i = 0; i < pointidx.size(); ++i)	{
                        if(!p["pointidx"][i].is_null())
                            pointidx[i] = static_cast<size_t>(p["pointidx"][i].number_value());
                    }
                    for(size_t i = 0; i < lineidx.size(); ++i)	{
                        if(!p["lineidx"][i].is_null())
                            lineidx[i] = static_cast<size_t>(p["lineidx"][i].number_value());
                    }
                    for(size_t i = 0; i < faceidx.size(); ++i)	{
                        if(!p["faceidx"][i].is_null())
                            faceidx[i] = static_cast<size_t>(p["faceidx"][i].number_value());
                    }
                    if(!p["com"].is_null()) com.from_json(p["com"].dump());
                    if(!p["volume"].is_null()) volume = p["volume"].number_value();
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        struct material
        {
            string name = "";
            float density = 0.f;
            Vector ambient;
            Vector diffuse;
            Vector specular;

            // Convert class contents to JSON object
            json11::Json to_json() const {
                return json11::Json::object {
                    { "name" , name },
                    { "density" , density },
                    { "ambient" , ambient },
                    { "diffuse" , diffuse },
                    { "specular", specular }
                };
            }

            // Set class contents from JSON string
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["name"].is_null()) name = p["name"].string_value();
                    if(!p["density"].is_null()) density = p["density"].number_value();
                    if(!p["ambient"].is_null()) ambient.from_json(p["ambient"].dump());
                    if(!p["diffuse"].is_null()) diffuse.from_json(p["diffuse"].dump());
                    if(!p["specular"].is_null()) specular.from_json(p["specular"].dump());
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
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
        void add_3u(string basename, Vector offset);
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
        string name = "";

        // Convert class contents to JSON object
        json11::Json to_json() const {
            return json11::Json::object {
                { "Vg" , Vg },
                { "Vt" , Vt },
                { "Vn" , Vn },
                { "Vp" , Vp },
                { "Materials" , Materials },
                { "Points" , Points },
                { "Lines"  , Lines },
                { "Faces"  , Faces },
                { "Groups" , Groups },
                { "name"   , name }
            };
        }

        // Set class contents from JSON string
        void from_json(const string& s) {
            string error;
            json11::Json p = json11::Json::parse(s,error);
            if(error.empty()) {
                for(size_t i = 0; i < Vg.size(); ++i)	{
                    if(!p["Vg"][i].is_null())
                        Vg[i].from_json(p["Vg"][i].dump());
                }
                for(size_t i = 0; i < Vt.size(); ++i)	{
                    if(!p["Vt"][i].is_null())
                        Vt[i].from_json(p["Vt"][i].dump());
                }
                for(size_t i = 0; i < Vn.size(); ++i)	{
                    if(!p["Vn"][i].is_null())
                        Vn[i].from_json(p["Vn"][i].dump());
                }
                for(size_t i = 0; i < Materials.size(); ++i)	{
                    if(!p["Materials"][i].is_null())
                        Materials[i].from_json(p["Materials"][i].dump());
                }

                for(size_t i = 0; i < Points.size(); ++i)	{
                    if(!p["Points"][i].is_null())
                        Points[i].from_json(p["Points"][i].dump());
                }
                for(size_t i = 0; i < Lines.size(); ++i)	{
                    if(!p["Lines"][i].is_null())
                        Lines[i].from_json(p["Lines"][i].dump());
                }
                for(size_t i = 0; i < Faces.size(); ++i)	{
                    if(!p["Faces"][i].is_null())
                        Faces[i].from_json(p["Faces"][i].dump());
                }
                for(size_t i = 0; i < Groups.size(); ++i)	{
                    if(!p["Groups"][i].is_null())
                        Groups[i].from_json(p["Groups"][i].dump());
                }
                if(!p["name"].is_null()) name = p["name"].string_value();
            } else {
                cerr<<"ERROR: <"<<error<<">"<<endl;
            }
            return;
        }

    private:
        vector <size_t> cGroups;
        void parseobj(string input);
        void parsemtl(string input);
        string location = "";
    };
}

#endif // OBJLIB_H
