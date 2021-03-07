#include <cstring>
#include <cstdio>
#include <string>
#include "envi.h"

int read_envi_hdr(std::string file, envi_hdr &hdr)
{
    char fname[250], inbuf[1000];
    FILE *fp;

    hdr.basename = file;
    sprintf(fname,"%s.hdr",file.data());
    fp = fopen(fname,"r");
    if (fp == nullptr)
    {
        for (size_t i=file.size()-1; i>=0; --i)
        {
            if (file[i] == '.')
            {
                file.resize(i);
                hdr.basename = file;
                sprintf(fname,"%s.hdr",file.data());
                fp = fopen(fname,"r");
                break;
            }
        }
        if (fp == nullptr)
        {
            return (-1);
        }
    }

    hdr.keys.resize(0);
    hdr.values.resize(0);
    while(fgets(inbuf,999,fp) != NULL)
    {
        size_t loc;
        std::string line = inbuf;

        if (line[0] != ';' && (loc=line.find(" = ")) != (std::string::npos))
        {
            if (inbuf[loc+3] == '{')
            {
                if ((loc=line.find("}")) == std::string::npos)
                {
                    while(fgets(inbuf,999,fp) != NULL)
                    {
                        line = inbuf;
                        if ((loc=line.find("}")) != std::string::npos)
                        {
                            break;
                        }
                    }
                }
            }
            else
            {
                size_t vloc = loc + 2;
                for (size_t i=loc+2; i<line.size(); ++i)
                {
                    if (line[i] != ' ')
                    {
                        vloc = i;
                        break;
                    }
                }
                hdr.values.push_back(line.substr(vloc,line.size()-(vloc)));
                for (size_t i=loc; i>0; --i)
                {
                    if (line[i-1] != ' ')
                    {
                        loc = i;
                        break;
                    }
                }
                for (size_t i=0; i<loc; ++i)
                {
                    if (line[i] >= 'A' && line[i] <= 'Z')
                    {
                        line[i] += 'a' - 'A';
                    }
                }
                hdr.keys.push_back(line.substr(0,loc));
            }
        }
    }

    for (size_t i=0; i<hdr.keys.size(); ++i)
    {
        if (hdr.keys[i] == "samples")
        {
            sscanf(hdr.values[i].data(), "%lu",&hdr.columns);
        }
        else if (hdr.keys[i] == "lines")
        {
            sscanf(hdr.values[i].data(), "%lu",&hdr.rows);
        }
        else if (hdr.keys[i] == "bands")
        {
            sscanf(hdr.values[i].data(), "%lu",&hdr.planes);
        }
        else if (hdr.keys[i] == "header offset")
        {
            sscanf(hdr.values[i].data(), "%lu",&hdr.offset);
        }
        else if (hdr.keys[i] == "data type")
        {
            sscanf(hdr.values[i].data(), "%lu",&hdr.datatype);
        }
        else if (hdr.keys[i] == "byte order")
        {
            sscanf(hdr.values[i].data(), "%lu",&hdr.byteorder);
        }
        else if (hdr.keys[i] == "interleave")
        {
            if (!hdr.values[i].compare(0,3,"bsq") || !hdr.values[i].compare(0,3,"BSQ"))
            {
                hdr.interleave = BSQ;
            }
            else if (!hdr.values[i].compare(0,3,"bip") || !hdr.values[i].compare(0,3,"BIP"))
            {
                hdr.interleave = BIP;
            }
            else if (!hdr.values[i].compare(0,3,"bil") || !hdr.values[i].compare(0,3,"BIL"))
            {
                hdr.interleave = BIL;
            }
        }
    }

    fclose(fp);
    return (0);
}

int write_envi_hdr(envi_hdr &hdr)
{
    char fname[1000];
    FILE *fp;

    sprintf(fname,"%s.hdr",hdr.basename.data());
    fp = fopen(fname,"w");
    fprintf(fp,"ENVI\n");
    //    for (size_t k=0; k<hdr.keys.size(); ++k)
    //    {
    //        if (hdr.keys[k] == "description")
    //        {
    //            hdr.description = hdr.values[k];
    //        }
    //        if (hdr.keys[k] == "samples")
    //        {
    //            sscanf(hdr.values[k].data(), "%hu", &hdr.columns);
    //        }
    //        if (hdr.keys[k] == "lines")
    //        {
    //            sscanf(hdr.values[k].data(), "%hu", &hdr.rows);
    //        }
    //        if (hdr.keys[k] == "planes")
    //        {
    //            sscanf(hdr.values[k].data(), "%hu", &hdr.planes);
    //        }
    //        if (hdr.keys[k] == "header offset")
    //        {
    //            sscanf(hdr.values[k].data(), "%u", &hdr.offset);
    //        }
    //        if (hdr.keys[k] == "data type")
    //        {
    //            sscanf(hdr.values[k].data(), "%hu", &hdr.datatype);
    //        }
    //        if (hdr.keys[k] == "interleave")
    //        {
    //            if (hdr.values[k] == "BSQ" || hdr.values[k] == "bsq")
    //            {
    //                hdr.interleave = BSQ;
    //            }
    //            else if (hdr.values[k] == "BIP" || hdr.values[k] == "bip")
    //            {
    //                hdr.interleave = BIP;
    //            }
    //            else if (hdr.values[k] == "BIL" || hdr.values[k] == "bil")
    //            {
    //                hdr.interleave = BIL;
    //            }
    //        }
    //        if (hdr.keys[k] == "byte order")
    //        {
    //            sscanf(hdr.values[k].data(), "%hu", &hdr.byteorder);
    //        }
    //    }

    fprintf(fp,"description = { %s }\n", hdr.description.data());
    fprintf(fp,"samples = %lu\n",hdr.columns);
    fprintf(fp,"lines = %lu\n",hdr.rows);
    fprintf(fp,"bands = %lu\n",hdr.planes);
    fprintf(fp,"header offset = %lu\n",hdr.offset);
    fprintf(fp,"file type = ENVI Standard\n");
    fprintf(fp,"data type = %lu\n",hdr.datatype);
    switch(hdr.interleave)
    {
    case BSQ:
        fprintf(fp,"interleave = bsq\n");
        break;
    case BIP:
        fprintf(fp,"interleave = bip\n");
        break;
    case BIL:
        fprintf(fp,"interleave = bil\n");
        break;
    }
    fprintf(fp,"byte order = %lu\n",hdr.byteorder);
    if (hdr.map_info.size())
    {
        fprintf(fp, "%s\n", hdr.map_info.c_str());
    }
    else
    {
        fprintf(fp,"map info = {Arbitrary, 1.5000, 1.5000, %f, %f, %f, %f, units=Meters)\n",hdr.x0,hdr.y0,hdr.xmpp,hdr.ympp);
    }
    if (hdr.projection_info.size())
    {
        fprintf(fp, "%s\n", hdr.projection_info.c_str());
    }
    if (hdr.band_name.size())
    {
        fprintf(fp,"band names = {\n");
        for (size_t i=0; i<hdr.planes; i++)
        {
            fprintf(fp,"%s",hdr.band_name[i].data());
            if (i!=hdr.planes-1)
            {
                fprintf(fp,",\n");
            }
        }
        fprintf(fp,"}\n");
    }
    if (hdr.wavelength.size())
    {
        fprintf(fp,"wavelength = {\n");
        for (size_t i=0; i<hdr.planes; i++)
        {
            fprintf(fp,"%f",hdr.wavelength[i]);
            if (i!=hdr.planes-1)
            {
                fprintf(fp,",\n");
            }
        }
        fprintf(fp,"}\n");
    }

    fclose(fp);
    return (0);
}
