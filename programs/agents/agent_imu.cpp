/* **************************************************************************
 * File/Desc:           agent_imu.cpp
 * Description:         IMU Agent for HiakaSat OBC
 * Target Device/Freq:	Hiakasat OBC
 * By:                  20150700MN: -see SVN log-
 * Updated:             20141015JC: Implemented new per-device state machine methods
 * For:                 HawaiiSat-1 Mission, Hawaii Space Flight Laboratory
 * Dependancies:
 * IDE:                 N/A
 * Special Compilation:
 * Problems:
 * Other Comments:
 * ************************************************************************** */

// *********************************************************************
// Instructions
// *********************************************************************
// To start the agent on the flight computer:
// $ agent_isc hiakasat 192.168.201.20 /dev/ttyO0


// *********************************************************************
// Libraries
// *********************************************************************
#include "configCosmos.h"

// COSMOS Core Libraries
#include "agentlib.h"
#include "jsonlib.h"
#include "vn100_lib.h"
#include "print_utils.h"
#include "math/rotation.h"
#include "math/mathlib.h"

#define UNUSED_VARIABLE(x) (void)(x)

// Debug Var
bool debug_flag = false;
bool broadcast_flag = false;

// *********************************************************************
// Preprocessor Macros
// *********************************************************************
// Version Information
#define APP_NAME    "IMU Agent"
#define VERSION     "0.2"

// COSMOS Definitions
#define COSMOS_NODE_NAME    "hiakasat"
#define COSMOS_AGENT_NAME   "imu"

cosmosstruc *cdata;
#define AGENTSVR_MAXBUF_BYTES 10000

// IMU Broadcast POST TYPE: HS1_CPOST_TYPE_HIAKASAT_IMU



// *********************************************************************
// IMU Specific Info
// *********************************************************************
//#define ACCELX  (cdata->devspec.imu[0]->accel.col[0])
//#define ACCELY  (cdata->devspec.imu[0]->accel.col[1])
//#define ACCELZ  (cdata->devspec.imu[0]->accel.col[2])
//#define OX      (cdata->devspec.imu[0]->omega.col[0])
//#define OY      (cdata->devspec.imu[0]->omega.col[1])
//#define OZ      (cdata->devspec.imu[0]->omega.col[2])
//#define MAGX    (cdata->devspec.imu[0]->mag.col[0])
//#define MAGY    (cdata->devspec.imu[0]->mag.col[1])
//#define MAGZ    (cdata->devspec.imu[0]->mag.col[2])

#define ACCELX  (newimu.accel.col[0])
#define ACCELY  (newimu.accel.col[1])
#define ACCELZ  (newimu.accel.col[2])
#define OX      (newimu.omega.col[0])
#define OY      (newimu.omega.col[1])
#define OZ      (newimu.omega.col[2])
#define MAGX    (newimu.mag.col[0])
#define MAGY    (newimu.mag.col[1])
#define MAGZ    (newimu.mag.col[2])
#define MAGMAG  sqrt(MAGX * MAGX + MAGY*MAGY + MAGZ*MAGZ)

#define MEAS_TIMESTAMP   (last_imu_measurement_mjd)
double last_imu_measurement_mjd;

class DataImu {
public:
    rvector mag;
    double magTotal;
    rvector acc;
    rvector gyro;
};

#define AVERAGE_COUNT 10



lsfit lastimuomega(4, 2, .0003);
lsfit lastimumag(4, 2, .001);

double log_period=0.;
std::string imuDataString;

// Calibration Values
float vn100magscale[3];
float vn100magbias[3];
float vn100omegabias[3];

// For measurements
DCM dcm;
basisOrthonormal frame_body;
basisOrthonormal frame_imu;
cmatrix R_body_from_imu;
imustruc newimu;
double magTotal;

// Device handle and overall mutex
std::mutex imu_mutex;
vn100_handle vn100handle;


// IMU routines
int32_t imu_init_data();
int32_t imu_take_measurement();
int32_t imu_post_to_broadcast();
int32_t imu_post_to_user_screen();

// *********************************************************************
// Data Logging
// *********************************************************************
std::string log_string;
// for the log file of this program
double utc_agent_start = 0;




// *********************************************************************
// State Machine
// *********************************************************************

// Vars
int imu_fsm_state;
int imu_consec_fail_count;
double imu_last_contact_mjd;
DataImu dataImu;




// Macros
#define HARD_CODED_DEVICE_PATH                  "/dev/ttyIMU"
#define IMU_MUTEX_LOCK                          imu_mutex.lock();
#define IMU_MUTEX_UNLOCK                        imu_mutex.unlock();
#define IMU_CONSEC_FAIL_COUNT_INCREMENT         { imu_consec_fail_count++; if (imu_consec_fail_count > 30000) imu_consec_fail_count = 30000; }
#define IMU_CONSEC_FAIL_COUNT_RESET             { imu_consec_fail_count = 0; }
#define IMU_CONTACT_SUCCESS                     { imu_consec_fail_count = 0; imu_last_contact_mjd = currentmjd(); }
#define MUTEX_IMU_CONSEC_FAIL_COUNT_INCREMENT   { IMU_MUTEX_LOCK; IMU_CONSEC_FAIL_COUNT_INCREMENT; IMU_MUTEX_UNLOCK; }
#define MUTEX_IMU_CONSEC_FAIL_COUNT_RESET       { IMU_MUTEX_LOCK; IMU_CONSEC_FAIL_COUNT_RESET; IMU_MUTEX_UNLOCK; }
#define MUTEX_IMU_CONTACT_SUCCESS               { IMU_MUTEX_LOCK; IMU_CONTACT_SUCCESS; IMU_MUTEX_UNLOCK; }
#define IMU_STATE_CLOSED                        0
#define IMU_STATE_OPEN                          1
#define IMU_STATE_SIMULATED                     2

#define IMU_SIM_OMEGA_SELECT_OPTITRACK_DATA      0
#define IMU_SIM_OMEGA_SELECT_IMU_DATA            1
#define IMU_SIM_OMEGA_SELECT                     IMU_SIM_OMEGA_SELECT_IMU_DATA

// State machine functions
int fail_count_read();
void run_imu_handle_open();
void run_imu_handle_closed();
void imu_handle_init();
void imu_state_to_open();
void imu_state_to_closed();
void run_imu_simulated();


// Request Support Routines
//int req_string_parse_3arg_float(char *string, char *respstr, float *arg1, float *arg2, float *arg3, float limit_low, float limit_hi);
//int req_string_parse_1arg_float(char *string, char *respstr, float *arg, float limit_low, float limit_hi);

// COSMOS Agent Server
//#define REQ_VMT_STATE_CHECK if (vmt_fsm_state != VMT_STATE_OPEN) { sprintf(response, "[ERROR] ST Disconnected.  Aborting.\n"); return 0; }
int32_t request_mag(char *request, char* response, void *cdata);
int32_t request_all(char *request, char* response, void *cdata);
int32_t request_imufltdata(char *request, char* response, void *root);
int32_t request_debug(char *request, char *response, void *cdata);

int main(int argc, char *argv[])
{
    // No command line inputs
    UNUSED_VARIABLE(argc);
    UNUSED_VARIABLE(argv);
    int iretn;

    // Debug
    //debug_print = false;

    // *********************************************************************
    // Command Line Argument Processing
    // *********************************************************************


    // *********************************************************************
    // Splash
    // *********************************************************************
    printf("\n================================================\n");
    printf("%s at Node/Name:[%s:%s]\n", APP_NAME, COSMOS_NODE_NAME, COSMOS_AGENT_NAME);
    printf("Version %s built on %s %s\n", VERSION, __DATE__, __TIME__);
    printf("================================================\n\n");


    // *********************************************************************
    // Set-Up COSMOS Agent Server
    // *********************************************************************
    // Initialize the Agent
    if (!(cdata = agent_setup_server(NetworkType::UDP, COSMOS_NODE_NAME, COSMOS_AGENT_NAME, .2, 0, AGENTSVR_MAXBUF_BYTES,(bool)false)))
        exit (AGENT_ERROR_JSON_CREATE);
    printf("- Agent server started\n");

    // Add Requests
    if ((iretn=agent_add_request(cdata,"mag",request_mag,"Magnetic Field Data Only\n\t\tRespFormat: MAGxyz(+mag)[uT],MJD")))
        exit (iretn);
    if ((iretn=agent_add_request(cdata,"all",request_all,"All IMU Data\n\t\tRespFormat: MAGxyz(+mag)[uT],Accel[g]xyz,AngRatesXYZ[r/s],MJD")))
        exit (iretn);
    if ((iretn=agent_add_request(cdata,"fltdata",request_imufltdata,"Only IMU Flight Data\n\t\tRespFormat: MAG[uT]xyz,AngRates[r/s]xyz,,MJD")))
        exit (iretn);
    if ((iretn=agent_add_request(cdata,"debug",request_debug,"IMU Debug information")))
        exit (iretn);

    // Setup Heartbeat information
    char imu_soh[2000] = "{\"device_imu_utc_000\",\"device_imu_omega_000\",\"device_imu_mag_000\",\"device_imu_bdot_000\"}";
    agent_set_sohstring(cdata, imu_soh);



    // *********************************************************************
    // Initialize Logging
    // *********************************************************************
    // Record Start Time for Logging
    utc_agent_start = currentmjd();

    // Open calibration file for scaling of VN100
    std::string cnodedir = get_nodedir(cdata[0].node.name);
    std::string filename = cnodedir + "/vn100.ini";

    FILE *fp;
    if ((fp=fopen(filename.c_str(),"r")) != NULL)
    {
        iretn = fscanf(fp, "%f %f %f\n", &vn100magscale[0], &vn100magscale[1], &vn100magscale[2]);
        iretn = fscanf(fp, "%f %f %f\n", &vn100magbias[0], &vn100magbias[1], &vn100magbias[2]);
        iretn = fscanf(fp, "%f %f %f\n", &vn100omegabias[0], &vn100omegabias[1], &vn100omegabias[2]); // in rad/sec
        fclose(fp);
        std::cout << "[CALIBRATION] Calibration values read from " << filename.c_str() << std::endl;
    }
    else
    {
        // File open failed, set hardcoded bias
        vn100magscale[0] =   0.000016160;
        vn100magscale[1] =   0.000016740;
        vn100magscale[2] =   0.000016660;
        vn100magbias[0] =    0.29;
        vn100magbias[1] =   -0.15;
        vn100magbias[2] =    0.33;
        vn100omegabias[0] =  0.0035;
        vn100omegabias[1] =  0.0001;
        vn100omegabias[2] = -0.0065;

        std::cerr << "[WARNING] Unable to open calibration file at " << filename.c_str() << ". Hardcoded IMU corrections applied." << std::endl;
    }

    //
    imu_init_data();


    // *********************************************************************
    // Initialize State Machine
    // *********************************************************************
    // Initialize IMU state machine
    imu_handle_init();



    // *********************************************************************
    // Main Loop
    // *********************************************************************
    while (agent_running(cdata))
    {
        // Manage connection to IMU
        switch (imu_fsm_state)
        {
        case IMU_STATE_CLOSED:       run_imu_handle_closed(); break;
        case IMU_STATE_OPEN:         run_imu_handle_open(); break;
        default:
            printf("State Error!");
            imu_state_to_closed();
            fflush(stdout);
            break;
        }
        COSMOS_SLEEP(.1);
    }


    // Close out
    agent_shutdown_server(cdata);
    return 0;
}

// ***********************************************************************
// State Machine
// ***********************************************************************
void imu_handle_init()
{
    IMU_MUTEX_LOCK;
    imu_last_contact_mjd = 0;
    imu_consec_fail_count = 0;
    imu_fsm_state = IMU_STATE_CLOSED;
    IMU_MUTEX_UNLOCK;
}



int fail_count_read()
{
    int tmp;
    IMU_MUTEX_LOCK;
    if (imu_consec_fail_count < 0) imu_consec_fail_count = 0;
    tmp = imu_consec_fail_count;
    IMU_MUTEX_UNLOCK;
    return tmp;
}



void run_imu_handle_open()
{
    int32_t iretn;

    if (debug_flag)
    {
        //printf("O");
        //fflush(stdout);
    }

    // Task: Measure IMU
    iretn = imu_take_measurement();
    if (iretn < 0)
    {
        // Failure
        IMU_CONSEC_FAIL_COUNT_INCREMENT;

        if (debug_flag)
        {
            printf("-");
            fflush(stdout);
        }
        //printf("[FAIL] IMU Measurement Failed, Code %d\n", iretn);
    }
    else
    {
        // Success!
        IMU_CONSEC_FAIL_COUNT_RESET;
        if (debug_flag)
        {
            //printf("+");
            //fflush(stdout);
        }

        // Contact!
        imu_last_contact_mjd = currentmjd();
    }


    // Task: Broadcast IMU Data
    if (broadcast_flag)
    {
        imu_post_to_broadcast();
    }

    if (debug_flag)
    {
        //printf("P");
        //fflush(stdout);
    }

    // Task: Print to screen
    if (debug_flag)
    {
        imu_post_to_user_screen();
    }

    // Task: Check for communication failure
    if (fail_count_read() > 4)
    {
        // Failure detected
        printf("[STATE CHANGE] IMU Failure Detected.  State->CLOSED\n\n");
        imu_state_to_closed();
    }
    else
    {
        // Wait 0.090 seconds between telemetry updates (Roughly 10Hz)
        usleep(90000);
    }
}

void run_imu_handle_closed()
{
    int32_t iretn;


    // Task 1: Connect to IMU

    // Phase 1: Attempt to connect to handle
    IMU_MUTEX_LOCK;
    vn100_disconnect(&vn100handle); // Disconnect just in case it was connected
    iretn = vn100_connect(cdata->port[cdata->devspec.imu[0]->gen.portidx].name, &vn100handle);
    IMU_MUTEX_UNLOCK;
    if (iretn < 0)
    {
        // Fail!
        IMU_CONSEC_FAIL_COUNT_INCREMENT;
        printf("[FAIL] Error connecting to IMU, code %d.  Fail count %d\n", iretn, imu_consec_fail_count);
        fflush(stdout);

        IMU_MUTEX_LOCK;
        vn100_disconnect(&vn100handle); // Disconnect for cleanup
        IMU_MUTEX_UNLOCK;

        sleep(1);           // Sleep to wait and try again in a few seconds
    }
    else
    {
        // Success!
        IMU_CONSEC_FAIL_COUNT_RESET;

        // Contact!
        imu_last_contact_mjd = currentmjd();

        printf("[STATE CHANGE] IMU Connect Success!  State->OPEN\n\n");
        imu_state_to_open();
        return;
    }

    return;

}

void imu_state_to_open()
{
    IMU_MUTEX_LOCK;

    imu_consec_fail_count = 0;
    imu_fsm_state = IMU_STATE_OPEN;

    // Flag COSMOS Active
    cdata->devspec.imu[0]->gen.flag = DEVICE_FLAG_ON;
    cdata->devspec.imu[0]->gen.flag |= DEVICE_FLAG_ACTIVE;

    IMU_MUTEX_UNLOCK;
}

void imu_state_to_closed()
{
    IMU_MUTEX_LOCK;

    // Flag COSMOS Inactive
    cdata->devspec.imu[0]->gen.flag = DEVICE_FLAG_OFF;
    cdata->devspec.imu[0]->gen.flag &= ~DEVICE_FLAG_ACTIVE;

    // Disconnect from serial port
    vn100_disconnect(&vn100handle);

    // Reset all data
    imu_consec_fail_count = 0;
    imu_fsm_state = IMU_STATE_CLOSED;

    IMU_MUTEX_UNLOCK;
}


int32_t imu_init_data()
{
    // ------------------------------------------
    // compute IMU sensor frame conversion to body frame
    // initiate DCM class for later use in sensor transformation

    // define body frame base
    frame_body.i = cv_unitx();
    frame_body.j = cv_unity();
    frame_body.k = cv_unitz();

    // define IMU frame
    // IMU+X is aligned with BODY+Y
    // IMU+Y is aligned with BODY+X
    // IMU+Z is aligned with BODY-Z
    frame_imu.i = {0,1,0};
    frame_imu.j = {1,0,0};
    frame_imu.k = {0,0,-1};

    // get the DCM to convert IMU vector into body vector
    R_body_from_imu = dcm.base1_from_base2(frame_body,frame_imu);

    // IMU cdata
    //newimu = *(cdata->devspec.imu[0]);

    return 0;
}


int32_t imu_take_measurement()
{
    int32_t iretn;

    // *****************************************************
    IMU_MUTEX_LOCK;

    // always reinitialize data from flash for radiation tolerance

    // no need to call init all the time
    //imu_init_data();

    // IMU cdata
    newimu = *(cdata->devspec.imu[0]);

    if ((iretn=vn100_measurements(&vn100handle)) >= 0)      // 20150805JC: @~10Hz Runs, This line takes up about 6.1% CPU, rest of program takes about 1.3% CPU
    {
        newimu.gen.utc = currentmjd(0.);
        // get temp data
        newimu.gen.temp = vn100handle.imu.gen.temp/10.;

        //-------------------------------------

        // get gyro data
        newimu.omega = vn100handle.imu.omega;

        // apply gyro bias
        newimu.omega.col[0] -= vn100omegabias[0];
        newimu.omega.col[1] -= vn100omegabias[1];
        newimu.omega.col[2] -= vn100omegabias[2];
        newimu.omega = rv_mmult(rm_from_cm(R_body_from_imu),newimu.omega);

        //-------------------------------------

        // get acceleration data
        newimu.accel = vn100handle.imu.accel;
        //std::cout << "Raw:" << newimu.accel << std::endl;
        newimu.accel = rv_mmult(rm_from_cm(R_body_from_imu), newimu.accel);
        //std::cout << "New:" << newimu.accel << std::endl;

        //-------------------------------------

        // get mag data from handle
        newimu.mag = vn100handle.imu.mag;

        // apply mag bias
        newimu.mag.col[0] -= vn100magbias[0];
        newimu.mag.col[1] -= vn100magbias[1];
        newimu.mag.col[2] -= vn100magbias[2];

        // apply mag scale
        newimu.mag.col[0] *= vn100magscale[0];
        newimu.mag.col[1] *= vn100magscale[1];
        newimu.mag.col[2] *= vn100magscale[2];

        // convert from imu sensor frame to satellite body frame
        newimu.mag = rv_mmult(rm_from_cm(R_body_from_imu),newimu.mag);

        // for agent request
        dataImu.mag = newimu.mag;

        magTotal = length_rv(newimu.mag);

        MEAS_TIMESTAMP = currentmjd();
        cdata->devspec.imu[0]->gen.utc = MEAS_TIMESTAMP;

        // put data into cdata to be broadcasted
        lastimumag.update(MEAS_TIMESTAMP, newimu.mag);
        newimu.mag = lastimumag.evalrvector(MEAS_TIMESTAMP);
        newimu.bdot = rv_smult(1./86400., lastimumag.slopervector(MEAS_TIMESTAMP));
        *(cdata->devspec.imu[0]) = newimu;
        //        cdata->devspec.imu[0]->omega = newimu.omega;
        //        cdata->devspec.imu[0]->mag = newimu.mag;
        //        cdata->devspec.imu[0]->accel = newimu.accel;

    }
    IMU_MUTEX_UNLOCK;
    // *****************************************************

    iretn = 0;
    return iretn;
}

int32_t imu_post_to_user_screen()
{
    IMU_MUTEX_LOCK;
    double mjd = cdata->devspec.imu[0]->gen.utc;
    std::string mjdString = mjdToGregorian(mjd);

    if (debug_flag)
    {
        // User Printout String

        PrintUtils print;
        print.scale = 1e6;
        print.precision = 1;
        print.fieldwidth = 5;
        print.vectorAndMag("MAG[uT],",newimu.mag,",");

        print.precision = 2;
        print.scale = 180./PI;
        print.vectorAndMag("OMEGA[deg/s],",newimu.omega);
        print.endline();
    }

    IMU_MUTEX_UNLOCK;

    return 0;
}

int32_t imu_post_to_broadcast()
{
    int32_t iretn;

    // *****************************************************
    IMU_MUTEX_LOCK;

    // Network String
    imuDataString = std::to_string(newimu.mag.col[0]*1e6) + ","
            + std::to_string (newimu.mag.col[1]*1e6) + ","
            + std::to_string (newimu.mag.col[2]*1e6) + ","
            + std::to_string (magTotal*1e6) + ","
            + std::to_string (newimu.accel.col[0]) + ","
            + std::to_string (newimu.accel.col[1]) + ","
            + std::to_string (newimu.accel.col[2]) + ","
            + std::to_string (newimu.omega.col[0]) + ","
            + std::to_string (newimu.omega.col[1]) + ","
            + std::to_string (newimu.omega.col[2]) + ","
            + std::to_string (MEAS_TIMESTAMP) + ","
            ;

    // post imu information on the network
    // 0xBE for IMU
    iretn = agent_post(cdata, 0xBE, imuDataString);

    IMU_MUTEX_UNLOCK;
    // *****************************************************

    return iretn;
}


int32_t request_mag(char *request, char* response, void *cdata)
{
    UNUSED_VARIABLE(request);
    UNUSED_VARIABLE(cdata);

    // *****************************************************
    IMU_MUTEX_LOCK;
    sprintf(response,"%.17f,%.17f,%.17f,%.17f,%.17f\n",
            MAGX,
            MAGY,
            MAGZ, MAGMAG, MEAS_TIMESTAMP);
    IMU_MUTEX_UNLOCK;
    // *****************************************************

    // must return something!
    return 0;
}

int32_t request_all(char *request, char* response, void *root)
{
    UNUSED_VARIABLE(request);
    UNUSED_VARIABLE(root);

    // *****************************************************
    // Start Locking Attitude Data
    IMU_MUTEX_LOCK;
    // *****************************************************

    sprintf(response,"%.17f,%.17f,%.17f,%.17f,%.17f,%.17f,%.17f,%.17f,%.17f,%.17f,%.17f\n",MAGX,MAGY,MAGZ,MAGMAG,ACCELX,ACCELY,ACCELZ,OX,OY,OZ,MEAS_TIMESTAMP); // FMT:MAGxyzm[uT],Accel[g]xyz,AngRatesXYZ[r/s]

    // *****************************************************
    IMU_MUTEX_UNLOCK;
    // Unlock Attitude Data
    // *****************************************************

    // must return something!
    return 0;
}


int32_t request_imufltdata(char *request, char* response, void *root)
{
    UNUSED_VARIABLE(request);
    UNUSED_VARIABLE(root);

    // *****************************************************
    // Start Locking Attitude Data
    IMU_MUTEX_LOCK;
    // *****************************************************

    sprintf(response,"%.17f,%.17f,%.17f,%.17f,%.17f,%.17f,%.17f\n",MAGX,MAGY,MAGZ,OX,OY,OZ,MEAS_TIMESTAMP); // FMT:MAG[uT]xyz,AngRates[r/s]xyz,MJD

    // *****************************************************
    IMU_MUTEX_UNLOCK;
    // Unlock Attitude Data
    // *****************************************************

    // must return something!
    return 0;
}


// agent hiakasat imu "debug [0/1]"
int32_t request_debug(char *request, char *response, void *cdata)
{

    std::string requestString = std::string(request);
    StringParser sp(requestString, ' ');

    debug_flag = sp.getFieldNumberAsDouble(2); // should be getFieldNumberAsBoolean

    std::cout << "debug_flag: " << debug_flag << std::endl;
	return 0;
}

// agent hiakasat imu "broadcast [0/1]"
int32_t request_broadcast(char *request, char *response, void *cdata)
{

    std::string requestString = std::string(request);
    StringParser sp(requestString, ' ');

    broadcast_flag = sp.getFieldNumberAsDouble(2); // should be getFieldNumberAsBoolean

    std::cout << "broadcast_flag: " << broadcast_flag << std::endl;
	return 0;
}
