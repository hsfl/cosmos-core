#include "support/objlib.h"
#include "support/datalib.h"

namespace Cosmos {

    wavefront::wavefront()
    {
        group tgroup;
        tgroup.name = "All";
        Groups.push_back(tgroup);
    }

    void wavefront::add_geometric_vertex(Math::Vector v)
    {
        Vg.push_back(v);
    }

    void wavefront::add_texture_vertex(Math::Vector v)
    {
        Vt.push_back(v);
    }

    void wavefront::add_normal_vertex(Math::Vector v)
    {
        Vn.push_back(v);
    }

    void wavefront::add_parameter_vertex(Math::Vector v)
    {
        Vp.push_back(v);
    }

    void wavefront::add_point(Cosmos::wavefront::point point)
    {
        // Add pointidx to Groups
        for (size_t i=0; i<cGroups.size(); ++i)
        {
            Groups[cGroups[i]].pointidx.push_back(Points.size());
        }

        point.groups = cGroups;
        Points.push_back(point);
    }

    void wavefront::add_line(Cosmos::wavefront::line line)
    {
        // Add lineidx to Groups
        for (size_t i=0; i<cGroups.size(); ++i)
        {
            Groups[cGroups[i]].lineidx.push_back(Lines.size());
        }

        line.groups = cGroups;
        Lines.push_back(line);
    }

    void wavefront::add_face(Cosmos::wavefront::face face)
    {
        // Add faceidx to Groups
        for (size_t i=0; i<cGroups.size(); ++i)
        {
            Groups[cGroups[i]].faceidx.push_back(Faces.size());
        }

        face.groups = cGroups;
        Faces.push_back(face);
    }

    void wavefront::add_groups(vector <string> groups)
    {
        cGroups.clear();
        cGroups.push_back(0);

        for (size_t k=0; k<groups.size(); ++k)
        {
            // Only add groups with COSMOS name format
            if (groups[k].find("cosmos_") == string::npos)
            {
                continue;
            }
            // Add group if not already present
            bool newgroup = true;
            for (size_t i=0; i<Groups.size(); ++i)
            {
                if (groups[k] == Groups[i].name)
                {
                    newgroup = false;
                    cGroups.push_back(i);
                    break;
                }
            }
            if (newgroup)
            {
                cGroups.push_back(Groups.size());
                group tgroup;
                tgroup.name = groups[k];
                Groups.push_back(tgroup);
            }
        }
    }

    void wavefront::parse(string input)
    {
        size_t index = 0;
        // Find command
        while (isspace(input[index]))
        {
            if (++index == input.size())
            {
                return;
            }
        }
        switch (input[index++])
        {
        case 'b':
            // bmat, bevel
            break;
        case 'c':
            // cstype, curv, curv2, con, c_interp, ctech
            break;
        case 'd':
            // deg, d_interp
            break;
        case 'e':
            // end
            break;
        case 'f':
            // f
            {
                face tface;
                do
                {
                    while (index < input.size() && isspace(input[index]))
                    {
                        ++index;
                    }
                    string tval;
                    while (index < input.size() && !isspace(input[index]))
                    {
                        tval.push_back(input[index++]);
                    }
                    vertex tvertex;
                    if (tval.size())
                    {
                        sscanf(tval.data(), "%lu/%lu/%lu", &tvertex.v, &tvertex.vt, &tvertex.vn);
                        tface.vertices.push_back(tvertex);
                    }
                } while (index < input.size());
                add_face(tface);
            }
            break;
        case 'g':
            // g
            {
                vector <string> tgroups;
                do
                {
                    while (index < input.size() && isspace(input[index]))
                    {
                        ++index;
                    }
                    string tgroup;
                    while (index < input.size() && !isspace(input[index]))
                    {
                        tgroup.push_back(input[index++]);
                    }
                    if (tgroup.size())
                    {
                        tgroups.push_back(tgroup);
                    }
                } while (index < input.size());
                add_groups(tgroups);
            }
            break;
        case 'h':
            //hole
            break;
        case 'l':
            // l, lod
            switch (input[index+1])
            {
            case 'o':
                // lod
                break;
            default:
                // l
                {
                    line tline;
                    do
                    {
                        while (index < input.size() && isspace(input[index]))
                        {
                            ++index;
                        }
                        string tval;
                        while (index < input.size() && !isspace(input[index]))
                        {
                            tval.push_back(input[index++]);
                        }
                        vertex tvertex;
                        if (tval.size())
                        {
                            sscanf(tval.data(), "%u/%u", &tvertex.v, &tvertex.vt);
                            tline.vertices.push_back(tvertex);
                        }
                    } while (index < input.size());
                    add_line(tline);
                }
                break;
            }
        case 'm':
            // mg, mtllib
            break;
        case 'o':
            // o
            while (index < input.size() && isspace(input[index]))
            {
                ++index;
            }
            name = input.substr(index, input.size());
            break;
        case 'p':
            // p, parm
            switch (input[index+1])
            {
            case 'a':
                // parm
                break;
            default:
                // p
                do
                {
                    while (index < input.size() && isspace(input[index]))
                    {
                        ++index;
                    }
                    string tval;
                    while (index < input.size() && !isspace(input[index]))
                    {
                        tval.push_back(input[index++]);
                    }
                    if (tval.size())
                    {
                        point tpoint;
                        sscanf(&input[index], "%u", &tpoint.vertex);
                        add_point(tpoint);
                    }
                } while (index < input.size());
            }
            break;
        case 's':
            // step, surf, scrv, sp, s, shadow_obj, stech
            break;
        case 't':
            // trim, trace_obj
            break;
        case 'u':
            // usemtl
            break;
        case 'v':
            // v, vt, vn, vp
            Vector tvec;
            int i = sscanf(&input[index], "%lf %lf %lf %lf", &tvec.x, &tvec.y, &tvec.z, &tvec.w);
            switch (input[index])
            {
            case 't':
                if (i == 1 || i == 2 || i == 3)
                {
                    Vt.push_back(tvec);
                }
                break;
            case 'n':
                if (i == 1 || i == 2 || i == 3)
                {
                    Vn.push_back(tvec);
                }
                break;
            case 'p':
                if (i == 1 || i == 2 || i == 3)
                {
                    Vp.push_back(tvec);
                }
                break;
            default:
                if (i == 3 || i == 4)
                {
                    tvec /= 1000.;
                    Vg.push_back(tvec);
                }
                break;
            }
            break;
        }
    }

    int32_t wavefront::load_file(string path)
    {

        if (!data_isfile(path))
        {
            return -errno;
        }

        Vg.clear();
        Vt.clear();
        Vn.clear();
        Vp.clear();
        Points.clear();
        Lines.clear();
        Faces.clear();
        Groups.clear();

        ifstream infile(path);
        string sinput;
        while (std::getline(infile, sinput))
        {
            parse(sinput);
        }

        // Calculate centroid for each line
        for (size_t i=0; i<Lines.size(); ++i)
        {
            Lines[i].centroid = Math::Vector();
            for (size_t j=0; j<Lines[i].vertices.size(); ++j)
            {
                Lines[i].centroid += Vg[Lines[i].vertices[j].v];
            }
            Lines[i].centroid /= Lines[i].vertices.size();
        }

        // Calculate length for each line
        for (size_t i=0; i<Lines.size(); ++i)
        {
            Lines[i].length = 0.;
            for (size_t j=1; j<Lines[i].vertices.size(); ++j)
            {
                Lines[i].length += (Vg[Lines[i].vertices[j].v] - Vg[Lines[i].vertices[j-1].v]).norm();
            }
        }

        // Calculate centroid and area for each face
        for (size_t i=0; i<Faces.size(); ++i)
        {
            Vector fcentroid = Math::Vector();
            for (size_t j=0; j<Faces[i].vertices.size(); ++j)
            {
                fcentroid += Vg[Faces[i].vertices[j].v];
            }
            fcentroid /= Faces[i].vertices.size();

            Faces[i].com = Math::Vector();
            Faces[i].area = 0.;
            Vector v1 = Vg[Faces[i].vertices[Faces[i].vertices.size()-1].v] - fcentroid;
            for (size_t j=0; j<Faces[i].vertices.size(); ++j)
            {
                Vector v2 = Vg[Faces[i].vertices[j].v] - fcentroid;
                // Area of triangle made by v1, v2 and Face centroid
                double tarea = v1.area(v2);
                // Sum
                Faces[i].area += tarea;
                // Centroid of triangle made by v1, v2 amd Face centroid
                Vector tcentroid = (v1 + v2 + fcentroid) / 3.;
                // Weighted sum
//                Vector test = tarea * tcentroid;
                Faces[i].com += tcentroid * tarea;
                v1 = v2;
            }
            // Divide by summed weights
            Faces[i].com /= Faces[i].area;
        }

        // Calculate center of mass for each Group using Points, Lines and Faces
        for (size_t i=0; i<Groups.size(); ++i)
        {
            Groups[i].com = Math::Vector();

            for (size_t j=0; j<Groups[i].pointidx.size(); ++j)
            {
                Groups[i].com += Vg[Points[Groups[i].pointidx[j]].vertex];
            }

            for (size_t j=0; j<Groups[i].lineidx.size(); ++j)
            {
                Groups[i].com += Lines[Groups[i].lineidx[j]].centroid;
            }

            for (size_t j=0; j<Groups[i].faceidx.size(); ++j)
            {
                Groups[i].com += Faces[Groups[i].faceidx[j]].com;
            }

            Groups[i].com /= (Groups[i].pointidx.size() + Groups[i].lineidx.size() + Groups[i].faceidx.size());
        }

        // Calculate volume for each Group using center of mass for each face
        for (size_t i=0; i<Groups.size(); ++i)
        {
            Groups[i].volume = 0.;
            for (size_t j=0; j<Groups[i].faceidx.size(); ++j)
            {
                Vector dv = Faces[Groups[i].faceidx[j]].com - Groups[i].com;
                Groups[i].volume += Faces[Groups[i].faceidx[j]].area * dv.norm() / 3.;
            }
        }

        return 0;
    }

    vector <string> wavefront::split(string str, char c)
    {
        vector <string> result;
        const char *strptr = str.data();

        do
        {
            const char *begin = strptr;

            while(*strptr != c && *strptr)
            {
                strptr++;
            }

            result.push_back(string(begin, strptr));
        } while (0 != *strptr++);

        return result;
    }

}
