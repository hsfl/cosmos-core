#include "physicsclass.h"

namespace Cosmos
{
    namespace Physics
    {
        int32_t Structure::Setup(Type type)
        {
            int32_t iretn = 0;

            switch (type)
            {
            case NoType:
                iretn = add_u();
            case U1:
                iretn = add_u(1, 1, 1, NoPanel);
                break;
            case U1X:
                iretn = add_u(1, 1, 1, X);
                break;
            case U1Y:
                iretn = add_u(1, 1, 1, Y);
                break;
            case U1XY:
                iretn = add_u(1, 1, 1, XY);
                break;
            case U1_5:
                iretn = add_u(1, 1, 1.5, NoPanel);
                break;
            case U1_5X:
                iretn = add_u(1, 1, 1.5, X);
                break;
            case U1_5Y:
                iretn = add_u(1, 1, 1.5, Y);
                break;
            case U1_5XY:
                iretn = add_u(1, 1, 1.5, XY);
                break;
            case U3:
                iretn = add_u(1, 1, 3, NoPanel);
                break;
            case U3X:
                iretn = add_u(1, 1, 3, X);
                break;
            case U3Y:
                iretn = add_u(1, 1, 3, Y);
                break;
            case U3XY:
                iretn = add_u(1, 1, 3, XY);
                break;
            case U6:
                iretn = add_u(1, 2, 3, NoPanel);
                break;
            case U6X:
                iretn = add_u(1, 2, 3, X);
                break;
            case U6Y:
                iretn = add_u(1, 2, 3, Y);
                break;
            case U6XY:
                iretn = add_u(1, 2, 3, XY);
                break;
            case U12:
                iretn = add_u(2, 2, 3, NoPanel);
                break;
            case U12X:
                iretn = add_u(2, 2, 3, X);
                break;
            case U12Y:
                iretn = add_u(2, 2, 3, Y);
                break;
            case U12XY:
                iretn = add_u(2, 2, 3, XY);
                break;
            case HEX65W80H:
                iretn = add_hex(.65, .80, NoPanel);
                break;
            default:
                iretn =  GENERAL_ERROR_OUTOFRANGE;
            }

            if (iretn < 0)
            {
                return iretn;
            }

            // Calculate physical quantities
            iretn = PhysSetup(currentphys);
            if (iretn < 0)
            {
                return iretn;
            }

            return 0;
        }

        int32_t Structure::add_u(double x, double y, double z, ExternalPanelType type)
        {
            x *= .1;
            y *= .1;
            z *= .1;
            switch (type)
            {
            case NoType:
                add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .01);
                add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., -y/2., z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., y/2., -z/2.), .01);
                add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., y/2., -z/2.), .01);
                add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -y/2., z/2.), Vector(-x/2., -y/2., z/2.), .01);
                add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .01);
                add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., -y/2., -z/2.), .01);
                break;
            case X:
                add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .01, 2, 0.);
                add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., -y/2., z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., y/2., -z/2.), .01, 2, 0.);
                add_face("panel+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(z + x/2., y/2., -z/2.), Vector(z + x/2., -y/2., -z/2.), .01);
                add_face("panel-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-z - x/2., y/2., -z/2.), Vector(-z - x/2., -y/2., -z/2.), .01);

                add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., y/2., -z/2.), .01);
                add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -y/2., z/2.), Vector(-x/2., -y/2., z/2.), .01);

                add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .01);
                add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., -y/2., -z/2.), .01);
                break;
            case Y:
                add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .01);
                add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., -y/2., z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., y/2., -z/2.), .01);

                add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., y/2., -z/2.), .01, 2, 0.);
                add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -y/2., z/2.), Vector(-x/2., -y/2., z/2.), .01, 2, 0.);
                add_face("panel+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., z + y/2., -z/2.), Vector(-x/2., z + y/2., -z/2.), .01);
                add_face("panel-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -z - y/2., -z/2.), Vector(-x/2., -z - y/2., -z/2.), .01);

                add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .01);
                add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., -y/2., -z/2.), .01);
                break;
            case XY:
                add_face("external+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., -y/2., z/2.), .01, 2, 0.);
                add_face("external-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., -y/2., z/2.), Vector(-x/2., y/2., z/2.), Vector(-x/2., y/2., -z/2.), .01, 2, 0.);
                add_face("panel+x", Vector(x/2., -y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(z + x/2., y/2., -z/2.), Vector(z + x/2., -y/2., -z/2.), .01);
                add_face("panel-x", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(-z - x/2., y/2., -z/2.), Vector(-z - x/2., -y/2., -z/2.), .01);

                add_face("external+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., y/2., z/2.), Vector(x/2., y/2., -z/2.), .01, 2, 0.);
                add_face("external-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -y/2., z/2.), Vector(-x/2., -y/2., z/2.), .01, 2, 0.);
                add_face("panel+y", Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., z + y/2., -z/2.), Vector(-x/2., z + y/2., -z/2.), .01);
                add_face("panel-y", Vector(-x/2., -y/2., -z/2.), Vector(x/2., -y/2., -z/2.), Vector(x/2., -z - y/2., -z/2.), Vector(-x/2., -z - y/2., -z/2.), .01);

                add_face("external+z", Vector(-x/2., -y/2., z/2.), Vector(x/2., -y/2., z/2.), Vector(x/2., y/2., z/2.), Vector(-x/2., y/2., z/2.), .01);
                add_face("external-z", Vector(-x/2., -y/2., -z/2.), Vector(-x/2., y/2., -z/2.), Vector(x/2., y/2., -z/2.), Vector(x/2., -y/2., -z/2.), .01);
                break;
            }

            return 0;
        }

        int32_t Structure::add_hex(double width, double height, ExternalPanelType type)
        {
            switch (type)
            {
            case NoType:
                for (float angle=0.; angle<D2PI; angle+=D2PI/6.)
                {
                    add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, 1, .65);
                    add_triangle(Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), .01, true, .4);
                    add_triangle(Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, true, .4);
                }
                break;
            case X:
                for (float angle=0.; angle<D2PI; angle+=D2PI/6.)
                {
                    add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, 1, .65);
                    add_triangle(Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), .01, true, .4);
                    add_triangle(Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, true, .4);
                }
                break;
            case Y:
                for (float angle=0.; angle<D2PI; angle+=D2PI/6.)
                {
                    add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, 1, .65);
                    add_triangle(Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), .01, true, .4);
                    add_triangle(Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, true, .4);
                }
                break;
            case XY:
                for (float angle=0.; angle<D2PI; angle+=D2PI/6.)
                {
                    add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, 1, .65);
                    add_triangle(Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), height/2.), .01, true, .4);
                    add_triangle(Vector(width/2*cos(angle-D2PI/12.), width/2*sin(angle-D2PI/12.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/12.), width/2*sin(angle+D2PI/12.), -height/2.), .01, true, .4);
                }
                break;
            }

            return 0;
        }

        int32_t Structure::add_oct(double width, double height, ExternalPanelType type)
        {
            switch (type)
            {
            case NoType:
                for (float angle=0.; angle<D2PI; angle+=D2PI/8.)
                {
                    add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, 1, .65);
                    add_triangle(Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), .01, true, .4);
                    add_triangle(Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, true, .4);
                }

                break;
            case X:
                for (float angle=0.; angle<D2PI; angle+=D2PI/8.)
                {
                    add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, 1, .65);
                    add_triangle(Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), .01, true, .4);
                    add_triangle(Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, true, .4);
                }

                break;
            case Y:
                for (float angle=0.; angle<D2PI; angle+=D2PI/8.)
                {
                    add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, 1, .65);
                    add_triangle(Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), .01, true, .4);
                    add_triangle(Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, true, .4);
                }

                break;
            case XY:
                for (float angle=0.; angle<D2PI; angle+=D2PI/8.)
                {
                    add_face("side"+to_unsigned(angle, 3, true), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, 1, .65);
                    add_triangle(Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), height/2.), Vector(0., 0., height/2.), Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), height/2.), .01, true, .4);
                    add_triangle(Vector(width/2*cos(angle-D2PI/16.), width/2*sin(angle-D2PI/16.), -height/2.), Vector(0., 0., -height/2.), Vector(width/2*cos(angle+D2PI/16.), width/2*sin(angle+D2PI/16.), -height/2.), .01, true, .4);
                }

                break;
            }

            return 0;
        }

        int32_t Structure::add_cuboid(string name, Vector size, double depth, Quaternion orientation, Vector offset)
        {
            add_face(name+"+x", Vector(size.x/2., -size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(size.x/2., -size.y/2., size.z/2.), depth, 1, 0., orientation, offset);
            add_face(name+"-x", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(-size.x/2., -size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., -size.z/2.), depth, 1, 0., orientation, offset);
            add_face(name+"+y", Vector(-size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), depth, 1, 0., orientation, offset);
            add_face(name+"-y", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(size.x/2., -size.y/2., -size.z/2.), depth, 1, 0., orientation, offset);
            add_face(name+"+z", Vector(-size.x/2., -size.y/2., size.z/2.), Vector(size.x/2., -size.y/2., size.z/2.), Vector(size.x/2., size.y/2., size.z/2.), Vector(-size.x/2., size.y/2., size.z/2.), depth, 1, 0., orientation, offset);
            add_face(name+"-z", Vector(-size.x/2., -size.y/2., -size.z/2.), Vector(-size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., size.y/2., -size.z/2.), Vector(size.x/2., -size.y/2., -size.z/2.), depth, 1, 0., orientation, offset);

            return 0;
        }

        int32_t Structure::add_face(string name, Vector point0, Vector point1, Vector point2, Vector point3, double depth, uint8_t external, float pcell, Quaternion orientation, Vector offset)
        {
            point0 = orientation.irotate(point0);
            point0 += offset;
            point1 = orientation.irotate(point1);
            point1 += offset;
            point2 = orientation.irotate(point2);
            point2 += offset;
            point3 = orientation.irotate(point3);
            point3 += offset;
            Vector point4 = (point0 + point1 + point2 + point3) / 4.;

            add_triangle(point0, point1, point4, depth, external, pcell);
            add_triangle(point1, point2, point4, depth, external, pcell);
            add_triangle(point2, point3, point4, depth, external, pcell);
            add_triangle(point3, point0, point4, depth, external, pcell);

            return 0;
        }

        int32_t Structure::add_face(string name, Vector size, Quaternion orientation, Vector offset)
        {
            Vector points[5];

            points[0].x = -size.x / 2.;
            points[2].x = -size.x / 2.;
            points[1].x = size.x / 2.;
            points[3].x = size.x / 2.;

            points[0].y = -size.y / 2.;
            points[1].y = -size.y / 2.;
            points[2].y = size.y / 2.;
            points[3].y = size.y / 2.;

            for (uint16_t i=0; i<5; ++i)
            {
                points[i] = orientation.irotate(points[i]);
                points[i] += offset;
            }

            add_triangle(points[0], points[1], points[4], size.z);
            add_triangle(points[1], points[1], points[4], size.z);
            add_triangle(points[2], points[3], points[4], size.z);
            add_triangle(points[3], points[0], points[4], size.z);

            return 4;
        }

        int32_t Structure::add_triangle(Vector pointa, Vector pointb, Vector pointc, double depth, bool external, float pcell)
        {
            trianglestruc triangle;

            triangle.tidx[0] = add_vertex(pointa);
            triangle.tidx[1] = add_vertex(pointb);
            triangle.tidx[2] = add_vertex(pointc);

            triangle.external = external;
            triangle.depth = depth;
            triangle.pcell = pcell;
            triangle.com = (currentphys->vertices[triangle.tidx[0]] + currentphys->vertices[triangle.tidx[1]] + currentphys->vertices[triangle.tidx[2]]) / 3.;
            triangle.area = (currentphys->vertices[triangle.tidx[1]] - currentphys->vertices[triangle.tidx[0]]).area(currentphys->vertices[triangle.tidx[2]] - currentphys->vertices[triangle.tidx[0]]);
            triangle.normal = (currentphys->vertices[triangle.tidx[1]] - currentphys->vertices[triangle.tidx[0]]).cross(currentphys->vertices[triangle.tidx[2]] - currentphys->vertices[triangle.tidx[0]]).normalize(triangle.area);
            triangle.mass = triangle.area * triangle.depth * triangle.density;
            triangle.perimeter = (currentphys->vertices[triangle.tidx[1]] - currentphys->vertices[triangle.tidx[0]]).norm() + (currentphys->vertices[triangle.tidx[2]] - currentphys->vertices[triangle.tidx[1]]).norm() + (currentphys->vertices[triangle.tidx[0]] - currentphys->vertices[triangle.tidx[2]]).norm();

            triangle.twist = (triangle.com - triangle.normal).cross(triangle.normal);
            triangle.twist = (-triangle.area/(triangle.normal.norm2()) * triangle.twist);
            triangle.shove = Vector();
            for (uint16_t i=0; i<2; i++)
            {
                Vector tv0 = (currentphys->vertices[triangle.tidx[i]] - triangle.com);
                Vector tv1 = (currentphys->vertices[triangle.tidx[i+1]] - triangle.com);
                double ta = (tv0.norm());
                for (uint16_t j=0; j<=ta*100; j++)
                {
                    Vector tv2 = (tv0 * .01*j/ta);
                    Vector tv3 = (tv1 * .01*j/ta);
                    Vector dv = (tv3 - tv2);
                    double tb = (dv.norm());
                    for (size_t k=0; k<tb*100; k++)
                    {
                        Vector sv = (triangle.com + (tv2 + (dv * .01*k/tb)));
                        double tc = 1./(sv.norm2());
                        triangle.shove = (triangle.shove + (sv * tc));
                    }
                }
            }
            triangle.shove = (triangle.shove / -10000.);

            currentphys->triangles.push_back(triangle);

            return 1;
        }

        int32_t Structure::add_vertex(Vector point)
        {
            bool found = false;

            int32_t index = -1;
            for (uint16_t i=0; i<currentphys->vertices.size(); ++ i)
            {
                if ((point - currentphys->vertices[i]).norm() < .001)
                {
                    found = true;
                    index = i;
                    break;
                }
            }

            if (found)
            {
                return index;
            }
            else {
                currentphys->vertices.push_back(point);
                return currentphys->vertices.size() - 1;
            }
        }


        int32_t State::Init(string name, double idt, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, vector<Convert::tlestruc> lines, double utc)
        {
            dt = 86400.*((currentinfo.node.loc.utc + (idt / 86400.))-currentinfo.node.loc.utc);
            dtj = dt / 86400.;

            //            strncpy(currentinfo.node.name, name.c_str(), COSMOS_MAX_NAME);
            currentinfo.node.name = name;
            currentinfo.node.agent = "sim";
            currentinfo.node.loc.utc = utc;
            if (lines.size())
            {
                lines2eci(currentinfo.node.loc.utc, lines, currentinfo.node.loc.pos.eci);
                tle = lines;
            }
            else {
                return GENERAL_ERROR_POSITION;
            }

            structure = new Structure(&currentinfo.node.phys);
            structure->Setup(stype);
            this->stype = stype;

            switch (ptype)
            {
            case Propagator::Type::PositionInertial:
                inposition = new InertialPositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                dt = inposition->dt;
                dtj = inposition->dtj;
                break;
            case Propagator::Type::PositionIterative:
                itposition = new IterativePositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                dt = itposition->dt;
                dtj = itposition->dtj;
                break;
            case Propagator::Type::PositionGaussJackson:
                gjposition = new GaussJacksonPositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, 6);
                dt = gjposition->dt;
                dtj = gjposition->dtj;
                if (tle.size())
                {
                    gjposition->Init(tle);
                }
                else {
                    gjposition->Init();
                }
                break;
            case Propagator::Type::PositionGeo:
                geoposition = new GeoPositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                dt = geoposition->dt;
                dtj = geoposition->dtj;
                break;
            default:
                inposition = new InertialPositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                dt = inposition->dt;
                dtj = inposition->dtj;
                break;
            }
            this->ptype = ptype;

            switch (atype)
            {
            case Propagator::Type::AttitudeInertial:
                inattitude = new InertialAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                break;
            case Propagator::Type::AttitudeIterative:
                itattitude = new IterativeAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                break;
            case Propagator::Type::AttitudeLVLH:
                lvattitude = new LVLHAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                break;
            case Propagator::Type::AttitudeGeo:
                geoattitude = new GeoAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                break;
            case Propagator::Type::AttitudeSolar:
                solarattitude = new SolarAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                break;
            default:
                inattitude = new InertialAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                break;
            }
            this->atype = atype;

            switch (ttype)
            {
            case Propagator::Type::Thermal:
                thermal = new ThermalPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, 300.);
                break;
            default:
                thermal = new ThermalPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, 300.);
                break;
            }
            this->ttype = ttype;

            switch (etype)
            {
            case Propagator::Type::Electrical:
                electrical = new ElectricalPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, .5);
                break;
            default:
                electrical = new ElectricalPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, .5);
                break;
            }
            this->etype = etype;

            if (ptype == Propagator::PositionGeo)
            {
                Convert::pos_geod(currentinfo.node.loc);
            }
            else
            {
                Convert::pos_eci(currentinfo.node.loc);
                PosAccel(currentinfo.node.loc, currentinfo.node.phys);
            }
            if (atype == Propagator::AttitudeGeo)
            {
                currentinfo.node.loc.att.geoc.pass++;
                Convert::att_geoc(currentinfo.node.loc);
            }
            else if (atype == Propagator::AttitudeLVLH)
            {
                currentinfo.node.loc.att.lvlh.s = q_eye();
                currentinfo.node.loc.att.lvlh.v = rv_zero();
                currentinfo.node.loc.att.lvlh.a = rv_zero();
                currentinfo.node.loc.att.lvlh.utc = utc;
                currentinfo.node.loc.att.lvlh.pass++;
                Convert::att_lvlh(currentinfo.node.loc);
            }
            else
            {
                currentinfo.node.loc.att.icrf.pass++;
                Convert::att_icrf(currentinfo.node.loc);
                AttAccel(currentinfo.node.loc, currentinfo.node.phys);
            }

            initialloc = currentinfo.node.loc;
            initialphys = currentinfo.node.phys;
            currentinfo.node.utc = currentinfo.node.loc.utc;
            return 0;
        }

        int32_t State::Init(string name, double idt, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::cartpos eci)
        {
            int32_t iretn = 0;
            Convert::pos_clear(currentinfo.node.loc);
            currentinfo.node.loc.pos.eci = eci;
            currentinfo.node.loc.pos.eci.pass++;
            iretn = Convert::pos_eci(currentinfo.node.loc);
            if (iretn < 0)
            {
                return iretn;
            }
            currentinfo.node.loc.att.lvlh.pass++;
            currentinfo.node.loc.att.lvlh.s = q_eye();
            currentinfo.node.loc.att.lvlh.v = rv_zero();
            currentinfo.node.loc.att.lvlh.a = rv_zero();
            currentinfo.node.loc.att.lvlh.utc = eci.utc;
            iretn = Convert::att_lvlh(currentinfo.node.loc);
            if (iretn < 0)
            {
                return iretn;
            }

            return Init(name, idt, stype, ptype, atype, ttype, etype);
        }

        int32_t State::Init(string name, double idt, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::cartpos eci, Convert::qatt icrf)
        {
            int32_t iretn = 0;
            Convert::pos_clear(currentinfo.node.loc);
            currentinfo.node.loc.pos.eci = eci;
            currentinfo.node.loc.pos.eci.pass++;
            iretn = Convert::pos_eci(currentinfo.node.loc);
            if (iretn < 0)
            {
                return iretn;
            }
            currentinfo.node.loc.att.icrf = icrf;
            currentinfo.node.loc.att.icrf.pass++;
            iretn = Convert::att_icrf(currentinfo.node.loc);
            if (iretn < 0)
            {
                return iretn;
            }

            return Init(name, idt, stype, ptype, atype, ttype, etype);
        }

        int32_t State::Init(string name, double idt, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype)
        {
            dt = 86400.*((currentinfo.node.loc.utc + (idt / 86400.))-currentinfo.node.loc.utc);
            dtj = dt / 86400.;

            currentinfo.node.name = name;
            currentinfo.node.agent = "sim";

            structure = new Structure(&currentinfo.node.phys);
            structure->Setup(stype);
            this->stype = stype;

            switch (ptype)
            {
            case Propagator::Type::PositionInertial:
                inposition = new InertialPositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                inposition->Init();
                break;
            case Propagator::Type::PositionIterative:
                itposition = new IterativePositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                itposition->Init();
                break;
            case Propagator::Type::PositionGaussJackson:
                gjposition = new GaussJacksonPositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, 6);
                gjposition->Init();
                break;
            case Propagator::Type::PositionGeo:
                geoposition = new GeoPositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                geoposition->Init();
                break;
            default:
                inposition = new InertialPositionPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                inposition->Init();
                break;
            }
            this->ptype = ptype;

            switch (atype)
            {
            case Propagator::Type::AttitudeInertial:
                inattitude = new InertialAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                inattitude->Init();
                break;
            case Propagator::Type::AttitudeGeo:
                geoattitude = new GeoAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                geoattitude->Init();
                break;
            case Propagator::Type::AttitudeSolar:
                solarattitude = new SolarAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                solarattitude->Init();
                break;
            case Propagator::Type::AttitudeIterative:
                itattitude = new IterativeAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                itattitude->Init();
                break;
            case Propagator::Type::AttitudeLVLH:
                lvattitude = new LVLHAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                lvattitude->Init();
                break;
            default:
                inattitude = new InertialAttitudePropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt);
                inattitude->Init();
                break;
            }
            this->atype = atype;

            switch (ttype)
            {
            case Propagator::Type::Thermal:
                thermal = new ThermalPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, 300.);
                thermal->Init();
                break;
            default:
                thermal = new ThermalPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, 300.);
                thermal->Init();
                break;
            }
            this->ttype = ttype;

            switch (etype)
            {
            case Propagator::Type::Electrical:
                electrical = new ElectricalPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, .5);
                electrical->Init();
                break;
            default:
                electrical = new ElectricalPropagator(&currentinfo.node.loc, &currentinfo.node.phys, dt, .5);
                electrical->Init();
                break;
            }
            this->etype = etype;

            if (ptype == Propagator::PositionGeo)
            {
                Convert::pos_geod(currentinfo.node.loc);
            }
            else
            {
                Convert::pos_eci(currentinfo.node.loc);
                PosAccel(currentinfo.node.loc, currentinfo.node.phys);
            }
            if (atype == Propagator::AttitudeGeo)
            {
                att_geoc(currentinfo.node.loc);
            }
            else
            {
                Convert::att_icrf(currentinfo.node.loc);
                AttAccel(currentinfo.node.loc, currentinfo.node.phys);
            }

            initialloc = currentinfo.node.loc;
            initialphys = currentinfo.node.phys;
            currentinfo.node.utc = currentinfo.node.loc.utc;
            return 0;
        }

        int32_t State::Propagate(double nextutc)
        {
            int32_t count = 0;
            if (nextutc == 0.)
            {
                nextutc = currentinfo.node.utc + dtj;
            }

            while ((nextutc - currentinfo.node.utc) > dtj / 2.)
            {
                PhysCalc(&currentinfo.node.loc, &currentinfo.node.phys);

                // Thermal
                static_cast<ThermalPropagator *>(thermal)->Propagate(nextutc);

                // Electrical
                static_cast<ElectricalPropagator *>(electrical)->Propagate(nextutc);

                // Attitude
                switch (atype)
                {
                case Propagator::Type::AttitudeIterative:
                    static_cast<IterativeAttitudePropagator *>(itattitude)->Propagate(nextutc);
                    break;
                case Propagator::Type::AttitudeInertial:
                    static_cast<InertialAttitudePropagator *>(inattitude)->Propagate(nextutc);
                    break;
                case Propagator::Type::AttitudeLVLH:
                    static_cast<LVLHAttitudePropagator *>(lvattitude)->Propagate(nextutc);
                    break;
                case Propagator::Type::AttitudeGeo:
                    static_cast<GeoAttitudePropagator *>(geoattitude)->Propagate(nextutc);
                    break;
                case Propagator::Type::AttitudeSolar:
                    static_cast<SolarAttitudePropagator *>(solarattitude)->Propagate(nextutc);
                    break;
                default:
                    break;
                }

                // Position
                switch (ptype)
                {
                case Propagator::Type::PositionIterative:
                    static_cast<IterativePositionPropagator *>(itposition)->Propagate(nextutc);
                    break;
                case Propagator::Type::PositionInertial:
                    static_cast<InertialPositionPropagator *>(inposition)->Propagate(nextutc);
                    break;
                case Propagator::Type::PositionGaussJackson:
                    static_cast<GaussJacksonPositionPropagator *>(gjposition)->Propagate(nextutc, currentinfo.node.loc.att.icrf.s);
                    break;
                case Propagator::Type::PositionGeo:
                    static_cast<GeoPositionPropagator *>(geoposition)->Propagate(nextutc);
                    break;
                default:
                    break;
                }

                if (ptype == Propagator::PositionGeo)
                {
                    Convert::pos_geod(currentinfo.node.loc);
                }
                else
                {
                    Convert::pos_eci(currentinfo.node.loc);
                    PosAccel(currentinfo.node.loc, currentinfo.node.phys);
                }
                if (atype == Propagator::AttitudeGeo)
                {
                    att_geoc(currentinfo.node.loc);
                }
                else
                {
                    Convert::att_icrf(currentinfo.node.loc);
                    AttAccel(currentinfo.node.loc, currentinfo.node.phys);
                }

                // Update time
                currentinfo.node.utc += dtj;
                ++count;
            } ;

            // Update Targets
            rvector topo;
            rvector ds;
            rvector dv;
            for (targetstruc &target : targets)
            {
                target.cloc.pos.geod.utc = currentinfo.node.utc;
                target.cloc.pos.geod.pass++;
                Convert::loc_update(target.cloc);
                target.loc = target.cloc;
                if (target.size.lat)
                {
                    if (currentinfo.node.loc.pos.geod.s.lon >= fixangle(target.cloc.pos.geod.s.lon - target.size.lon / 2., false) && currentinfo.node.loc.pos.geod.s.lon <= fixangle(target.cloc.pos.geod.s.lon + target.size.lon / 2., false))
                    {
                        target.loc.pos.geod.s.lon = currentinfo.node.loc.pos.geod.s.lon;
                        if (currentinfo.node.loc.pos.geod.s.lat >= target.cloc.pos.geod.s.lat + target.size.lat / 2.)
                        {
                            target.loc.pos.geod.s.lat = target.cloc.pos.geod.s.lat + target.size.lat / 2.;
                            target.loc.pos.geod.pass++;
                            Convert::pos_geod(target.loc);
                            if (currentinfo.node.loc.pos.geod.v.lat < 0)
                            {
                                target.min = 1.;
                                target.utc = target.loc.utc;
                            }
                            else
                            {
                                target.min = 3.;
                                target.utc = target.loc.utc;
                            }
                        }
                        else if (currentinfo.node.loc.pos.geod.s.lat <= target.cloc.pos.geod.s.lat - target.size.lat / 2)
                        {
                            target.loc.pos.geod.s.lat = target.cloc.pos.geod.s.lat - target.size.lat / 2.;
                            target.loc.pos.geod.pass++;
                            Convert::pos_geod(target.loc);
                            if (currentinfo.node.loc.pos.geod.v.lat > 0)
                            {
                                target.min = 1.;
                                target.utc = target.loc.utc;
                            }
                            else
                            {
                                target.min = 3.;
                                target.utc = target.loc.utc;
                            }
                        }
                        else
                        {
                            if (target.min != 2.)
                            {
                                target.utc = target.loc.utc;
                            }
                            target.loc.pos.geod.pass++;
                            Convert::pos_geod(target.loc);
                            target.min = 2.;
                        }
                    }
                    else
                    {
                        target.min = 0.;
                        target.utc = target.loc.utc;
                    }
                }
                else
                {
                    target.min = 0.;
                    target.utc = target.loc.utc;
                }
                // Calculate bearing and distance
                double dx = cos(target.loc.pos.geod.s.lat) * sin(target.loc.pos.geod.s.lon - currentinfo.node.loc.pos.geod.s.lon);
                double dy = cos(currentinfo.node.loc.pos.geod.s.lat) * sin(target.loc.pos.geod.s.lat) - sin(currentinfo.node.loc.pos.geod.s.lat) * cos(target.loc.pos.geod.s.lat) * cos(target.loc.pos.geod.s.lon - currentinfo.node.loc.pos.geod.s.lon);
                target.bearing = atan2(dy, dx);
                target.distance = sep_rv(currentinfo.node.loc.pos.geoc.s, target.loc.pos.geoc.s);

                // Calculate Alt, Az, Range, Close
                Convert::geoc2topo(target.loc.pos.geod.s, currentinfo.node.loc.pos.geoc.s,topo);
                Convert::topo2azel(topo, target.azto, target.elto);
                if (target.elto <= 0.)
                {
                    target.maxelto = target.elto;
                }
                else if (target.elto > target.maxelto)
                {
                    target.maxelto = target.elto;
                }

                Convert::geoc2topo(currentinfo.node.loc.pos.geod.s, target.loc.pos.geoc.s, topo);
                Convert::topo2azel(topo, target.azfrom, target.elfrom);
                // Calculate direct vector from source to target
                ds = rv_sub(target.loc.pos.geoc.s, currentinfo.node.loc.pos.geoc.s);
                target.range = length_rv(ds);
                // Calculate velocity of target WRT source
                dv = rv_sub(target.loc.pos.geoc.v, currentinfo.node.loc.pos.geoc.v);
                // Closing speed is length of ds in 1 second minus length of ds now.
                target.close = length_rv(rv_sub(ds,dv)) - length_rv(ds);
            }

            return count;
        }

        int32_t State::Reset(double nextutc)
        {
            int32_t iretn = 0;
            currentinfo.node.loc = initialloc;
            currentinfo.node.utc = currentinfo.node.loc.utc;
            iretn = Propagate(nextutc);

            return iretn;
        }

        int32_t State::AddTarget(std::string name, Convert::locstruc loc, uint16_t type, gvector size)
        {
            targetstruc ttarget;
            ttarget.type = type;
            ttarget.name = name;
            ttarget.cloc = loc;
            ttarget.size = size;
            ttarget.loc = loc;

            targets.push_back(ttarget);
            return targets.size();
        }

        int32_t State::AddTarget(string name, double lat, double lon, double alt, uint16_t type)
        {
            Convert::locstruc loc;
            loc.pos.geod.pass = 1;
            loc.pos.geod.utc = currentinfo.node.utc;
            loc.pos.geod.s.lat = lat;
            loc.pos.geod.s.lon = lon;
            loc.pos.geod.s.h = alt;
            loc.pos.geod.v = gv_zero();
            loc.pos.geod.a = gv_zero();
            loc.pos.geod.pass++;
            Convert::pos_geod(loc);
            return AddTarget(name, loc, type);
        }

        int32_t State::AddTarget(string name, double ullat, double ullon, double lrlat, double lrlon, double alt, uint16_t type)
        {
            Convert::locstruc loc;
            loc.pos.geod.pass = 1;
            loc.pos.geod.utc = currentinfo.node.utc;
            loc.pos.geod.s.lat = (ullat + lrlat) / 2.;
            loc.pos.geod.s.lon = (ullon + lrlon) / 2.;
            loc.pos.geod.s.h = alt;
            loc.pos.geod.v = gv_zero();
            loc.pos.geod.a = gv_zero();
            gvector size(ullat-lrlat, lrlon-ullon, 0.);
            loc.pos.geod.pass++;
            Convert::pos_geod(loc);
            return AddTarget(name, loc, type, size);
        }

        int32_t State::AddTarget(string name, double clat, double clon, double radius, double alt)
        {
            Convert::locstruc loc;
            loc.pos.geod.pass = 1;
            loc.pos.geod.utc = currentinfo.node.utc;
            loc.pos.geod.s.lat = clat;
            loc.pos.geod.s.lon = clon;
            loc.pos.geod.s.h = alt;
            loc.pos.geod.v = gv_zero();
            loc.pos.geod.a = gv_zero();
            gvector size(2. * radius, 2. * radius / cos(lat), 0.);
            loc.pos.geod.pass++;
            Convert::pos_geod(loc);
            return AddTarget(name, loc, type, size);
        }

        int32_t InertialAttitudePropagator::Init()
        {

            return  0;
        }

        int32_t InertialAttitudePropagator::Reset(double nextutc)
        {
            currentloc->att = initialloc.att;
            currentutc = currentloc->att.utc;
            Propagate(nextutc);

            return  0;
        }

        int32_t InertialAttitudePropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            currentloc->att.icrf = initialloc.att.icrf;
            currentutc = nextutc;
            currentloc->att.icrf.utc = nextutc;
            currentloc->att.icrf.pass++;
            Convert::att_icrf(currentloc);

            return 0;
        }

        int32_t GeoAttitudePropagator::Init()
        {

            return  0;
        }

        int32_t GeoAttitudePropagator::Reset(double nextutc)
        {
            currentloc->att = initialloc.att;
            currentutc = currentloc->att.utc;
            Propagate(nextutc);

            return  0;
        }

        int32_t GeoAttitudePropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            currentloc->att.geoc = initialloc.att.geoc;
            currentutc = nextutc;
            currentloc->att.geoc.utc = nextutc;
            currentloc->att.geoc.pass++;
            att_geoc(currentloc);

            return 0;
        }

        int32_t IterativeAttitudePropagator::Init()
        {
            AttAccel(currentloc, currentphys);

            return  0;
        }

        int32_t IterativeAttitudePropagator::Reset(double nextutc)
        {
            currentloc->att = initialloc.att;
            currentutc = currentloc->att.utc;
            Propagate(nextutc);

            return 0;
        }

        int32_t IterativeAttitudePropagator::Propagate(double nextutc)
        {
            quaternion q1;

            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            while ((nextutc - currentutc) > dtj / 2.)
            {
                currentutc += dtj;
                q1 = q_axis2quaternion_rv(rv_smult(dt, currentloc->att.icrf.v));
                currentloc->att.icrf.s = q_fmult(q1, currentloc->att.icrf.s);
                normalize_q(&currentloc->att.icrf.s);
                // Calculate new v from da
                currentloc->att.icrf.v = rv_add(currentloc->att.icrf.v, rv_smult(dt, currentloc->att.icrf.a));
                currentloc->att.icrf.utc = currentutc;
                currentloc->att.icrf.pass++;
                AttAccel(currentloc, currentphys);
                Convert::att_icrf(currentloc);
            }

            return 0;
        }

        int32_t LVLHAttitudePropagator::Init()
        {
            currentloc->att.lvlh.utc = currentutc;
            currentloc->att.lvlh.s = q_eye();
            currentloc->att.lvlh.v = rv_zero();
            currentloc->att.lvlh.a = rv_zero();
            ++currentloc->att.lvlh.pass;
            Convert::att_lvlh2icrf(currentloc);
            AttAccel(currentloc, currentphys);
            Convert::att_icrf(currentloc);

            return  0;
        }

        int32_t LVLHAttitudePropagator::Reset(double nextutc)
        {
            currentloc->att = initialloc.att;
            currentutc = currentloc->att.utc;
            Propagate(nextutc);

            return  0;
        }

        int32_t LVLHAttitudePropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }

            currentutc = nextutc;
            currentloc->att.lvlh.utc = nextutc;
            currentloc->att.lvlh.s = q_eye();
            currentloc->att.lvlh.v = rv_zero();
            currentloc->att.lvlh.a = rv_zero();
            ++currentloc->att.lvlh.pass;
            Convert::att_lvlh2icrf(currentloc);
            AttAccel(currentloc, currentphys);
            Convert::att_icrf(currentloc);

            return 0;
        }

        int32_t SolarAttitudePropagator::Init()
        {
            optimum = Quaternion(currentloc->att.icrf.s).drotate(Vector(0.,0.,1.));
            currentloc->att.icrf.utc = currentutc;
            currentloc->att.icrf.s = drotate_between(-Vector(currentloc->pos.extra.sun2earth.s), optimum).conjugate().to_q();
            currentloc->att.icrf.v = rv_zero();
            currentloc->att.icrf.a = rv_zero();
            ++currentloc->att.icrf.pass;
            AttAccel(currentloc, currentphys);
            Convert::att_icrf(currentloc);
            initialloc.att = currentloc->att;

            return  0;
        }

        int32_t SolarAttitudePropagator::Reset(double nextutc)
        {
            currentloc->att = initialloc.att;
            currentutc = currentloc->att.utc;
            Propagate(nextutc);

            return  0;
        }

        int32_t SolarAttitudePropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }

            currentutc = nextutc;
            currentloc->att.icrf.utc = nextutc;
            currentloc->att.icrf.s = drotate_between(-Vector(currentloc->pos.extra.sun2earth.s), optimum).conjugate().to_q();
            currentloc->att.icrf.v = rv_zero();
            currentloc->att.icrf.a = rv_zero();
            ++currentloc->att.icrf.pass;
            AttAccel(currentloc, currentphys);
            Convert::att_icrf(currentloc);

            return 0;
        }

        int32_t ThermalPropagator::Init(float temp)
        {
            currentphys->temp = temp;
            return 0;
        }

        int32_t ThermalPropagator::Reset(float temp)
        {
            currentutc = currentloc->utc;
            if (temp == 0.f)
            {
                currentphys->temp = initialphys.temp;
            }
            else
            {
                currentphys->temp = temp;
            }
            for (trianglestruc& triangle : currentphys->triangles)
            {
                triangle.temp = currentphys->temp;
                triangle.heat = triangle.temp * (triangle.mass * triangle.hcap);
            }
            return 0;
        }

        int32_t ThermalPropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            while ((nextutc - currentutc) > dtj / 2.)
            {
                currentutc += dtj;
                double energyd = 0.;
                currentphys->temp = currentphys->heat / (currentphys->mass * currentphys->hcap);
                currentphys->heat = 0.;
                double ienergyd = currentphys->radiation / currentphys->mass;
                currentphys->radiation = 0.;
                for (trianglestruc& triangle : currentphys->triangles)
                {
                    triangle.temp = triangle.heat / (triangle.mass * triangle.hcap);

                    // First do all inputs

                    if (triangle.external)
                    {
                        energyd = (triangle.sirradiation + triangle.eirradiation) * dt;
                        // Area not covered with cells
                        if (triangle.pcell > 0.)
                        {
                            triangle.heat += (1. - triangle.pcell) * triangle.abs * energyd;
                            // Area covered with cells
                            if (triangle.ecellbase > 0.)
                            {
                                double efficiency = triangle.ecellbase + triangle.ecellslope * triangle.temp;
                                triangle.heat += (triangle.pcell) * triangle.abs * (1. - efficiency) * energyd;
                            }
                            else
                            {
                                triangle.heat += triangle.pcell * triangle.abs * energyd;
                            }
                        }
                        else
                        {
                            triangle.heat = triangle.abs * energyd;
                        }

                        // Add back in previous radiation
                        triangle.heat += triangle.mass * ienergyd / 2.;
                    }
                    else
                    {
                        // Internal Normal faces
                        triangle.heat += triangle.mass * ienergyd;
                    }

                    // Then do outputs
                    energyd = 2. * triangle.area * dt * SIGMA * pow(triangle.temp ,4);
                    if (triangle.external == 0)
                    {
                        // Purely internal
                        triangle.heat -= triangle.iemi * energyd;
                        currentphys->radiation += triangle.iemi * energyd;
                    }
                    else
                    {
                        // External
                        triangle.heat -= 1.05 * triangle.emi * energyd;
                        currentphys->radiation += 1.95 * triangle.iemi * energyd;
                    }

                    triangle.temp = triangle.heat / (triangle.mass * triangle.hcap);
                    currentphys->heat += triangle.heat;
                }

                currentphys->temp = currentphys->heat / (currentphys->mass * currentphys->hcap);
                temperature = currentphys->temp;
            }

            return 0;
        }

        int32_t ElectricalPropagator::Init(float bp)
        {
            battery_charge = bp;
            return 0;
        }

        int32_t ElectricalPropagator::Reset(float bp)
        {
            currentutc = currentloc->utc;
            if (bp == 0.f)
            {
                currentphys->battlev = initialphys.battlev;
            }
            else
            {
                currentphys->battlev = bp;
            }
            return  0;
        }

        int32_t ElectricalPropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            while ((nextutc - currentutc) > dtj / 2.)
            {
                currentutc += dtj;
                currentphys->powgen = 0.;
                for (trianglestruc& triangle : currentphys->triangles)
                {
                    if (triangle.external)
                    {
                        if (triangle.pcell > 0.)
                        {
                            if (triangle.ecellbase > 0.)
                            {
                                double efficiency = triangle.ecellbase + triangle.ecellslope * triangle.temp;
                                triangle.power = efficiency * triangle.sirradiation;
                                triangle.volt = triangle.vcell;
                                triangle.amp = -triangle.power / triangle.volt;
                                currentphys->powgen += triangle.power;
                            }
                        }
                    }
                    else
                    {
                        triangle.power = 0.;
                        triangle.volt = 0.;
                        triangle.amp = 0.;
                    }

                }
            }
            return 0;
        }

        int32_t InertialPositionPropagator::Init()
        {
            PosAccel(currentloc, currentphys);

            return 0;
        }

        int32_t InertialPositionPropagator::Reset(double nextutc)
        {
            currentloc->pos = initialloc.pos;
            currentutc = currentloc->pos.utc;
            Propagate(nextutc);

            return 0;
        }

        int32_t InertialPositionPropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            currentloc->pos.icrf = initialloc.pos.icrf;
            currentutc = nextutc;
            currentloc->pos.icrf.utc = nextutc;
            currentloc->pos.icrf.pass++;
            Convert::pos_icrf(currentloc);
            PosAccel(currentloc, currentphys);

            return 0;
        }

        int32_t GeoPositionPropagator::Init()
        {
            //            PosAccel(currentloc, currentphys);

            return 0;
        }

        int32_t GeoPositionPropagator::Reset(double nextutc)
        {
            currentloc->pos = initialloc.pos;
            currentutc = currentloc->pos.utc;
            Propagate(nextutc);

            return 0;
        }

        int32_t GeoPositionPropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            currentloc->pos.geod = initialloc.pos.geod;
            currentutc = nextutc;
            currentloc->pos.geod.utc = nextutc;
            currentloc->pos.geod.pass = currentloc->pos.eci.pass + 1;
            Convert::pos_geod(currentloc);
            //            PosAccel(currentloc, currentphys);

            return 0;
        }

        int32_t IterativePositionPropagator::Init()
        {
            PosAccel(currentloc, currentphys);

            return 0;
        }

        int32_t IterativePositionPropagator::Reset(double nextutc)
        {
            currentloc->pos = initialloc.pos;
            currentutc = currentloc->pos.utc;
            Propagate(nextutc);

            return 0;
        }

        int32_t IterativePositionPropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            while ((nextutc - currentutc) > dtj / 2.)
            {
                currentutc += dtj;
                rvector ds = rv_smult(.5 * dt * dt, currentloc->pos.eci.a);
                ds = rv_add(ds, rv_smult(dt, currentloc->pos.eci.v));
                currentloc->pos.eci.s = rv_add(currentloc->pos.eci.s, ds);
                currentloc->pos.eci.v = rv_add(currentloc->pos.eci.v, rv_smult(dt, currentloc->pos.eci.a));
                currentloc->pos.eci.utc = currentutc;
                currentloc->pos.eci.pass++;
                PosAccel(currentloc, currentphys);
                Convert::pos_eci(currentloc);
            }

            return 0;
        }

        int32_t TlePositionPropagator::Init(vector<Convert::tlestruc> tles)
        {
            this->tles = tles;
            Convert::lines2eci(currentutc, tles, currentloc->pos.eci);
            currentloc->pos.eci.pass++;
            PosAccel(currentloc, currentphys);
            Convert::pos_eci(currentloc);

            return 0;
        }

        int32_t TlePositionPropagator::Reset(double nextutc)
        {
            currentloc->pos = initialloc.pos;
            currentutc = currentloc->pos.utc;
            Propagate(nextutc);

            return 0;
        }

        int32_t TlePositionPropagator::Propagate(double nextutc)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }
            while ((nextutc - currentutc) > dtj / 2.)
            {
                currentutc += dtj;
                Convert::lines2eci(currentutc, tles, currentloc->pos.eci);
                currentloc->pos.eci.pass++;
                PosAccel(currentloc, currentphys);
                Convert::pos_eci(currentloc);
            }

            return 0;
        }

        int32_t GaussJacksonPositionPropagator::Setup()
        {
            step.resize(order+2);
            binom.resize(order+2);
            beta.resize(order+2);
            alpha.resize(order+2);
            for (uint16_t i=0; i<order+2; ++i)
            {
                binom[i].resize(order+2);
                beta[i].resize(order+1);
                alpha[i].resize(order+1);
            }
            c.resize(order+3);
            gam.resize(order+2);
            q.resize(order+3);
            lam.resize(order+3);
            order = 0;

            dtsq = dt * dt;

            order2 = order/2;
            //            order = order2 * 2;

            for (uint16_t m=0; m<order+2; m++)
            {
                for (uint16_t i=0; i<order+2; i++)
                {
                    if (m > i)
                        binom[m][i] = 0;
                    else
                    {
                        if (m == i)
                            binom[m][i] = 1;
                        else
                        {
                            if (m == 0)
                                binom[m][i] = 1;
                            else
                            {
                                binom[m][i] = binom[m-1][i-1] + binom[m][i-1];
                            }
                        }
                    }
                }
            }

            c[0] = 1.;
            for (uint16_t n=1; n<order+3; n++)
            {
                c[n] = 0.;
                for (uint16_t i=0; i<=n-1; i++)
                {
                    c[n] -= c[i] / (n+1-i);
                }
            }

            gam[0] = c[0];
            for (uint16_t i=1; i<order+2; i++)
            {
                gam[i] = gam[i-1] + c[i];
            }

            for (uint16_t i=0; i<order+1; i++)
            {
                beta[order+1][i] = gam[i+1];
                beta[order][i] = c[i+1];
                for (uint32_t j=order-1; j<order; --j)
                {
                    if (!i)
                        beta[j][i] = beta[j+1][i];
                    else
                        beta[j][i] = beta[j+1][i] - beta[j+1][i-1];
                }
            }

            q[0] = 1.;
            for (uint16_t i=1; i<order+3; i++)
            {
                q[i] = 0.;
                for (uint32_t k=0; k<=i; k++)
                {
                    q[i] += c[k]*c[i-k];
                }
            }

            lam[0] = q[0];
            for (uint16_t i=1; i<order+3; i++)
            {
                lam[i] = lam[i-1] + q[i];
            }

            for (uint16_t i=0; i<order+1; i++)
            {
                alpha[order+1][i] = lam[i+2];
                alpha[order][i] = q[i+2];
                for (uint32_t j=order-1; j<order; --j)
                {
                    if (!i)
                        alpha[j][i] = alpha[j+1][i];
                    else
                        alpha[j][i] = alpha[j+1][i] - alpha[j+1][i-1];
                }
            }

            for (uint16_t j=0; j<order+2; j++)
            {
                step[j].a.resize(order+1);
                step[j].b.resize(order+1);
                for (uint16_t m=0; m<order+1; m++)
                {
                    step[j].a[order-m] = 0.;
                    step[j].b[order-m] = 0.;
                    for (uint32_t i=m; i<=order; i++)
                    {
                        step[j].a[order-m] += alpha[j][i] * binom[m][i];
                        step[j].b[order-m] += beta[j][i] * binom[m][i];
                    }
                    step[j].a[order-m] *= pow(-1.,m);
                    step[j].b[order-m] *= pow(-1.,m);
                    if (order-m == j)
                        step[j].b[order-m] += .5;
                }
            }
            return 0;
        }

        int32_t GaussJacksonPositionPropagator::Init(vector<Convert::tlestruc>lines)
        {
            int32_t iretn = 0;

            loc_clear(step[order+1].loc);
            lines2eci(currentloc->utc, lines, currentloc->pos.eci);
            ++currentloc->pos.eci.pass;
            Convert::pos_eci(currentloc);
            PosAccel(currentloc, currentphys);
            //            AttAccel(currentloc, currentphys);
            step[order2].loc = *currentloc;

            // Position at t0-dt
            for (uint32_t i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= dtj;
                lines2eci(step[i].loc.utc, lines, step[i].loc.pos.eci);
                step[i].loc.pos.eci.pass++;
                Convert::pos_eci(step[i].loc);

                step[i].loc.att.lvlh = step[i+1].loc.att.lvlh;
                Convert::att_lvlh2icrf(step[i].loc);

                PosAccel(&step[i].loc, currentphys);
                AttAccel(&step[i].loc, currentphys);
            }

            for (uint32_t i=order2+1; i<=order; i++)
            {
                step[i].loc = step[i-1].loc;

                step[i].loc.utc += dtj;
                lines2eci(step[i].loc.utc, lines, step[i].loc.pos.eci);
                step[i].loc.pos.eci.pass++;
                Convert::pos_eci(step[i].loc);

                step[i].loc.att.lvlh = step[i-1].loc.att.lvlh;
                Convert::att_lvlh2icrf(step[i].loc);

                PosAccel(&step[i].loc, currentphys);
                AttAccel(&step[i].loc, currentphys);
            }

            iretn = Converge();
            currentphys->utc = currentloc->utc;
            currentutc = currentloc->utc;

            return iretn;
        }

        //! Initialize Gauss-Jackson orbit using ECI state vector
        /*! Initializes Gauss-Jackson structures using supplied initial state vector.
            \param gjh Reference to ::gj_handle for Gauss-Jackson integration.
            \param order the order at which the integration will be performed (must be even)
            \param mode Mode of physics propagation. Zero is free propagation.
            \param dt Step size in seconds
            \param utc Initial step time as UTC in Modified Julian Days
            \param ipos Initial ECI Position
            \param iatt Initial ICRF Attitude
            \param physics Reference to ::physicsstruc to use.
            \param loc Reference to ::locstruc to use.
        */
        // TODO: split the orbit from the attitude propagation sections of the code
        int32_t GaussJacksonPositionPropagator::Init()
        {
            Convert::kepstruc kep;
            double dea;
            uint32_t i;
            quaternion q1;
            int32_t iretn = 0;

            // Make sure ::locstruc is internally self consistent
            ++currentloc->pos.eci.pass;
            Convert::pos_eci(currentloc);
            // Update accelerations
            PosAccel(currentloc, currentphys);

            initialloc = *currentloc;
            initialphys = *currentphys;
            currentphys->utc = currentloc->utc;

            // Zero out original N+1 bin
            loc_clear(step[order+1].loc);


            // Set central bin to initial state vector
            step[order2].loc = *currentloc;

            // Position at t0-dt
            Convert::eci2kep(currentloc->pos.eci, kep);

            // Initialize past bins
            for (i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= dtj;
                kep.utc = step[i].loc.utc;
                kep.ma -= dt * kep.mm;

                uint16_t count = 0;
                do
                {
                    dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
                    kep.ea -= dea;
                } while (++count < 100 && fabs(dea) > .000001);
                step[i].loc.pos.eci.utc = kep.utc;
                kep2eci(kep, step[i].loc.pos.eci);
                ++step[i].loc.pos.eci.pass;

                q1 = q_axis2quaternion_rv(rv_smult(-dt,step[i].loc.att.icrf.v));
                step[i].loc.att.icrf.s = q_fmult(q1,step[i].loc.att.icrf.s);
                normalize_q(&step[i].loc.att.icrf.s);
                // Calculate new v from da
                step[i].loc.att.icrf.v = rv_add(step[i].loc.att.icrf.v,rv_smult(-dt,step[i].loc.att.icrf.a));
                step[i].loc.att.icrf.utc = kep.utc;
                Convert::pos_eci(step[i].loc);

                PosAccel(&step[i].loc, currentphys);
            }

            Convert::eci2kep(currentloc->pos.eci, kep);

            // Initialize future bins
            for (i=order2+1; i<=order; i++)
            {
                step[i] = step[i-1];
                step[i].loc.utc += dtj;
                kep.utc = step[i].loc.utc;
                kep.ma += dt * kep.mm;

                uint16_t count = 0;
                do
                {
                    dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
                    kep.ea -= dea;
                } while (++count < 100 && fabs(dea) > .000001);
                step[i].loc.pos.eci.utc = kep.utc;
                kep2eci(kep, step[i].loc.pos.eci);
                ++step[i].loc.pos.eci.pass;

                q1 = q_axis2quaternion_rv(rv_smult(dt,step[i].loc.att.icrf.v));
                step[i].loc.att.icrf.s = q_fmult(q1,step[i].loc.att.icrf.s);
                normalize_q(&step[i].loc.att.icrf.s);
                // Calculate new v from da
                step[i].loc.att.icrf.v = rv_add(step[i].loc.att.icrf.v,rv_smult(dt,step[i].loc.att.icrf.a));
                step[i].loc.att.icrf.utc = kep.utc;
                Convert::pos_eci(step[i].loc);

                PosAccel(&step[i].loc, currentphys);
            }
            currentutc = step[order].loc.utc;

            // Converge on rational set of values
            iretn = Converge();

            return iretn;
        }

        int32_t GaussJacksonPositionPropagator::Init(vector<Convert::locstruc> locs)
        {
            int32_t iretn = 0;

            // Make sure ::locstruc is internally self consistent
            ++currentloc->pos.eci.pass;
            Convert::pos_eci(currentloc);

            // Zero out original N+1 bin
            loc_clear(step[order+1].loc);

            // Calculate initial accelerations
            PosAccel(currentloc, currentphys);
            //            AttAccel(currentloc, currentphys);

            // Set central bin to initial state vector
            step[order2].loc = *currentloc;

            // Initialize past bins
            for (uint32_t i=order2-1; i<order2; --i)
            {
                step[i].loc = step[i+1].loc;
                step[i].loc.utc -= dtj;

                size_t index = locs.size();
                double dutc = 50000.;
                for (uint16_t j=0; j<locs.size(); ++j)
                {
                    if (fabs(locs[j].utc - step[j].loc.utc) > dutc)
                    {
                        dutc = fabs(locs[j].utc - step[j].loc.utc);
                        index = j;
                    }
                }
                if (index < locs.size())
                {
                    step[i].loc = locs[index];
                }

                PosAccel(&step[i].loc, currentphys);
                //                AttAccel(&step[i].loc, currentphys);
            }

            for (uint32_t i=order2+1; i<=order; i++)
            {
                step[i] = step[i-1];
                step[i].loc.utc += dtj;

                size_t index = locs.size();
                double dutc = 50000.;
                for (uint16_t j=0; j<locs.size(); ++j)
                {
                    if (fabs(locs[j].utc - step[j].loc.utc) > dutc)
                    {
                        dutc = fabs(locs[j].utc - step[j].loc.utc);
                        index = j;
                    }
                }
                if (index < locs.size())
                {
                    step[i].loc = locs[index];
                }

                PosAccel(&step[i].loc, currentphys);
                //                AttAccel(&step[i].loc, currentphys);
            }
            iretn = Converge();

            currentphys->utc = currentloc->utc;
            currentutc = step[order].loc.utc;
            return iretn;
        }

        int32_t GaussJacksonPositionPropagator::Reset(double nextutc)
        {
            int32_t iretn = 0;
            currentloc->pos = initialloc.pos;
            currentutc = currentloc->pos.utc;
            iretn = Init();
            Propagate(nextutc);
            return iretn;
        }

        int32_t GaussJacksonPositionPropagator::Propagate(double nextutc, quaternion icrf)
        {
            if (nextutc == 0.)
            {
                nextutc = currentutc + dtj;
            }

            while ((nextutc - currentutc) > dtj / 2.)
            {
                currentutc += dtj;
//                Vector normal, unitv, unitx, unitp, unitp1, unitp2;
//                Vector lunitp1(.1,.1,0.);
//                Vector tvector;

                // Don't bother if too low
                if (Vector(currentloc->pos.eci.s).norm() < REARTHM)
                {
                    return GENERAL_ERROR_TOO_LOW;
                }

                step[order+1].loc.utc = step[order+1].loc.pos.utc = step[order+1].loc.pos.eci.utc = step[order].loc.pos.eci.utc + dtj;
                step[order+1].loc.att.icrf.s = icrf;

                // Calculate S(order/2+1)
                step[order+1].ss.col[0] = step[order].ss.col[0] + step[order].s.col[0] + step[order].loc.pos.eci.a.col[0]/2.;
                step[order+1].ss.col[1] = step[order].ss.col[1] + step[order].s.col[1] + step[order].loc.pos.eci.a.col[1]/2.;
                step[order+1].ss.col[2] = step[order].ss.col[2] + step[order].s.col[2] + step[order].loc.pos.eci.a.col[2]/2.;

                // Calculate Sum(order/2+1) for a and b
                step[order+1].sb = step[order+1].sa = rv_zero();
                for (uint16_t k=0; k<=order; k++)
                {
                    step[order+1].sb.col[0] += step[order+1].b[k] * step[k].loc.pos.eci.a.col[0];
                    step[order+1].sa.col[0] += step[order+1].a[k] * step[k].loc.pos.eci.a.col[0];
                    step[order+1].sb.col[1] += step[order+1].b[k] * step[k].loc.pos.eci.a.col[1];
                    step[order+1].sa.col[1] += step[order+1].a[k] * step[k].loc.pos.eci.a.col[1];
                    step[order+1].sb.col[2] += step[order+1].b[k] * step[k].loc.pos.eci.a.col[2];
                    step[order+1].sa.col[2] += step[order+1].a[k] * step[k].loc.pos.eci.a.col[2];
                }

                // Calculate pos.v(order/2+1)
                step[order+1].loc.pos.eci.v.col[0] = this->dt * (step[order].s.col[0] + step[order].loc.pos.eci.a.col[0]/2. + step[order+1].sb.col[0]);
                step[order+1].loc.pos.eci.v.col[1] = this->dt * (step[order].s.col[1] + step[order].loc.pos.eci.a.col[1]/2. + step[order+1].sb.col[1]);
                step[order+1].loc.pos.eci.v.col[2] = this->dt * (step[order].s.col[2] + step[order].loc.pos.eci.a.col[2]/2. + step[order+1].sb.col[2]);

                // Calculate pos.s(order/2+1)
                step[order+1].loc.pos.eci.s.col[0] = this->dtsq * (step[order+1].ss.col[0] + step[order+1].sa.col[0]);
                step[order+1].loc.pos.eci.s.col[1] = this->dtsq * (step[order+1].ss.col[1] + step[order+1].sa.col[1]);
                step[order+1].loc.pos.eci.s.col[2] = this->dtsq * (step[order+1].ss.col[2] + step[order+1].sa.col[2]);
                step[order+1].loc.pos.eci.pass++;
                Convert::pos_eci(step[order+1].loc);

                // Update inherent accelerations for this location
                PosAccel(&step[order+1].loc, currentphys);

                // Calculate s(order/2+1)
                step[order+1].s.col[0] = step[order].s.col[0] + (step[order].loc.pos.eci.a.col[0]+step[order+1].loc.pos.eci.a.col[0])/2.;
                step[order+1].s.col[1] = step[order].s.col[1] + (step[order].loc.pos.eci.a.col[1]+step[order+1].loc.pos.eci.a.col[1])/2.;
                step[order+1].s.col[2] = step[order].s.col[2] + (step[order].loc.pos.eci.a.col[2]+step[order+1].loc.pos.eci.a.col[2])/2.;

                // Shift everything over 1
                for (uint16_t j=0; j<=order; j++)
                {
                    step[j] = step[j+1];
                }

                // Adjust for any thrust
                if (currentphys->fpush.norm() && currentphys->mass)
                {
                    rvector dacc = (currentphys->fpush / currentphys->mass).to_rv();
                    for (gjstruc &cstep : step)
                    {
                        cstep.loc.pos.eci.s = rv_add(cstep.loc.pos.eci.s, rv_smult(.5 * this->dt2, dacc));
                        cstep.loc.pos.eci.v = rv_add(cstep.loc.pos.eci.v, rv_smult(this->dt, dacc));
                    }
                    Setup();
                    Converge();
                }
            }

            currentloc->pos = step[order].loc.pos;
            for (uint16_t i=order; i<=order; --i)
            {
                if (nextutc >= currentloc->pos.utc - dtj / 2.)
                {
                    break;
                }
                currentloc->pos = step[i].loc.pos;
                currentloc->utc = currentloc->pos.utc;
            }

            return 0;
        }

        int32_t GaussJacksonPositionPropagator::Converge()
        {
            uint32_t c_cnt, cflag=0, k;
            rvector oldsa;

            PosAccel(currentloc, currentphys);

            c_cnt = 0;
            do
            {
                step[order2].s.col[0] = step[order2].loc.pos.eci.v.col[0]/this->dt;
                step[order2].s.col[1] = step[order2].loc.pos.eci.v.col[1]/this->dt;
                step[order2].s.col[2] = step[order2].loc.pos.eci.v.col[2]/this->dt;
                for (k=0; k<=order; k++)
                {
                    step[order2].s.col[0] -= step[order2].b[k] * step[k].loc.pos.eci.a.col[0];
                    step[order2].s.col[1] -= step[order2].b[k] * step[k].loc.pos.eci.a.col[1];
                    step[order2].s.col[2] -= step[order2].b[k] * step[k].loc.pos.eci.a.col[2];
                }
                for (uint16_t n=1; n<=order2; n++)
                {
                    step[order2+n].s.col[0] = step[order2+n-1].s.col[0] + (step[order2+n].loc.pos.eci.a.col[0]+step[order2+n-1].loc.pos.eci.a.col[0])/2;
                    step[order2+n].s.col[1] = step[order2+n-1].s.col[1] + (step[order2+n].loc.pos.eci.a.col[1]+step[order2+n-1].loc.pos.eci.a.col[1])/2;
                    step[order2+n].s.col[2] = step[order2+n-1].s.col[2] + (step[order2+n].loc.pos.eci.a.col[2]+step[order2+n-1].loc.pos.eci.a.col[2])/2;
                    step[order2-n].s.col[0] = step[order2-n+1].s.col[0] - (step[order2-n].loc.pos.eci.a.col[0]+step[order2-n+1].loc.pos.eci.a.col[0])/2;
                    step[order2-n].s.col[1] = step[order2-n+1].s.col[1] - (step[order2-n].loc.pos.eci.a.col[1]+step[order2-n+1].loc.pos.eci.a.col[1])/2;
                    step[order2-n].s.col[2] = step[order2-n+1].s.col[2] - (step[order2-n].loc.pos.eci.a.col[2]+step[order2-n+1].loc.pos.eci.a.col[2])/2;
                }
                step[order2].ss.col[0] = step[order2].loc.pos.eci.s.col[0]/this->dtsq;
                step[order2].ss.col[1] = step[order2].loc.pos.eci.s.col[1]/this->dtsq;
                step[order2].ss.col[2] = step[order2].loc.pos.eci.s.col[2]/this->dtsq;
                for (k=0; k<=order; k++)
                {
                    step[order2].ss.col[0] -= step[order2].a[k] * step[k].loc.pos.eci.a.col[0];
                    step[order2].ss.col[1] -= step[order2].a[k] * step[k].loc.pos.eci.a.col[1];
                    step[order2].ss.col[2] -= step[order2].a[k] * step[k].loc.pos.eci.a.col[2];
                }
                for (uint16_t n=1; n<=order2; n++)
                {
                    step[order2+n].ss.col[0] = step[order2+n-1].ss.col[0] + step[order2+n-1].s.col[0] + (step[order2+n-1].loc.pos.eci.a.col[0])/2;
                    step[order2+n].ss.col[1] = step[order2+n-1].ss.col[1] + step[order2+n-1].s.col[1] + (step[order2+n-1].loc.pos.eci.a.col[1])/2;
                    step[order2+n].ss.col[2] = step[order2+n-1].ss.col[2] + step[order2+n-1].s.col[2] + (step[order2+n-1].loc.pos.eci.a.col[2])/2;
                    step[order2-n].ss.col[0] = step[order2-n+1].ss.col[0] - step[order2-n+1].s.col[0] + (step[order2-n+1].loc.pos.eci.a.col[0])/2;
                    step[order2-n].ss.col[1] = step[order2-n+1].ss.col[1] - step[order2-n+1].s.col[1] + (step[order2-n+1].loc.pos.eci.a.col[1])/2;
                    step[order2-n].ss.col[2] = step[order2-n+1].ss.col[2] - step[order2-n+1].s.col[2] + (step[order2-n+1].loc.pos.eci.a.col[2])/2;
                }

                for (uint16_t n=0; n<=order; n++)
                {
                    if (n == order2)
                        continue;
                    step[n].sb = step[n].sa = rv_zero();
                    for (k=0; k<=order; k++)
                    {
                        step[n].sb.col[0] += step[n].b[k] * step[k].loc.pos.eci.a.col[0];
                        step[n].sa.col[0] += step[n].a[k] * step[k].loc.pos.eci.a.col[0];
                        step[n].sb.col[1] += step[n].b[k] * step[k].loc.pos.eci.a.col[1];
                        step[n].sa.col[1] += step[n].a[k] * step[k].loc.pos.eci.a.col[1];
                        step[n].sb.col[2] += step[n].b[k] * step[k].loc.pos.eci.a.col[2];
                        step[n].sa.col[2] += step[n].a[k] * step[k].loc.pos.eci.a.col[2];
                    }
                }

                for (uint16_t n=1; n<=order2; n++)
                {
                    for (int32_t i=-1; i<2; i+=2)
                    {
                        cflag = 0;

                        // Save current acceleration for comparison with next iteration
                        oldsa.col[0] = step[order2+i*n].loc.pos.eci.a.col[0];
                        oldsa.col[1] = step[order2+i*n].loc.pos.eci.a.col[1];
                        oldsa.col[2] = step[order2+i*n].loc.pos.eci.a.col[2];

                        // Calculate new probable position and velocity
                        step[order2+i*n].loc.pos.eci.v.col[0] = this->dt * (step[order2+i*n].s.col[0] + step[order2+i*n].sb.col[0]);
                        step[order2+i*n].loc.pos.eci.v.col[1] = this->dt * (step[order2+i*n].s.col[1] + step[order2+i*n].sb.col[1]);
                        step[order2+i*n].loc.pos.eci.v.col[2] = this->dt * (step[order2+i*n].s.col[2] + step[order2+i*n].sb.col[2]);
                        step[order2+i*n].loc.pos.eci.s.col[0] = this->dtsq * (step[order2+i*n].ss.col[0] + step[order2+i*n].sa.col[0]);
                        step[order2+i*n].loc.pos.eci.s.col[1] = this->dtsq * (step[order2+i*n].ss.col[1] + step[order2+i*n].sa.col[1]);
                        step[order2+i*n].loc.pos.eci.s.col[2] = this->dtsq * (step[order2+i*n].ss.col[2] + step[order2+i*n].sa.col[2]);

                        // Perform conversions between different systems
                        step[order2+i*n].loc.pos.eci.pass++;
                        Convert::pos_eci(&step[order2+i*n].loc);
                        Convert::att_icrf2lvlh(&step[order2+i*n].loc);
                        //		eci2earth(&step[order2+i*n].loc.pos,&step[order2+i*n].att);

                        // Calculate acceleration at new position
                        PosAccel(&step[order2+i*n].loc, currentphys);

                        // Compare acceleration at new position to previous iteration
                        if (fabs(oldsa.col[0]-step[order2+i*n].loc.pos.eci.a.col[0])>1e-14 || fabs(oldsa.col[1]-step[order2+i*n].loc.pos.eci.a.col[1])>1e-14 || fabs(oldsa.col[2]-step[order2+i*n].loc.pos.eci.a.col[2])>1e-14)
                            cflag = 1;
                    }
                }
                c_cnt++;
            } while (c_cnt<10 && cflag);

            *currentloc = step[order2].loc;
            ++currentloc->pos.eci.pass;
            currentphys->fpush = rv_zero();
            PosAccel(currentloc, currentphys);
            Convert::pos_eci(currentloc);
            return 0;
        }

        //! Calculate dynamic physical attributes
        //! Calculate various derived physical quantities that change, like heat, power generation, torque and drag
        //! \param loc Pointer to ::locstruc
        //! \param phys Pointer to ::physstruc
        //! \return Zero, or negative error.
        int32_t PhysCalc(Convert::locstruc* loc, physicsstruc* phys)
        {
            Vector unitv = Quaternion(loc->att.geoc.s).irotate(Vector(loc->pos.geoc.v).normalize());
            Vector units = Quaternion(loc->att.icrf.s).irotate(Vector(loc->pos.icrf.s).normalize());
            Vector unite = Quaternion(loc->att.icrf.s).irotate(Vector(loc->pos.eci.s).normalize(-1.));
            Vector geov(loc->pos.geoc.v);
            double speed = geov.norm();
            double density;
            if (loc->pos.geod.s.h < 10000. || std::isnan(loc->pos.geod.s.h))
                density = 1.225;
            else
                density = 1000. * Msis00Density(loc->pos, 150., 150., 3.);
            double adrag = density * 1.1 * speed * speed;

            // External panel effects
            phys->adrag.clear();
            phys->atorque.clear();
            phys->rdrag.clear();
            phys->rtorque.clear();
            double surftemp = 310. - 80. * sin(loc->pos.geod.s.lat);
            for (trianglestruc& triangle : phys->triangles)
            {
                triangle.sirradiation = 0.;
                triangle.eirradiation = 0.;
                if (triangle.external)
                {
                    // Atmospheric effects
                    double vdot = unitv.dot(triangle.normal);
                    double sdot = units.dot((triangle.external==1?1.:-1.)*triangle.normal);
                    if (vdot > 0. && phys->mass > 0.F)
                    {
                        double ddrag = adrag * vdot / phys->mass;
                        Vector dtorque = ddrag * triangle.twist;
                        phys->atorque += dtorque;
                        Vector da = ddrag * triangle.shove;
                        phys->adrag += da;
                    }

                    // Solar effects
                    double sirradiation;
                    if (sdot > 0. && phys->mass > 0.F)
                    {
                        sirradiation = loc->pos.sunradiance * sdot;
                        triangle.sirradiation = triangle.pcell * sirradiation;
                        double ddrag = sirradiation / (3e8*phys->mass);
                        Vector dtorque = ddrag * triangle.twist;
                        phys->rtorque += dtorque;
                        Vector da = ddrag * triangle.shove;
                        phys->rdrag += da;
                    }

                    // Earth effects
                    double edot;
                    if (triangle.external == 1)
                    {
                        edot = acos(unite.dot(triangle.normal) / triangle.normal.norm()) - RADOF(5.);
                    }
                    else
                    {
                        edot = acos(unite.dot(-triangle.normal) / triangle.normal.norm()) - RADOF(5.);
                    }
                    if (edot < 0.)
                    {
                        edot = 1.;
                    }
                    else
                    {
                        edot = cos(edot);
                    }

                    double eirradiation = edot * SIGMA * pow(surftemp,4);
                    if (eirradiation > 0)
                    {
                        triangle.eirradiation += triangle.area * triangle.pcell * eirradiation;
                    }

                }
            }
            return 0;
        }

        //! Calculate static physical attributes
        //! Calculate various derived physical quantities, like Center of Mass and Moments of Inertia
        //! \param loc Pointer to ::locstruc
        //! \param phys Pointer to ::physstruc
        //! \return Zero, or negative error.
        int32_t PhysSetup(physicsstruc* phys)
        {
            // Calculate Center of mass and internal area
            phys->area = 0.;
            phys->com.clear();
            phys->mass = 0.;
            for (trianglestruc triangle : phys->triangles)
            {
                if (triangle.external <= 1)
                {
                    phys->area += triangle.area;
                }
                if (!triangle.external)
                {
                    phys->area += triangle.area;
                }
                phys->com += triangle.com * triangle.mass;
                phys->mass += triangle.mass;
            }
            if (phys->mass == 0.F)
            {
                return GENERAL_ERROR_TOO_LOW;
            }
            phys->com /= phys->mass;

            // Calculate Principal Moments of Inertia WRT COM
            phys->moi.clear();
            for (trianglestruc triangle : phys->triangles)
            {
                phys->moi.x += (phys->com.x - triangle.com.x) * (phys->com.x - triangle.com.x) * triangle.mass;
                phys->moi.y += (phys->com.y - triangle.com.y) * (phys->com.y - triangle.com.y) * triangle.mass;
                phys->moi.z += (phys->com.z - triangle.com.z) * (phys->com.z - triangle.com.z) * triangle.mass;
            }

            return 0;
        }

        //! Attitude acceleration
        /*! Calculate the torque forces on the specified satellite at the specified location/
            \param physics Pointer to structure specifying satellite.
            \param loc Structure specifying location.
        */
        int32_t AttAccel(Convert::locstruc &loc, physicsstruc &phys)
        {
            return AttAccel(&loc, &phys);
        }

        int32_t AttAccel(Convert::locstruc *loc, physicsstruc *phys)
        {
            //    rvector ue, ta, tv;
            //    rvector ttorque;
            Vector ue, ta, tv;
            Vector ttorque;
            rmatrix mom;

            Convert::att_extra(loc);

            ttorque = phys->ctorque;

            // Now calculate Gravity Gradient Torque
            // Unit vector towards earth, rotated into body frame
            //    ue = irotate((loc->att.icrf.s),rv_smult(-1.,loc->pos.eci.s));
            //    normalize_rv(ue);
            ue = Quaternion(loc->att.icrf.s).irotate(-1. * Vector(loc->pos.eci.s)).normalize();

            //    phys->gtorque = rv_smult((3.*GM/pow(loc->pos.geos.s.r,3.)),rv_cross(ue,rv_mult(phys->moi,ue)));
            phys->gtorque = (3. * GM / pow(loc->pos.geos.s.r,3.)) * ue.cross(phys->moi * ue);

            //    ttorque = rv_add(ttorque,phys->gtorque);
            ttorque += phys->gtorque;

            // Atmospheric and solar torque
            //	ttorque = rv_add(ttorque,phys->atorque);
            //	ttorque = rv_add(ttorque,phys->rtorque);

            // Torque from rotational effects

            // Moment of Inertia in Body frame
            mom = rm_diag(phys->moi.to_rv());
            // Attitude rate in Body frame
            tv = irotate(loc->att.icrf.s,loc->att.icrf.v);

            // Torque from cross product of angular velocity and angular momentum
            //    phys->htorque = rv_smult(-1., rv_cross(tv,rv_add(rv_mmult(mom,tv),phys->hmomentum)));
            //    ttorque = rv_add(ttorque,phys->htorque);
            phys->htorque = -1. * tv.cross(Vector(rv_mmult(mom, tv.to_rv())) + phys->hmomentum);
            ttorque += phys->htorque;

            // Convert torque into accelerations, doing math in Body frame

            // I x alpha = tau, so alpha = I inverse x tau
            //    ta = rv_mmult(rm_inverse(mom),ttorque);
            ta = Vector(rv_mmult(rm_inverse(mom),ttorque.to_rv()));

            // Convert body frame acceleration back to other frames.
            loc->att.icrf.a = irotate(q_conjugate(loc->att.icrf.s), ta.to_rv());
            loc->att.topo.a = irotate(q_conjugate(loc->att.topo.s), ta.to_rv());
            loc->att.lvlh.a = irotate(q_conjugate(loc->att.lvlh.s), ta.to_rv());
            loc->att.geoc.a = irotate(q_conjugate(loc->att.geoc.s), ta.to_rv());
            loc->att.selc.a = irotate(q_conjugate(loc->att.selc.s), ta.to_rv());
            return 0;
        }

        static const uint8_t GravityPGM2000A = 1;
        static const uint8_t GravityEGM2008 = 2;
        static const uint8_t GravityPGM2000A_NORM = 3;
        static const uint8_t GravityEGM2008_NORM = 4;

        //! Position acceleration
        /*! Calculate the linear forces on the specified sattelite at the specified location/
            \param phys Pointer to structure specifying satellite.
            \param loc Structure specifying location.
        */
        int32_t PosAccel(Convert::locstruc &loc, physicsstruc &phys)
        {
            return PosAccel(&loc, &phys);
        }

        int32_t PosAccel(Convert::locstruc* loc, physicsstruc* phys)
        {
            int32_t iretn = 0;
            double radius;
            Vector ctpos, da, tda;
            Convert::cartpos bodypos;

            radius = length_rv(loc->pos.eci.s);

            loc->pos.eci.a = rv_zero();
            Convert::pos_eci2geoc(*loc);

            // Earth gravity
            // Calculate Geocentric acceleration vector

            if (radius > REARTHM)
            {
                // Start with gravity vector in ITRS

                da = GravityAccel(loc->pos,GravityEGM2008_NORM,12);

                // Correct for earth rotation, polar motion, precession, nutation

                da = Matrix(loc->pos.extra.e2j) * da;
            }
            else
            {
                // Simple 2 body
                da = -GM/(radius*radius*radius) * Vector(loc->pos.eci.s);
            }
            loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());

            // Sun gravity
            // Calculate Satellite to Sun vector
            ctpos = rv_sub(rv_smult(-1., loc->pos.extra.sun2earth.s), loc->pos.eci.s);
            radius = ctpos.norm();
            da = GSUN/(radius*radius*radius) * ctpos;
            loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());

            // Adjust for acceleration of frame
            radius = length_rv(loc->pos.extra.sun2earth.s);
            da = rv_smult(GSUN/(radius*radius*radius), loc->pos.extra.sun2earth.s);
            tda = da;
            loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());

            // Moon gravity
            // Calculate Satellite to Moon vector
            bodypos.s = rv_sub( loc->pos.extra.sun2earth.s, loc->pos.extra.sun2moon.s);
            ctpos = rv_sub(bodypos.s, loc->pos.eci.s);
            radius = ctpos.norm();
            da = GMOON/(radius*radius*radius) * ctpos;
            loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());

            // Adjust for acceleration of frame due to moon
            radius = length_rv(bodypos.s);
            da = rv_smult(GMOON/(radius*radius*radius),bodypos.s);
            tda -= da;
            loc->pos.eci.a = rv_sub(loc->pos.eci.a, da.to_rv());

            // Add thrust
            //            loc->pos.eci.a = rv_add(loc->pos.eci.a, rv_smult(1./phys->mass, phys->fpush.to_rv()));

            /*
        // Jupiter gravity
        // Calculate Satellite to Jupiter vector
        Convert::jplpos(JPL_EARTH,JPL_JUPITER, loc->pos.extra.tt,(Convert::cartpos *)&bodypos);
        ctpos = rv_sub(bodypos.s, loc->pos.eci.s);
        radius = length_rv(ctpos);

        // Calculate acceleration
        da = rv_smult(GJUPITER/(radius*radius*radius),ctpos);
        // loc->pos.eci.a = rv_add( loc->pos.eci.a,da);
        */


            Quaternion iratt = Quaternion(loc->att.icrf.s).conjugate();
            if (phys->adrag.norm() > 0.)
            {
                da = iratt.irotate(phys->adrag);
                loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
            }
            // Solar drag
            if (phys->rdrag.norm() > 0.)
            {
                da = iratt.irotate(phys->rdrag);
                loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
            }
            // Fictitious drag
            if (phys->fdrag.norm() > 0.)
            {
                da = iratt.irotate(phys->fdrag);
                loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
            }
            // Thrust
            if (phys->thrust.norm() > 0.)
            {
                da = iratt.irotate(phys->thrust);
                loc->pos.eci.a = rv_add(loc->pos.eci.a, da.to_rv());
            }

            loc->pos.eci.pass++;
            iretn = Convert::pos_eci(loc);
            if (iretn < 0)
            {
                return iretn;
            }
            if (std::isnan( loc->pos.eci.a.col[0]))
            {
                loc->pos.eci.a.col[0] = 0.;
            }
            if (std::isnan( loc->pos.eci.a.col[1]))
            {
                loc->pos.eci.a.col[1] = 0.;
            }
            if (std::isnan( loc->pos.eci.a.col[2]))
            {
                loc->pos.eci.a.col[2] = 0.;
            }
            return 0;
        }

        //! Calculate atmospheric density
        /*! Calculate atmospheric density at indicated Latitute/Longitude/Altitude using the
         * NRLMSISE-00 atmospheric model.
            \param pos Structure indicating position
            \param f107avg Average 10.7 cm solar flux
            \param f107 Current 10.7 cm solar flux
            \param magidx Ap daily geomagnetic index
            \return Density in kg/m3
        */
        double Msis00Density(Convert::posstruc pos, float f107avg, float f107, float magidx)
        {
            struct nrlmsise_output output;
            struct nrlmsise_input input;
            struct nrlmsise_flags flags = {
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.},
            {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.}};
            int year, month;
            double day, doy;
            static double lastmjd = 0.;
            static double lastdensity = 0.;
            static double lastperiod = 0.;

            if (lastperiod != 0.)
            {
                if (fabs(lastperiod) > (pos.extra.utc-lastmjd))
                {
                    return (lastdensity*(1.+(.001*(pos.extra.utc-lastmjd)/lastperiod)));
                }
            }

            mjd2ymd(pos.extra.utc,year,month,day,doy);
            input.doy = static_cast <int32_t>(doy);
            input.g_lat = pos.geod.s.lat*180./DPI;
            input.g_long = pos.geod.s.lon*180./DPI;
            input.alt = pos.geod.s.h / 1000.;
            input.sec = (doy - input.doy)*86400.;;
            input.lst = input.sec / 3600. + input.g_long / 15.;
            input.f107A = f107avg;
            input.f107 = f107;
            input.ap = magidx;
            gtd7d(&input,&flags,&output);

            if (lastdensity != 0. && lastdensity != output.d[5])
                lastperiod = (pos.extra.utc-lastmjd)*.001*output.d[5]/(output.d[5]-lastdensity);
            lastmjd = pos.extra.utc;
            lastdensity = output.d[5];
            return((double)output.d[5]);
        }

        //! Data structures for spherical harmonic expansion
        /*! Coefficients for real and imaginary components of expansion. Of order and rank maxdegree
        */
        static const uint16_t maxdegree = 360;
        static double vc[maxdegree+1][maxdegree+1], wc[maxdegree+1][maxdegree+1];
        static double coef[maxdegree+1][maxdegree+1][2];
        static double ftl[2*maxdegree+1];
        static double spmm[maxdegree+1];

        //! Spherical harmonic  gravitational vector
        /*!
        * Calculates a spherical harmonic expansion of the chosen model of indicated order and
        * degree for the requested position.
        * The result is returned as a geocentric vector calculated at the epoch.
            \param pos a ::posstruc providing the position at the epoch
            \param model Model to use for coefficients
            \param degree Order and degree to calculate
            \return A ::Vector pointing toward the earth
            \see pgm2000a_coef.txt
        */
        Vector GravityAccel(Convert::posstruc pos, uint16_t model, uint32_t degree)
        {
            uint32_t il, im;
            double tmult;
            double ratio, rratio, xratio, yratio, zratio;
            Vector accel;
            double fr;

            // Zero out vc and wc
            memset(vc,0,sizeof(vc));
            memset(wc,0,sizeof(wc));

            // Load Params
            GravityParams(model);

            // Calculate cartesian Legendre terms
            vc[0][0] = REARTHM/pos.geos.s.r;
            wc[0][0] = 0.;
            ratio = vc[0][0] / pos.geos.s.r;
            rratio = REARTHM * ratio;
            xratio = pos.geoc.s.col[0] * ratio;
            yratio = pos.geoc.s.col[1] * ratio;
            zratio = pos.geoc.s.col[2] * ratio;
            vc[1][0] = zratio * vc[0][0];
            wc[1][0] = 0.;
            for (il=2; il<=degree+1; il++)
            {
                vc[il][0] = (2*il-1)*zratio * vc[il-1][0] / il - (il-1) * rratio * vc[il-2][0] / il;
                wc[il][0] = 0.;
            }
            for (im=1; im<=degree+1; im++)
            {
                vc[im][im] = (2*im-1) * (xratio * vc[im-1][im-1] - yratio * wc[im-1][im-1]);
                wc[im][im] = (2*im-1) * (xratio * wc[im-1][im-1] + yratio * vc[im-1][im-1]);
                if (im <= degree)
                {
                    vc[im+1][im] = (2*im+1) * zratio * vc[im][im];
                    wc[im+1][im] = (2*im+1) * zratio * wc[im][im];
                }
                for (il=im+2; il<=degree+1; il++)
                {
                    vc[il][im] = (2*il-1) * zratio * vc[il-1][im] / (il-im) - (il+im-1) * rratio * vc[il-2][im] / (il-im);
                    wc[il][im] = (2*il-1) * zratio * wc[il-1][im] / (il-im) - (il+im-1) * rratio * wc[il-2][im] / (il-im);
                }
            }

            //	dr = dlon = dlat = 0.;

            accel.clear();
            for (im=0; im<=degree; im++)
            {
                for (il=im; il<=degree; il++)
                {
                    if (im == 0)
                    {
                        accel[0] -= coef[il][0][0] * vc[il+1][1];
                        accel[1] -= coef[il][0][0] * wc[il+1][1];
                        accel[2] -= (il+1) * (coef[il][0][0] * vc[il+1][0]);
                    }
                    else
                    {
                        fr = ftl[il-im+2] / ftl[il-im];
                        accel[0] -= .5 * (coef[il][im][0] * vc[il+1][im+1] + coef[il][im][1] * wc[il+1][im+1] - fr * (coef[il][im][0] * vc[il+1][im-1] + coef[il][im][1] * wc[il+1][im-1]));
                        accel[1] -= .5 * (coef[il][im][0] * wc[il+1][im+1] - coef[il][im][1] * vc[il+1][im+1] + fr * (coef[il][im][0] * wc[il+1][im-1] - coef[il][im][1] * vc[il+1][im-1]));
                        accel[2] -= (il-im+1) * (coef[il][im][0] * vc[il+1][im] + coef[il][im][1] * wc[il+1][im]);
                    }
                }
            }
            tmult = GM / (REARTHM*REARTHM);
            accel[0] *= tmult;
            accel[2] *= tmult;
            accel[1] *= tmult;

            return (accel);
        }

        int32_t GravityParams(int16_t model)
        {
            static int16_t cmodel = -1;

            int32_t iretn = 0;
            uint32_t il, im;
            double norm;
            uint32_t dil, dim;
            double dummy1, dummy2;

            // Calculate factorial
            if (ftl[0] == 0.)
            {
                ftl[0] = 1.;
                for (il=1; il<2*maxdegree+1; il++)
                {
                    ftl[il] = il * ftl[il-1];
                }
            }

            // Load Coefficients
            if (cmodel != model)
            {
                coef[0][0][0] = 1.;
                coef[0][0][1] = 0.;
                coef[1][0][0] = coef[1][0][1] = 0.;
                coef[1][1][0] = coef[1][1][1] = 0.;
                string fname;
                FILE *fi;
                switch (model)
                {
                case GravityEGM2008:
                case GravityEGM2008_NORM:
                    fname = get_cosmosresources();
                    if (fname.empty())
                    {
                        return GENERAL_ERROR_EMPTY;
                    }
                    fname += "/general/egm2008_coef.txt";
                    fi = fopen(fname.c_str(),"r");

                    if (fi==nullptr)
                    {
                        cout << "could not load file " << fname << endl;
                        return iretn;
                    }

                    for (il=2; il<101; il++)
                    {
                        for (im=0; im<= il; im++)
                        {
                            iretn = fscanf(fi,"%u %u %lf %lf\n",&dil,&dim,&coef[il][im][0],&coef[il][im][1]);
                            if (iretn && model == GravityEGM2008_NORM)
                            {
                                norm = sqrt(ftl[il+im]/((2-(im==0?1:0))*(2*il+1)*ftl[il-im]));
                                coef[il][im][0] /= norm;
                                coef[il][im][1] /= norm;
                            }
                        }
                    }
                    fclose(fi);
                    cmodel = model;
                    break;
                case GravityPGM2000A:
                case GravityPGM2000A_NORM:
                default:
                    iretn = get_cosmosresources(fname);
                    if (iretn < 0)
                    {
                        return iretn;
                    }
                    fname += "/general/pgm2000a_coef.txt";
                    fi = fopen(fname.c_str(),"r");
                    for (il=2; il<361; il++)
                    {
                        for (im=0; im<= il; im++)
                        {
                            iretn = fscanf(fi,"%u %u %lf %lf %lf %lf\n",&dil,&dim,&coef[il][im][0],&coef[il][im][1],&dummy1,&dummy2);
                            if (iretn && model == GravityPGM2000A_NORM)
                            {
                                norm = sqrt(ftl[il+im]/((2-(il==im?1:0))*(2*il+1)*ftl[il-im]));
                                coef[il][im][0] /= norm;
                                coef[il][im][1] /= norm;
                            }
                        }
                    }
                    fclose(fi);
                    cmodel = model;
                    break;
                }
            }
            return 0;
        }

        double nplgndr(uint32_t l, uint32_t m, double x)
        {
            double fact,pll,pmm,pmmp1,omx2, oldfact;
            uint16_t i, ll, mm;
            static double lastx = 10.;
            static uint16_t lastm = 65535;

            pll = 0.;
            if (lastm == 65535 || m > lastm || x != lastx)
            {
                lastx = x;
                lastm = m;
                mm = m;
                //	for (mm=0; mm<=maxdegree; mm++)
                //		{
                pmm=1.0;
                if (mm > 0)
                {
                    omx2=((1.0-x)*(1.0+x));
                    fact=1.0;
                    for (i=1;i<=mm;i++)
                    {
                        pmm *= fact*omx2/(fact+1.);
                        fact += 2.0;
                    }
                }
                pmm = sqrt((2.*m+1.)*pmm);
                if (mm%2 == 1)
                    pmm = - pmm;
                spmm[mm] = pmm;
                //		}
            }

            pmm = spmm[m];
            if (l == m)
                return pmm;
            else {
                pmmp1=x*sqrt(2.*m+3.)*pmm;
                if (l == (m+1))
                    return pmmp1;
                else {
                    oldfact = sqrt(2.*m+3.);
                    for (ll=m+2;ll<=l;ll++)
                    {
                        fact = sqrt((4.*ll*ll-1.)/(ll*ll-m*m));
                        pll=(x*pmmp1-pmm/oldfact)*fact;
                        oldfact = fact;
                        pmm=pmmp1;
                        pmmp1=pll;
                    }
                    return pll;
                }
            }
        }

        double rearth(double lat)
        {
            double st,ct;
            double c;

            st = sin(lat);
            ct = cos(lat);
            c = sqrt(((FRATIO2 * FRATIO2 * st * st) + (ct * ct))/((ct * ct) + (FRATIO2 * st * st)));
            return (REARTHM * c);
        }

        Convert::locstruc shape2eci(double utc, double altitude, double angle, double timeshift)
        {
            return shape2eci(utc, 0., 0., altitude, angle, timeshift);
        }

        Convert::locstruc shape2eci(double utc, double latitude, double longitude, double altitude, double angle, double timeshift)
        {
            Convert::locstruc loc;

            //            longitude += 2. * DPI * (fabs(hour)/24. - (utc - (int)utc));

            Convert::pos_clear(loc);

            // Determine effects of oblate spheroid
            double ct = cos(latitude);
            double st = sin(latitude);
            double c = 1./sqrt(ct * ct + FRATIO2 * st * st);
            double s = FRATIO2 * c;
            double r = (rearth(0.) * c + altitude) * ct;
            double z = ((rearth(0.) * s + altitude) * st);
            double radius = sqrt(r * r + z * z);
            double phi = asin(z / radius);

            // Adjust for problems
            if (phi > angle)
            {
                phi = angle;
            }

            // Initial position
            Vector s0(radius, 0., 0.);
            double velocity = sqrt(GM/radius) - cos(angle) * radius * D2PI / 86400.;
            Vector v0(0., velocity, 0.);

            // First, rotate around X vector by angle
            Quaternion q1 = drotate_around_x(angle);
            Vector s1 = q1.drotate(s0);
            Vector v1 = q1.drotate(v0);

            double angle2;
            if (angle)
            {
                angle2 = asin(sin(phi) / sin(angle));
            }
            else
            {
                angle2 = 0.;
            }

            // Second, rotate around L vector by angle2, determine change imposed on longitude, then adjust for timeshift
            Vector L = (s1).cross(v1);
            Quaternion q2 = drotate_around(L, angle2);
            Vector s2 = q2.drotate(s1);
            double deltal = atan2(s2.y, s2.x);

//            q2 = drotate_around(L, angle2 + 0.960 * timeshift * sqrt(GM/pow(radius,3.)));
            q2 = drotate_around(L, angle2 + timeshift * sqrt(GM/pow(radius,3.)));
            s2 = q2.drotate(s1);
            Vector v2 = q2.drotate(v1);


            // Third, rotate around Z vector by remaining distance, related to timeshift, longitude, and  change incurred from angle2)
//            Quaternion q3 = drotate_around_z(-0.00 * timeshift*(D2PI/86400.) + longitude - deltal);
            Quaternion q3 = drotate_around_z(longitude - deltal);
            Vector s3 = q3.drotate(s2);
            Vector v3 = q3.drotate(v2);

            loc.pos.geoc.utc = loc.att.geoc.utc = utc;
            loc.pos.geoc.s.col[0] = s3.x;
            loc.pos.geoc.v.col[0] = v3.x;
            loc.pos.geoc.s.col[1] = s3.y;
            loc.pos.geoc.v.col[1] = v3.y;
            loc.pos.geoc.s.col[2] = s3.z;
            loc.pos.geoc.v.col[2] = v3.z;
            loc.pos.geoc.pass++;
            Convert::pos_geoc(loc);

            //            Convert::kepstruc kep;
            //            Convert::eci2kep(loc.pos.eci, kep);
            //            if (timeshift != 0.)
            //            {
            //                Convert::kepstruc kep;
            //                double dea;
            //                Convert::eci2kep(loc.pos.eci, kep);
            //                kep.ma += timeshift * kep.mm;
            //                uint16_t count = 0;
            //                do
            //                {
            //                    dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
            //                    kep.ea -= dea;
            //                } while (++count < 100 && fabs(dea) > .000001);
            //                kep2eci(kep, loc.pos.eci);
            //                loc.pos.eci.pass++;
            //                Convert::pos_eci(loc);
            //            }


            return loc;
        }
    }

}
