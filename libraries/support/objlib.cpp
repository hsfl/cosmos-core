#include "support/objlib.h"
#include "support/datalib.h"

namespace Cosmos {

    wavefront::wavefront()
    {
        Vg.clear();
        Vg.push_back(Vector());
        Vt.clear();
        Vt.push_back(Vector());
        Vn.clear();
        Vn.push_back(Vector());
        Vp.clear();
        Vp.push_back(Vector());
        Points.clear();
        Lines.clear();
        Faces.clear();

        Materials.clear();
        material tmaterial;
        tmaterial.name = "Any";
        tmaterial.density = 1.;
        tmaterial.ambient = {.5, .5, .5, 1.};
        tmaterial.diffuse = {.5, .5, .5, 1.};
        Materials.push_back(tmaterial);

        Groups.clear();
        group tgroup;
        tgroup.name = "All";
        Groups.push_back(tgroup);
    }

    size_t wavefront::add_geometric_vertex(Vector v)
    {
        Vg.push_back(v);
        return (Vg.size()-1);
    }

    size_t wavefront::add_texture_vertex(Vector  v)
    {
        Vt.push_back(v);
        return (Vt.size()-1);
    }

    size_t wavefront::add_normal_vertex(Vector  v)
    {
        Vn.push_back(v);
        return (Vn.size()-1);
    }

    size_t wavefront::add_parameter_vertex(Vector  v)
    {
        Vp.push_back(v);
        return (Vp.size()-1);
    }

    void wavefront::add_material(Cosmos::wavefront::material m)
    {
        Materials.push_back(m);
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
        update_face(face);
        Faces.push_back(face);
    }

    void wavefront::modify_groups(string group)
    {
        vector <string> groups;
        groups.push_back(group);
        modify_groups(groups);
    }

    void wavefront::modify_groups(vector <string> groups)
    {
        cGroups.clear();
        cGroups.push_back(0);

        for (size_t k=0; k<groups.size(); ++k)
        {
            // Only add groups with COSMOS name format
            if (groups[k].find("cosmos.") == string::npos)
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
                tgroup.name = groups[k].substr(7);
                tgroup.materialidx = 0;
                Groups.push_back(tgroup);
            }
        }
    }

    void wavefront::add_cuboid(string name, Vector size, Quaternion orientation, Vector offset)
    {
        modify_groups(name);
        face tface;
        vertex tvertex;
        Vector tvector;
        for (int16_t i=-1; i<2; i+=2)
        {
            // x
            tface.vertices.clear();
            tvector = unitxV(i);
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.vn = add_normal_vertex(tvector);

            tvector = {i*size.x/2., size.y/2., -i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {i*size.x/2., size.y/2., i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {i*size.x/2., -size.y/2., i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {i*size.x/2., -size.y/2., -i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            add_face(tface);

            // y
            tface.vertices.clear();
            tvector = unityV(i);
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.vn = add_normal_vertex(tvector);

            tvector = {size.x/2., i*size.y/2., i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {size.x/2., i*size.y/2., -i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {-size.x/2., i*size.y/2., -i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {-size.x/2., i*size.y/2., i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            add_face(tface);

            // z
            tface.vertices.clear();
            tvector = unitzV(i);
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.vn = add_normal_vertex(tvector);

            tvector = {size.x/2., size.y/2., i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {-i*size.x/2., i*size.y/2., i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {-size.x/2., -size.y/2., i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            tvector = {i*size.x/2., -i*size.y/2., i*size.z/2.};
            tvector = orientation.irotate(tvector);
            tvector += offset;
            tvertex.v = add_geometric_vertex(tvector);
            tface.vertices.push_back(tvertex);

            add_face(tface);
        }
    }

    void wavefront::add_1u(string basename, Vector offset)
    {
        string name;

        // Verticals
        name = "cosmos.none." + basename + "_z+x+y";
        add_cuboid(name, Vector(.003, .003, .1), eyeQ(), offset + Vector(.0485, .0485, 0.));
        name = "cosmos.none." + basename + "_z+x-y";
        add_cuboid(name, Vector(.003, .003, .1), eyeQ(), offset + Vector(.0485, -.0485, 0.));
        name = "cosmos.none." + basename + "_z-x-y";
        add_cuboid(name, Vector(.003, .003, .1), eyeQ(), offset + Vector(-.0485, -.0485, 0.));
        name = "cosmos.none." + basename + "_z-x+y";
        add_cuboid(name, Vector(.003, .003, .1), eyeQ(), offset + Vector(-.0485, .0485, 0.));

        // Horizontals
        name = "cosmos.none." + basename + "_+y+z";
        add_cuboid(name, Vector(.1, .003, .003), eyeQ(), offset + Vector(0., .0485, .0485));
        name = "cosmos.none." + basename + "_-y+z";
        add_cuboid(name, Vector(.1, .003, .003), eyeQ(), offset + Vector(0., -.0485, .0485));
        name = "cosmos.none." + basename + "_+x+z";
        add_cuboid(name, Vector(.003, .1, .003), eyeQ(), offset + Vector(.0485, 0., .0485));
        name = "cosmos.none." + basename + "_-x+z";
        add_cuboid(name, Vector(.003, .1, .003), eyeQ(), offset + Vector(-.0485, 0., .0485));

        name = "cosmos.none." + basename + "_+y-z";
        add_cuboid(name, Vector(.1, .003, .003), eyeQ(), offset + Vector(0., .0485, -.0485));
        name = "cosmos.none." + basename + "_-y-z";
        add_cuboid(name, Vector(.1, .003, .003), eyeQ(), offset + Vector(0., -.0485, -.0485));
        name = "cosmos.none." + basename + "_+x-z";
        add_cuboid(name, Vector(.003, .1, .003), eyeQ(), offset + Vector(.0485, 0., -.0485));
        name = "cosmos.none." + basename + "_-x-z";
        add_cuboid(name, Vector(.003, .1, .003), eyeQ(), offset + Vector(-.0485, 0., -.0485));

    }

    void wavefront::add_3u(string basename, Vector offset)
    {
        string name;

        // Verticals
        name = "cosmos.none." + basename + "_z+x+y";
        add_cuboid(name, Vector(.003, .003, .3), eyeQ(), offset + Vector(.0485, .0485, 0.));
        name = "cosmos.none." + basename + "_z+x-y";
        add_cuboid(name, Vector(.003, .003, .3), eyeQ(), offset + Vector(.0485, -.0485, 0.));
        name = "cosmos.none." + basename + "_z-x-y";
        add_cuboid(name, Vector(.003, .003, .3), eyeQ(), offset + Vector(-.0485, -.0485, 0.));
        name = "cosmos.none." + basename + "_z-x+y";
        add_cuboid(name, Vector(.003, .003, .3), eyeQ(), offset + Vector(-.0485, .0485, 0.));

        // Horizontals
        name = "cosmos.none." + basename + "_+y+z";
        add_cuboid(name, Vector(.1, .003, .003), eyeQ(), offset + Vector(0., .0485, .1485));
        name = "cosmos.none." + basename + "_-y+z";
        add_cuboid(name, Vector(.1, .003, .003), eyeQ(), offset + Vector(0., -.0485, .1485));
        name = "cosmos.none." + basename + "_+x+z";
        add_cuboid(name, Vector(.003, .1, .003), eyeQ(), offset + Vector(.0485, 0., .1485));
        name = "cosmos.none." + basename + "_-x+z";
        add_cuboid(name, Vector(.003, .1, .003), eyeQ(), offset + Vector(-.0485, 0., .1485));

        name = "cosmos.none." + basename + "_+y-z";
        add_cuboid(name, Vector(.1, .003, .003), eyeQ(), offset + Vector(0., .0485, -.1485));
        name = "cosmos.none." + basename + "_-y-z";
        add_cuboid(name, Vector(.1, .003, .003), eyeQ(), offset + Vector(0., -.0485, -.1485));
        name = "cosmos.none." + basename + "_+x-z";
        add_cuboid(name, Vector(.003, .1, .003), eyeQ(), offset + Vector(.0485, 0., -.1485));
        name = "cosmos.none." + basename + "_-x-z";
        add_cuboid(name, Vector(.003, .1, .003), eyeQ(), offset + Vector(-.0485, 0., -.1485));

    }

    void wavefront::parseobj(string input)
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
                        if (tval.find("///") != string::npos)
                        {
                            sscanf(tval.data(), "%lu///", &tvertex.v);
                        }
                        else if (tval.find("//") != string::npos)
                        {
                            sscanf(tval.data(), "%lu//%lu", &tvertex.v, &tvertex.vn);
                        }
                        else
                        {
                            sscanf(tval.data(), "%lu/%lu/%lu", &tvertex.v, &tvertex.vt, &tvertex.vn);
                        }

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
                modify_groups(tgroups);
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
                            sscanf(tval.data(), "%lu/%lu", &tvertex.v, &tvertex.vt);
                            tline.vertices.push_back(tvertex);
                        }
                    } while (index < input.size());
                    add_line(tline);
                }
                break;
            }
        case 'm':
            // mg, mtllib
            switch (input[index])
            {
            case 't':
                // mtllib
                string mtlname = input.substr(input.find(" ")+1);
                if (!data_isfile(location+"/"+mtlname))
                {
                    break;
                }
                ifstream mtlfile(location+"/"+mtlname);
                string sinput;
                while (std::getline(mtlfile, sinput))
                {
                    parsemtl(sinput);
                }
                mtlfile.close();
            }
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
                        sscanf(&input[index], "%lu", &tpoint.vertex);
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
            {
                string mtlname = input.substr(input.find(" ")+1);
                // Add materialidx to Groups
                for (size_t j=0; j<Materials.size(); ++j)
                {
                    if (Materials[j].name == mtlname)
                    {
                        for (size_t i=0; i<cGroups.size(); ++i)
                        {
                            Groups[cGroups[i]].materialidx = j;
                        }
                    }
                }
            }
            break;
        case 'v':
            // v, vt, vn, vp
            Vector tvec;
            switch (input[index])
            {
            case 't':
                {
                    ++index;
                    int i = sscanf(&input[index], "%lf %lf %lf", &tvec.x, &tvec.y, &tvec.z);
                    if (i == 1 || i == 2 || i == 3)
                    {
                        add_texture_vertex(tvec);
                    }
                }
                break;
            case 'n':
                {
                    ++index;
                    int i = sscanf(&input[index], "%lf %lf %lf", &tvec.x, &tvec.y, &tvec.z);
                    if (i == 1 || i == 2 || i == 3)
                    {
                        add_normal_vertex(tvec);
                    }
                }
                break;
            case 'p':
                {
                    ++index;
                    int i = sscanf(&input[index], "%lf %lf %lf", &tvec.x, &tvec.y, &tvec.z);
                    if (i == 1 || i == 2 || i == 3)
                    {
                        add_parameter_vertex(tvec);
                    }
                }
                break;
            default:
                {
                    int i = sscanf(&input[index], "%lf %lf %lf %lf", &tvec.x, &tvec.y, &tvec.z, &tvec.w);
                    if (i == 3 || i == 4)
                    {
                        //                        tvec /= 1000.;
                        add_geometric_vertex(tvec);
                    }
                }
                break;
            }
            break;
        }
    }

    void wavefront::parsemtl(string input)
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
        case 'd':
            {
                sscanf(&input[index], "%f", &Materials.back().density);
            }
            break;
        case 'n':
            {
                struct material tmaterial;
                tmaterial.name = input.substr(input.find(" ")+1);
                tmaterial.density = 1.;
                tmaterial.ambient = {.1,.1,.1};
                tmaterial.diffuse = {.9, .9, .9};
                add_material(tmaterial);
            }
            break;
        case 'K':
            Vector  tval;
            switch (input[index])
            {
            case 'a':
                {
                    ++index;
                    int i = sscanf(&input[index], "%lf %lf %lf", &tval[0], &tval[1], &tval[2]);
                    if (i == 1)
                    {
                        tval[1] = tval[0];
                        tval[2] = tval[0];
                    }
                    if (i)
                    {
                        Materials.back().ambient = tval;
                    }
                }
                break;
            case 'd':
                {
                    ++index;
                    int i = sscanf(&input[index], "%lf %lf %lf", &tval[0], &tval[1], &tval[2]);
                    if (i == 1)
                    {
                        tval[1] = tval[0];
                        tval[2] = tval[0];
                    }
                    if (i)
                    {
                        Materials.back().diffuse = tval;
                    }
                }
                break;
            case 's':
                {
                    ++index;
                    int i = sscanf(&input[index], "%lf %lf %lf", &tval[0], &tval[1], &tval[2]);
                    if (i == 1)
                    {
                        tval[1] = tval[0];
                        tval[2] = tval[0];
                    }
                    if (i)
                    {
                        Materials.back().specular = tval;
                    }
                }
                break;
            }
            break;
        }
    }

    void wavefront::rebase()
    {
        for (face &nextf : Faces)
        {
            update_face(nextf);
        }

        Vector tcom = Vector();
        double tvolume = 0.;
        for (group &nextg : Groups)
        {
            if (nextg.name == "All")
            {
                continue;
            }
            update_group(nextg);
            tvolume += nextg.volume;
            tcom +=  nextg.com * nextg.volume;
        }
        if (tvolume)
        {
            tcom /= tvolume;
        }

        // Recenter all vectors to total Center of Mass
        for (face &nextf : Faces)
        {
            nextf.com -= tcom;
            nextf.normal -= tcom;
        }

        for (group &nextg : Groups)
        {
            nextg.com -= tcom;
        }

        for (Vector &nextv : Vg)
        {
            nextv -= tcom;
        }

        for (Vector &nextv : Vn)
        {
            nextv -= tcom;
        }
    }

    int32_t wavefront::load_file(string loc, string name)
    {

        if (!data_isfile(loc+"/"+name))
        {
            return -errno;
        }
        location = loc;

        Vg.clear();
        Vg.push_back(Vector());
        Vt.clear();
        Vt.push_back(Vector());
        Vn.clear();
        Vn.push_back(Vector());
        Vp.clear();
        Vp.push_back(Vector());
        Points.clear();
        Lines.clear();
        Faces.clear();

        Materials.clear();
        material tmaterial;
        tmaterial.name = "Any";
        tmaterial.density = 1.;
        tmaterial.ambient = {.5, .5, .5, 1.};
        tmaterial.diffuse = {.5, .5, .5, 1.};
        Materials.push_back(tmaterial);

        Groups.clear();
        group tgroup;
        tgroup.name = "All";
        Groups.push_back(tgroup);

        ifstream infile(loc+"/"+name);
        string sinput;
        while (std::getline(infile, sinput))
        {
            parseobj(sinput);
        }
        infile.close();

        // Calculate centroid and length of each line
        for (line &nextl : Lines)
        {
            update_line(nextl);
        }

        // Calculate centroid and area for each face
        for (face &nextf : Faces)
        {
            update_face(nextf);
        }

        // Calculate center of mass and volume for each Group
        for (group &nextg : Groups)
        {
            update_group(nextg);
        }

        rebase();

        return 0;
    }

    int32_t wavefront::save_file(string loc, string name)
    {
        char outbuf[300];

        ofstream outfile(loc+"/"+name+".obj");
        if (!outfile.is_open())
        {
            return -errno;
        }

        rebase();

        outfile << "# Node Object Model" << endl;
        outfile << "# File units = meters" << endl;
        outfile << "mtllib material.mtl" << endl;
        outfile << endl;

        for (size_t i=1; i<Vg.size(); ++i)
        {
            sprintf(outbuf, "v %f %f %f", Vg[i].x, Vg[i].y, Vg[i].z);
            outfile << outbuf << endl;
        }
        outfile << endl;

        for (size_t i=1; i<Vt.size(); ++i)
        {
            sprintf(outbuf, "vt %f %f", Vt[i].x, Vt[i].y);
            outfile << outbuf << endl;
        }
        outfile << endl;

        for (size_t i=1; i<Vn.size(); ++i)
        {
            sprintf(outbuf, "vn %f %f %f", Vn[i].x, Vn[i].y, Vn[i].z);
            outfile << outbuf << endl;
        }
        outfile << endl;

        for (group &nextg : Groups)
        {
            if (nextg.name == "All")
            {
                continue;
            }
            sprintf(outbuf, "g cosmos.%s", nextg.name.c_str());
            outfile << outbuf << endl;
            for (size_t fidx : nextg.faceidx)
            {
                outfile << "f ";
                for (vertex nextv : Faces[fidx].vertices)
                {
                    if (nextv.vt == 0)
                    {
                        sprintf(outbuf, " %lu//%lu", nextv.v, nextv.vn);
                    }
                    else
                    {
                        sprintf(outbuf, " %lu/%lu/%lu", nextv.v, nextv.vt, nextv.vn);
                    }
                    outfile << outbuf;
                }
                outfile << endl;
            }
            outfile << endl;
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

    void wavefront::update_line(line &cline)
    {
        cline.centroid = Vector();
        cline.length = 0.;
        if (cline.vertices.size() > 0)
        {
        for (size_t j=0; j<cline.vertices.size(); ++j)
        {
            cline.centroid += Vg[cline.vertices[j].v];
            if (j>1)
            {
                cline.length += (Vg[cline.vertices[j].v] - Vg[cline.vertices[j-1].v]).norm();
            }
        }
        cline.centroid /= cline.vertices.size();
        }
    }

    void wavefront::update_face(face &cface)
    {
        if (cface.vertices.size() < 2)
        {
            return;
        }

        Vector fcentroid = Vg[cface.vertices[0].v];
        fcentroid += Vg[cface.vertices[1].v];
        Vector v1 = Vg[cface.vertices[0].v] - Vg[cface.vertices[cface.vertices.size()-1].v];
        Vector v2 = Vg[cface.vertices[1].v] - Vg[cface.vertices[0].v];
        Vector fnormal = v1.cross(v2);
        for (size_t j=2; j<cface.vertices.size(); ++j)
        {
            fcentroid += Vg[cface.vertices[j].v];
            v1 = v2;
            v2 = Vg[cface.vertices[j].v] - Vg[cface.vertices[j-1].v];
            fnormal += v1.cross(v2);
        }
        fcentroid /= cface.vertices.size();
        v1 = v2;
        v2 = Vg[cface.vertices[0].v] - Vg[cface.vertices[cface.vertices.size()-1].v];
        fnormal += v1.cross(v2);
        fnormal.normalize();
        cface.normal = fnormal;

        cface.com = Vector ();
        cface.area = 0.;
        v1 = Vg[cface.vertices[cface.vertices.size()-1].v] - fcentroid;
        for (size_t j=0; j<cface.vertices.size(); ++j)
        {
            v2 = Vg[cface.vertices[j].v] - fcentroid;
            // Area of triangle made by v1, v2 and Face centroid
            double tarea = v1.area(v2);
            // Sum
            cface.area += tarea;
            // Centroid of triangle made by v1, v2 amd Face centroid
            Vector tcentroid = (v1 + v2) / 3. + fcentroid;
            // Weighted sum
            //                Vector test = tarea * tcentroid;
            cface.com += tcentroid * tarea;
            v1 = v2;
        }
        // Divide by summed weights
        if (cface.area)
        {
            cface.com /= cface.area;
        }
    }

    void wavefront::update_group(group &cgroup)
    {
        // Calculate Center of Mass
        cgroup.com = Vector();
        for (size_t j=0; j<cgroup.faceidx.size(); ++j)
        {
            cgroup.com += Faces[cgroup.faceidx[j]].com;
        }
        if (cgroup.faceidx.size())
        {
            cgroup.com /= cgroup.faceidx.size();
        }

        // Calculate volume
        cgroup.volume = 0.;
        for (size_t j=0; j<cgroup.faceidx.size(); ++j)
        {
            Vector dv = Faces[(cgroup.faceidx[j])].com - cgroup.com;
            if (dv.norm() != 0.)
            {
                cgroup.volume += Faces[(cgroup.faceidx[j])].area * dv.norm() / 3.;
            }
        }
    }
}
