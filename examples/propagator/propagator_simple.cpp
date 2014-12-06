// This is a simple propagator program without COSMOS node dependecy (is that possible?)
// , it starts propagating from a given state
// (load from cosmosroot-state.ini?)

#include <sys/stat.h>
#include <typeinfo>

#include "physicslib.h"
#include "mathlib.h"
#include "nodelib.h"
#include "agentlib.h"
#include "jsonlib.h"
#include "datalib.h"
#include "print_utils.h"

// ------------------------------------------------------------------
// User config

// config agent
string node_name = "cubesat1";
string agent_name = "propagator_simple";

//bool    use_agent       = true;
//bool    use_propagator   = true; //

// ------------------------------------------------------------------
// Other vars

// general purpose buffer
char buffer[255] = "";

cosmosstruc* cdata;
vector<shorteventstruc> eventdict;
vector<shorteventstruc> events;
jstring mainjstring={0,0,0};

void printMjdAndDateTime(double mjd){
    cout << setprecision(10) << mjd << " (" << mjd2human(mjd) << ")"; // << endl;
}

int main(int argc, char* argv[]){

    // for propagator
    int32_t order   = 6; // integration order
    int32_t mode    = 1; // attitude mode (0 - propagate?, 1-LVLH, ...)
    double dt       = 1; // >> check with Eric .1 or 1?

    double triger_time = 0;
    double iteration_rate = 1; //in sec
    double trigger_offset_ms = 10; // ms before sending the command
    int precision = 1/iteration_rate;
    double sleep_time = 0.7*iteration_rate;
    double set_time = 0;
    double elapsed_seconds = 0;
    double utc_now;
    int32_t iretn;


    // for time profiling
    ElapsedTime ep;
    ep.print = false;

    PrintUtils print;

    locstruc state; // Set state container
    locstruc initState; // Set initial state

    // break down state vector
    double x = state.pos.eci.s.col[0];
    double y = state.pos.eci.s.col[1];
    double z = state.pos.eci.s.col[2];

    double vx = state.pos.eci.v.col[0];
    double vy = state.pos.eci.v.col[1];
    double vz = state.pos.eci.v.col[2];

    double q1 = state.att.icrf.s.d.x;
    double q2 = state.att.icrf.s.d.y;
    double q3 = state.att.icrf.s.d.z;
    double q4 = state.att.icrf.s.w;

    // --------------------------------------------------------------

    switch (argc)
    {
    case 2:
        //node_name = argv[1];
        break;
    case 3:
        //mjdstart = atof(argv[2]);
        break;
    default:
        //cout << "Usage: propogator nodename [mjd|0]" << endl;
        //exit (-1);
        break;
    }

    cout << "-----------------------------------------------" << endl;
    cout << "|      COSMOS Propagator Example              |" << endl;
    cout << "-----------------------------------------------" << endl;

    //if(use_agent){

        // Establish the command channel and heartbeat
        if (!(cdata = agent_setup_server(AGENT_TYPE_UDP, node_name.c_str(), agent_name.c_str(),1.,0,AGENTMAXBUFFER)))
        {
            cout<<"agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
            exit (AGENT_ERROR_JSON_CREATE);
        }

        //cdata->physics.mode = mode;
        //json_clone(cdata);

        //load_dictionary(eventdict, cdata, (char *)"events.dict");

    //}


    ep.tic();

    // ------------------------------------
    // load state.ini

    cout << "-----------------------------------------------" << endl;
    cout << "Loading state.ini info from " << node_name << " node (node must exist in cosmosroot) "  << endl;

    struct stat fstat;
    FILE* fdes;
    string fname = get_cnodedir((node_name.c_str()));
    fname += "/state.ini";

    pos_clear(&initState);

    if ((iretn=stat(fname.c_str(), &fstat)) == 0 && (fdes=fopen(fname.c_str(),"r")) != NULL)
    {
        char* ibuf = (char *)calloc(1,fstat.st_size+1);
        //size_t nbytes =
        fread(ibuf, 1, fstat.st_size, fdes);
        //		fgets(ibuf,fstat.st_size,fdes);
        json_parse(ibuf,cdata);
        free(ibuf);

        initState.pos.eci   = cdata->node.loc.pos.eci;
        //initState.att.icrf  = cdata->node.loc.att.icrf;
        initState.utc       = cdata->node.loc.pos.eci.utc;

        cout << "Sucessfully found state.ini"  << endl;

        cout << "UTC from state.ini   : ";
        printMjdAndDateTime(initState.utc);
        print.end();
        print.vector("Initial ECI Position : ", initState.pos.eci.s, " m", 3);print.end();
        print.vector("Initial ECI Velocity : ", initState.pos.eci.v, " m/s",3);print.end();
        //print.end();
        cout << "-----------------------------------------------" << endl;

    }
    else
    {
        printf("Unable to open state.ini\n");
        exit (-1);
    }

    ep.toc("load state.ini");

    //cout << "UTC now              : ";
    //printMjdAndDateTime(currentmjd());
    //print.end();

    // propagate the changes to all frames
    initState.pos.eci.pass++;
    pos_eci(&initState);

    // initialize propagator
    gauss_jackson_init_eci(order,
                           mode,
                           dt,
                           currentmjd(),// use curretn time instead of initState.utc for this demo, otherwise it will take a long time to update
                           initState.pos.eci,
                           initState.att.icrf,
                           cdata);

    // propagate state to current time so we get an updated state vector
    // to initialize the GPS sim
    gauss_jackson_propagate(cdata, currentmjd());

    //get initial sim tim
    double mjd_start_sim = currentmjd();

    // --------------------------------------------------------------
    //while(1){ // for general purpose
    while (agent_running(cdata)){ //for agent use
        // get the elapsed seconds from the sim start
        utc_now = currentmjd(0);
        elapsed_seconds = (utc_now - mjd_start_sim)*86400; //+ 50./1000.

        set_time = floor(elapsed_seconds*precision)/precision + iteration_rate;
        triger_time = set_time - trigger_offset_ms/1000.;

        if (elapsed_seconds > triger_time){ // send the command 100 ms before the set time

            // propagate
            gauss_jackson_propagate(cdata, utc_now);
            state = cdata->node.loc;

            // break down state vector for this demo
            x = state.pos.eci.s.col[0];
            y = state.pos.eci.s.col[1];
            z = state.pos.eci.s.col[2];

            vx = state.pos.eci.v.col[0];
            vy = state.pos.eci.v.col[1];
            vz = state.pos.eci.v.col[2];

            //
            q1 = state.att.geoc.s.d.x;
            q2 = state.att.geoc.s.d.y;
            q3 = state.att.geoc.s.d.z;
            q4 = state.att.geoc.s.w;

            sprintf(buffer,"%s,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                    mjd2human3(utc_now).c_str(),
                    x,y,z,
                    vx,vy,vz,
                    q1,q2,q3,q4);

            // magnetic field in Tesla
            //magField = cdata->node.loc.bearth;

            // print stuff
            //cout << "------------------------------------------------" << endl;
            cout << "UTC : ";
            printMjdAndDateTime(currentmjd());
            cout << " | ";
            cout << seconds2DDHHMMSS(elapsed_seconds) << " | ";
            print.vector("pos:", state.pos.geoc.s, " m | ", 3);
            print.vector("vel:", state.pos.geoc.v, " m/s | ", 3);
            print.vector("mag field:", cdata->node.loc.bearth, 1e6, " nT", 3);
            print.end();

            COSMOS_SLEEP(sleep_time); // sleep for 70% of the iteration time

        }

    } // end while

    return iretn;
}





