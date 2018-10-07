#include "support/configCosmos.h"
#include "support/objlib.h"

wavefront obj;
vector <string> part;
string name = "testu";

int main(int argc, char *argv[])
{
    uint16_t usize = 3;
    char variant = 'l';

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
        obj.add_1u("u1lf", Vector(.05, .05, .1));
        obj.add_1u("u1rf", Vector(.05, -.05, .1));
        obj.add_1u("u2lf", Vector(.05, .05, 0.));
        obj.add_1u("u2rf", Vector(.05, -.05, 0.));
        obj.add_1u("u3lf", Vector(.05, .05, -.1));
        obj.add_1u("u3rf", Vector(.05, -.05, -.1));
        obj.add_1u("u1lb", Vector(-.05, .05, .1));
        obj.add_1u("u1rb", Vector(-.05, -.05, .1));
        obj.add_1u("u2lb", Vector(-.05, .05, 0.));
        obj.add_1u("u2rb", Vector(-.05, -.05, 0.));
        obj.add_1u("u3lb", Vector(-.05, .05, -.1));
        obj.add_1u("u3rb", Vector(-.05, -.05, -.1));
        switch (variant)
        {
        case 'a':
            obj.add_cuboid("cosmos.x+panel", {.003, .2, .3}, eyeQ(), {.1, 0., 0.});
            obj.add_cuboid("cosmos.x-panel", {.003, .2, .3}, eyeQ(), {-.1, 0., 0.});
            obj.add_cuboid("cosmos.y+panel", {.2, .003, .3}, eyeQ(), {0., .1, 0.});
            obj.add_cuboid("cosmos.y-panel", {.2, .003, .3}, eyeQ(), {0., -.1, 0.});
            obj.add_cuboid("cosmos.z+panel", {.2, .2, .003}, eyeQ(), {0., 0., 1.5});
            obj.add_cuboid("cosmos.z-panel", {.2, .2, .003}, eyeQ(), {0., 0., -1.5});
            break;
        default:
            break;
        }
        break;
    case 6:
        obj.add_1u("u1l", Vector(0., .05, .1));
        obj.add_1u("u1r", Vector(0., -.05, .1));
        obj.add_1u("u2l", Vector(0., .05, 0.));
        obj.add_1u("u2r", Vector(0., -.05, 0.));
        obj.add_1u("u3l", Vector(0., .05, -.1));
        obj.add_1u("u3r", Vector(0., -.05, -.1));
        switch (variant)
        {
        case 'a':
            obj.add_cuboid("cosmos.x+panel", {.003, .2, .3}, eyeQ(), {.05, 0., 0.});
            obj.add_cuboid("cosmos.x-panel", {.003, .2, .3}, eyeQ(), {-.05, 0., 0.});
            obj.add_cuboid("cosmos.y+panel", {.1, .003, .3}, eyeQ(), {0., .1, 0.});
            obj.add_cuboid("cosmos.y-panel", {.1, .003, .3}, eyeQ(), {0., -.1, 0.});
            obj.add_cuboid("cosmos.z+panel", {.1, .2, .003}, eyeQ(), {0., 0., 1.5});
            obj.add_cuboid("cosmos.z-panel", {.1, .2, .003}, eyeQ(), {0., 0., -1.5});
            break;
        default:
            break;
        }
        break;
    case 3:
        obj.add_1u("u1", Vector(0., 0., .1));
        obj.add_1u("u2", Vector(0., 0., 0.));
        obj.add_1u("u3", Vector(0., 0., -.1));
        switch (variant)
        {
        case 'a':
            obj.add_cuboid("cosmos.x+panel", {.003, .1, .3}, eyeQ(), {.05, 0., 0.});
            obj.add_cuboid("cosmos.x-panel", {.003, .1, .3}, eyeQ(), {-.05, 0., 0.});
            obj.add_cuboid("cosmos.y+panel", {.1, .003, .3}, eyeQ(), {0., .05, 0.});
            obj.add_cuboid("cosmos.y-panel", {.1, .003, .3}, eyeQ(), {0., -.05, 0.});
            obj.add_cuboid("cosmos.z+panel", {.1, .1, .003}, eyeQ(), {0., 0., 1.5});
            obj.add_cuboid("cosmos.z-panel", {.1, .1, .003}, eyeQ(), {0., 0., -1.5});
            break;
        default:
            break;
        }
        break;
    case 1:
    default:
        obj.add_1u("u1", Vector(0., 0., 0.));
        switch (variant)
        {
        case 'a':
            obj.add_cuboid("cosmos.x+panel", {.003, .1, .1}, eyeQ(), {.05, 0., 0.});
            obj.add_cuboid("cosmos.x-panel", {.003, .1, .1}, eyeQ(), {-.05, 0., 0.});
            obj.add_cuboid("cosmos.y+panel", {.1, .003, .1}, eyeQ(), {0., .05, 0.});
            obj.add_cuboid("cosmos.y-panel", {.1, .003, .1}, eyeQ(), {0., -.05, 0.});
            obj.add_cuboid("cosmos.z+panel", {.1, .1, .003}, eyeQ(), {0., 0., .05});
            obj.add_cuboid("cosmos.z-panel", {.1, .1, .003}, eyeQ(), {0., 0., -.05});
            break;
        default:
            break;
        }
        break;
    }

    obj.save_file(".", name);
}
