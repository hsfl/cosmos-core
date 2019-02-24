#include "support/configCosmos.h"
#include "support/objlib.h"

int main(int argc, char **argv)
{
    wavefront wobj;

    wobj.load_file(".", argv[1]);
    for (size_t i=1; i<wobj.Groups.size(); ++i)
    {
        printf("#%d: %s\n", i, wobj.Groups[i].name.c_str());
        if (wobj.Groups[i].pointidx.size())
        {
            printf("Points:");
            for (size_t j=0; j<wobj.Groups[i].pointidx.size(); ++j)
            {
                printf("   %d", wobj.Points[wobj.Groups[i].pointidx[j]].vertex);
            }
            printf("\n");
        }
        if (wobj.Groups[i].lineidx.size())
        {
            printf("Lines:");
            for (size_t j=0; j<wobj.Groups[i].lineidx.size(); ++j)
            {
                printf(" [");
                for (size_t k=0; k<wobj.Lines[wobj.Groups[i].lineidx[j]].vertices.size(); ++k)
                {
                    printf(" %d/%d", wobj.Lines[wobj.Groups[i].lineidx[j]].vertices[k].v, wobj.Lines[wobj.Groups[i].lineidx[j]].vertices[k].vt);
                }
                printf(" ]");
            }
            printf("\n");
        }
        if (wobj.Groups[i].faceidx.size())
        {
            printf("Faces:");
            for (size_t j=0; j<wobj.Groups[i].faceidx.size(); ++j)
            {
                printf(" [");
                for (size_t k=0; k<wobj.Faces[wobj.Groups[i].faceidx[j]].vertices.size(); ++k)
                {
                    printf(" %d/%d/%d", wobj.Faces[wobj.Groups[i].faceidx[j]].vertices[k].v, wobj.Faces[wobj.Groups[i].faceidx[j]].vertices[k].vt, wobj.Faces[wobj.Groups[i].faceidx[j]].vertices[k].vn);
                }
                printf(" ]");
            }
            printf("\n");
        }
    }
}
