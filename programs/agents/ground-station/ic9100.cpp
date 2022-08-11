#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "device/general/ic9100_lib.h"
#include "device/general/usrp_lib.h"

static ic9100_handle ic9100;
static usrp_handle usrp;
static Agent *agent;
static string nodename="";
static string radioname;
static size_t deviceindex;
static size_t radioindex = 9999;
static string radiodevice;
static uint16_t radioaddr;
//static bool radioconnected = false;
//static bool radioenabled = false;
//static float freqoffset;
static string port;

static uint16_t model;
static uint16_t radiotype = static_cast<uint16_t>(DeviceType::NONE);
static float freq;
static float band;
static uint8_t opmode = static_cast<uint8_t>(DEVICE_RADIO_MODE_UNDEF);

double value1 = 0.;
double value2 = 0.;
string command = "";


int main(int argc, char *argv[])
{
    int32_t iretn = 0;

    switch (argc)
    {
    case 9:
        if (static_cast<string>("ic9100") == argv[3])
        {
            model = static_cast<uint16_t>(DEVICE_MODEL_IC9100);
            radiodevice = argv[8];
            radioaddr = stoi(radiodevice.substr(radiodevice.find(':')+1));
            radiodevice = radiodevice.substr(0, radiodevice.find(':'));
        }
        else if (static_cast<string>("astrodev") == argv[3])
        {
            model = static_cast<uint16_t>(DEVICE_MODEL_ASTRODEV);
            radiodevice = argv[8];
        }
        else if (static_cast<string>("usrp") == argv[3])
        {
            model = static_cast<uint16_t>(DEVICE_MODEL_USRP);
            radiodevice = argv[8];
            radioaddr = stoi(radiodevice.substr(radiodevice.find(':')+1));
            radiodevice = radiodevice.substr(0, radiodevice.find(':'));
        }
        if (static_cast<string>("txr") == argv[4])
        {
            radiotype = static_cast<uint16_t>(DeviceType::TXR);
        }
        else if (static_cast<string>("rxr") == argv[4])
        {
            radiotype = static_cast<uint16_t>(DeviceType::RXR);
        }
        else if (static_cast<string>("tcv") == argv[4])
        {
            radiotype = static_cast<uint16_t>(DeviceType::TCV);
        }
        freq = atof(argv[5]);
        band = atof(argv[6]);
        if (static_cast<string>("am") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_AM);
        }
        else if (static_cast<string>("amd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_AMD);
        }
        else if (static_cast<string>("fm") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_FM);
        }
        else if (static_cast<string>("fmd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_FMD);
        }
        else if (static_cast<string>("lsb") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_LSB);
        }
        else if (static_cast<string>("lsbd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_LSBD);
        }
        else if (static_cast<string>("usb") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_USB);
        }
        else if (static_cast<string>("usbd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_USBD);
        }
        else if (static_cast<string>("dv") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_DV);
        }
        else if (static_cast<string>("dvd") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_DVD);
        }
        else if (static_cast<string>("cw") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_CW);
        }
        else if (static_cast<string>("cwr") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_CWR);
        }
        else if (static_cast<string>("rtty") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_RTTY);
        }
        else if (static_cast<string>("rttyr") == argv[7])
        {
            opmode = static_cast<uint16_t>(DEVICE_RADIO_MODE_RTTYR);
        }
        nodename = argv[2];
        radioname = argv[1];
        break;
    case 6:
        nodename = argv[5];
    case 5:
        value2 = atof(argv[4]);
    case 4:
        value1 = atof(argv[3]);
    case 3:
        command = argv[2];
        radioname = argv[1];
        break;
    default:
        printf("Usage: agent->radio radioname [nodename  [ic9100/astrodev txr/rxr/tcv frequency bandwidth mode device:addr]]\n");
        exit (1);
        break;
    }

    // Establish the command channel and heartbeat
    agent = new Agent(nodename);

    if ((iretn = agent->wait()) < 0)
    {
        agent->debug_error.Printf("%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }

    if (argc > 6)
    {
        switch (radiotype)
        {
        case static_cast<uint16_t>(DeviceType::TXR):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::TXR);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex]->didx;
            break;
        case static_cast<uint16_t>(DeviceType::RXR):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::RXR);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex]->didx;
            break;
        case static_cast<uint16_t>(DeviceType::TCV):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::TCV);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex]->didx;
            break;
        }
        agent->cinfo->device[deviceindex]->model = model;
        agent->cinfo->device[deviceindex]->type = radiotype;
        agent->cinfo->devspec.tcv[radioindex].freq = freq;
        agent->cinfo->devspec.tcv[radioindex].band = band;
        agent->cinfo->devspec.tcv[radioindex].opmode = opmode;
        if (model == static_cast<uint16_t>(DEVICE_MODEL_IC9100))
        {
            agent->cinfo->device[deviceindex]->addr = radioaddr;
            iretn = json_createport(agent->cinfo, radiodevice, PORT_TYPE_RS232);
            if (iretn >= 0)
            {
                agent->cinfo->device[deviceindex]->portidx = iretn;
            }
        }
        else if (model == static_cast<uint16_t>(DEVICE_MODEL_TS2000))
        {
            iretn = json_createport(agent->cinfo, radiodevice, PORT_TYPE_RS232);
            if (iretn >= 0)
            {
                agent->cinfo->device[deviceindex]->portidx = iretn;
            }
        }
        else if (model == static_cast<uint16_t>(DEVICE_MODEL_USRP))
        {
            agent->cinfo->device[deviceindex]->addr = radioaddr;
            iretn = json_createport(agent->cinfo, radiodevice, PORT_TYPE_UDP);
            if (iretn >= 0)
            {
                agent->cinfo->device[deviceindex]->portidx = iretn;
            }
        }
        iretn = json_dump_node(agent->cinfo);
    }

    // Look for named radio so we can use the right one
    if (radiotype == static_cast<uint16_t>(DeviceType::NONE))
    {
        for (size_t i=0; i<agent->cinfo->devspec.rxr_cnt; ++i)
        {
//            if (!strcmp(radioname.c_str(), agent->cinfo->pieces[agent->cinfo->devspec.rxr[i].pidx].name))
            if (radioname == agent->cinfo->pieces[agent->cinfo->devspec.rxr[i].pidx].name)
            {
                deviceindex = agent->cinfo->devspec.rxr[i].cidx;
                radioindex = i;
                radiotype = static_cast<uint16_t>(DeviceType::RXR);
                break;
            }
        }
    }

    if (radiotype == static_cast<uint16_t>(DeviceType::NONE))
    {
        for (size_t i=0; i<agent->cinfo->devspec.txr_cnt; ++i)
        {
//            if (!strcmp(radioname.c_str(), agent->cinfo->pieces[agent->cinfo->devspec.txr[i].pidx].name))
            if (radioname == agent->cinfo->pieces[agent->cinfo->devspec.txr[i].pidx].name)
            {
                deviceindex = agent->cinfo->devspec.txr[i].cidx;
                radioindex = i;
                radiotype = static_cast<uint16_t>(DeviceType::TXR);
                break;
            }
        }
    }

    if (radiotype == static_cast<uint16_t>(DeviceType::NONE))
    {
        for (size_t i=0; i<agent->cinfo->devspec.tcv_cnt; ++i)
        {
//            if (!strcmp(radioname.c_str(), agent->cinfo->pieces[agent->cinfo->devspec.tcv[i].pidx].name))
            if (radioname == agent->cinfo->pieces[agent->cinfo->devspec.tcv[i].pidx].name)
            {
                deviceindex = agent->cinfo->devspec.tcv[i].cidx;
                radioindex = i;
                radiotype = static_cast<uint16_t>(DeviceType::TCV);
                break;
            }
        }
    }

    if (radiotype == static_cast<uint16_t>(DeviceType::NONE))
    {
        std::cout<<"Exiting " << radioname << " for Node: " << nodename << " no radio found." << std::endl;
        agent->shutdown();
        exit (1);
    }

    if (agent->cinfo->device[deviceindex]->model == DEVICE_MODEL_IC9100)
    {
        iretn = ic9100_connect(agent->cinfo->port[agent->cinfo->device[deviceindex]->portidx].name, agent->cinfo->device[deviceindex]->addr, ic9100);
        if (iretn < 0)
        {
            printf("Unable to connect to IC9100: %s\n", cosmos_error_string(iretn).c_str());
            exit(iretn);
        }

        iretn = ic9100_set_channel(ic9100, 0);
        if (iretn < 0)
        {
            printf("Unable to set IC9100 to Main: %s\n", cosmos_error_string(iretn).c_str());
            exit(iretn);
        }
        iretn = ic9100_get_frequency(ic9100);

        if (command == "freq")
        {
            if (argc == 3)
            {
                iretn = ic9100_get_frequency(ic9100);
                if (iretn < 0)
                {
                    printf("Unable to get IC9100 Frequency: %s\n", cosmos_error_string(iretn).c_str());
                }
                else {
                    printf("Frequency: %.0f Hz\n", ic9100.frequency);
                }
            }
            else {
                iretn = ic9100_set_frequency(ic9100, value1);
                if (iretn < 0)
                {
                    printf("Unable to set IC9100 Frequency: %s\n", cosmos_error_string(iretn).c_str());
                }
                else {
                    iretn = ic9100_get_frequency(ic9100);
                    printf("Frequency: %.0f Hz\n", ic9100.frequency);
                }
            }
        }
        else if (command == "rfpower")
        {
            iretn = ic9100_get_rfpower(ic9100);
            if (iretn < 0)
            {
                printf("Unable to get IC9100 RFPower: %s\n", cosmos_error_string(iretn).c_str());
            }
            else {
                printf("RFPower: %hhu\n", ic9100.rfpower);
            }
        }
    }
    else if (agent->cinfo->device[deviceindex]->model == DEVICE_MODEL_USRP)
    {
        iretn = usrp_connect(agent->cinfo->port[agent->cinfo->device[deviceindex]->portidx].name, agent->cinfo->device[deviceindex]->addr, usrp);
        if (command == "freq")
        {
            if (argc == 3)
            {
                iretn = usrp_get_frequency(usrp);
                if (iretn < 0)
                {
                    printf("Unable to get USRP Frequency: %s\n", cosmos_error_string(iretn).c_str());
                }
                else {
                    printf("Frequency: %.0f Hz\n", usrp.frequency);
                }
            }
            else {
                iretn = usrp_set_frequency(usrp, value1);
                if (iretn < 0)
                {
                    printf("Unable to set USRP Frequency: %s\n", cosmos_error_string(iretn).c_str());
                }
                else {
                    iretn = usrp_get_frequency(usrp);
                    printf("Frequency: %.0f Hz\n", usrp.frequency);
                }
            }
        }
    }

}
