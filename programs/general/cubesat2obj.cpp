#include "support/configCosmos.h"
#include "support/objlib.h"

wavefront obj;
vector <string> part;
string name = "testu";

int main(int argc, char *argv[])
{
    uint16_t usize = 3;
    char variant = 'a';

    switch (argc)
    {
    case 4:
        variant = argv[3][0];
    case 3:
        usize = atoi(argv[2]);
    case 2:
        name = argv[1];
        break;
    }

    switch (usize)
    {
    case 12:
        obj.add_3u("u12lf", Vector(.05, .05, 0.));
        obj.add_3u("u12rf", Vector(.05, -.05, 0.));
        obj.add_3u("u12lr", Vector(-.05, .05, 0.));
        obj.add_3u("u12rr", Vector(-.05, -.05, 0.));
        switch (variant)
        {
        case 'b':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.003, .2, .3), Math::Quaternions::eye(), Vector(.1, 0., 0.));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.003, .2, .3), Math::Quaternions::eye(), Vector(-.1, 0., 0.));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.2, .003, .3), Math::Quaternions::eye(), Vector(0., .1, 0.));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.2, .003, .3), Math::Quaternions::eye(), Vector(0., -.1, 0.));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.2, .2, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.2, .2, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'c':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.3, .2, .003), Math::Quaternions::eye(), Vector(.25, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.3, .2, .003), Math::Quaternions::eye(), Vector(-.25, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.2, .003, .3), Math::Quaternions::eye(), Vector(0., .1, 0.));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.2, .003, .3), Math::Quaternions::eye(), Vector(0., -.1, 0.));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.2, .2, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.2, .2, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'd':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.3, .2, .003), Math::Quaternions::eye(), Vector(.25, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.3, .2, .003), Math::Quaternions::eye(), Vector(-.25, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.2, .3, .003), Math::Quaternions::eye(), Vector(0., .25, .15));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.2, .3, .003), Math::Quaternions::eye(), Vector(0., -.25, .15));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.2, .2, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.2, .2, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'a':
        default:
            break;
        }
        break;
    case 6:
        obj.add_3u("u6l", Vector(0., .05, 0.));
        obj.add_3u("u6r", Vector(0., -.05, 0.));
        switch (variant)
        {
        case 'b':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.003, .2, .3), Math::Quaternions::eye(), Vector(.05, 0., 0.));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.003, .2, .3), Math::Quaternions::eye(), Vector(-.05, 0., 0.));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.1, .003, .3), Math::Quaternions::eye(), Vector(0., .1, 0.));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.1, .003, .3), Math::Quaternions::eye(), Vector(0., -.1, 0.));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.1, .2, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.1, .2, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'c':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.3, .2, .003), Math::Quaternions::eye(), Vector(.2, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.3, .2, .003), Math::Quaternions::eye(), Vector(-.2, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.1, .003, .3), Math::Quaternions::eye(), Vector(0., .1, 0.));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.1, .003, .3), Math::Quaternions::eye(), Vector(0., -.1, 0.));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.1, .2, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.1, .2, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'd':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.3, .2, .003), Math::Quaternions::eye(), Vector(.2, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.3, .2, .003), Math::Quaternions::eye(), Vector(-.2, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.1, .3, .003), Math::Quaternions::eye(), Vector(0., .25, .15));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.1, .3, .003), Math::Quaternions::eye(), Vector(0., -.25, .15));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.1, .2, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.1, .2, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'a':
        default:
            break;
        }
        break;
    case 3:
        obj.add_3u("u3", Vector(0., 0., 0.));
        switch (variant)
        {
        case 'b':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.003, .1, .3), Math::Quaternions::eye(), Vector(.05, 0., 0.));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.003, .1, .3), Math::Quaternions::eye(), Vector(-.05, 0., 0.));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.1, .003, .3), Math::Quaternions::eye(), Vector(0., .05, 0.));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.1, .003, .3), Math::Quaternions::eye(), Vector(0., -.05, 0.));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.1, .1, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.1, .1, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'c':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.3, .1, .003), Math::Quaternions::eye(), Vector(.2, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.3, .1, .003), Math::Quaternions::eye(), Vector(-.2, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.1, .003, .3), Math::Quaternions::eye(), Vector(0., .05, 0.));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.1, .003, .3), Math::Quaternions::eye(), Vector(0., -.05, 0.));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.1, .1, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.1, .1, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'd':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.3, .1, .003), Math::Quaternions::eye(), Vector(.2, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.3, .1, .003), Math::Quaternions::eye(), Vector(-.2, 0., .15));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.1, .3, .003), Math::Quaternions::eye(), Vector(0., .2, .15));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.1, .3, .003), Math::Quaternions::eye(), Vector(0., -.2, .15));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.1, .1, .003), Math::Quaternions::eye(), Vector(0., 0., .15));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.1, .1, .003), Math::Quaternions::eye(), Vector(0., 0., -.15));
            break;
        case 'a':
        default:
            break;
        }
        break;
    case 1:
    default:
        obj.add_1u("u1", Vector(0., 0., 0.));
        switch (variant)
        {
        case 'b':
            obj.add_cuboid("cosmos.pvstrg.x+panel", Vector(.003, .1, .1), Math::Quaternions::eye(), Vector(.05, 0., 0.));
            obj.add_cuboid("cosmos.pvstrg.x-panel", Vector(.003, .1, .1), Math::Quaternions::eye(), Vector(-.05, 0., 0.));
            obj.add_cuboid("cosmos.pvstrg.y+panel", Vector(.1, .003, .1), Math::Quaternions::eye(), Vector(0., .05, 0.));
            obj.add_cuboid("cosmos.pvstrg.y-panel", Vector(.1, .003, .1), Math::Quaternions::eye(), Vector(0., -.05, 0.));
            obj.add_cuboid("cosmos.pvstrg.z+panel", Vector(.1, .1, .003), Math::Quaternions::eye(), Vector(0., 0., .05));
            obj.add_cuboid("cosmos.pvstrg.z-panel", Vector(.1, .1, .003), Math::Quaternions::eye(), Vector(0., 0., -.05));
            break;
        case 'a':
        default:
            break;
        }
        break;
    }

    obj.save_file(".", name);
}
