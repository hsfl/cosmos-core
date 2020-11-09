#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "device/general/ic9100_lib.h"

static ic9100_handle ic9100;
static Agent *agent;
static std::string nodename="";
static std::string radioname;
static size_t deviceindex;
static size_t radioindex = 9999;
static string radiodevice="";
static uint16_t radioaddr;
static bool radioconnected = false;
static bool radioenabled = false;
static float freqoffset;
static string port;

static uint16_t nodetype = NODE_TYPE_SATELLITE;
static uint16_t model = DEVICE_MODEL_DIRECT;
static uint16_t radiotype = static_cast<uint16_t>(DeviceType::NONE);
static float freq;
static float band;
static uint8_t opmode = static_cast<uint8_t>(DEVICE_RADIO_MODE_UNDEF);

double value1 = 0.;
double value2 = 0.;
string command = "";


int main(int argc, char *argv[])
{
    int32_t iretn;

    switch (argc)
    {
    case 9:
        nodetype = NODE_TYPE_GROUNDSTATION;
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
        else if (static_cast<string>("direct") == argv[3])
        {
            model = static_cast<uint16_t>(DEVICE_MODEL_DIRECT);
            radiodevice = argv[8];
            radiodevice = "";
        }
    case 8:
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
    default:
        printf("Usage: add_radio radioname [nodename  [ic9100/astrodev txr/rxr/tcv frequency bandwidth mode {device:addr}]]\n");
        exit (1);
        break;
    }

    // Establish the command channel and heartbeat
    agent = new Agent(nodename);

    if ((iretn = agent->wait()) < 0)
    {
        fprintf(agent->get_debug_fd(), "%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }

    if (argc > 6)
    {
        agent->cinfo->node.type = nodetype;
        switch (radiotype)
        {
        case static_cast<uint16_t>(DeviceType::TXR):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::TXR);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex].txr.didx;
            break;
        case static_cast<uint16_t>(DeviceType::RXR):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::RXR);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex].rxr.didx;
            break;
        case static_cast<uint16_t>(DeviceType::TCV):
            iretn = json_createpiece(agent->cinfo, radioname, DeviceType::TCV);
            deviceindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
            radioindex = agent->cinfo->device[deviceindex].tcv.didx;
            break;
        }
        agent->cinfo->device[deviceindex].all.model = model;
        agent->cinfo->device[deviceindex].all.type = radiotype;
        agent->cinfo->device[deviceindex].tcv.freq = freq;
        agent->cinfo->device[deviceindex].tcv.band = band;
        agent->cinfo->device[deviceindex].tcv.opmode = opmode;
        if (model == static_cast<uint16_t>(DEVICE_MODEL_IC9100))
        {
            agent->cinfo->device[deviceindex].all.addr = radioaddr;
            iretn = json_createport(agent->cinfo, radiodevice, PORT_TYPE_RS232);
            if (iretn >= 0)
            {
                agent->cinfo->device[deviceindex].all.portidx = iretn;
            }
        }
        else if (model == static_cast<uint16_t>(DEVICE_MODEL_TS2000))
        {
            iretn = json_createport(agent->cinfo, radiodevice, PORT_TYPE_RS232);
            if (iretn >= 0)
            {
                agent->cinfo->device[deviceindex].all.portidx = iretn;
            }
        }
        else if (model == static_cast<uint16_t>(DEVICE_MODEL_USRP))
        {
            agent->cinfo->device[deviceindex].all.addr = radioaddr;
            iretn = json_createport(agent->cinfo, radiodevice, PORT_TYPE_UDP);
            if (iretn >= 0)
            {
                agent->cinfo->device[deviceindex].all.portidx = iretn;
            }
        }
        iretn = json_dump_node(agent->cinfo);
    }

}
