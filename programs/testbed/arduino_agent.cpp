/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

/**********************************************************************************************
 * arduino_agent.cpp
 * ********************************************************************************************/
#include "support/configCosmos.h"

#if !defined(COSMOS_MAC_OS) && !defined(COSMOS_WIN_OS)
//#include <sys/vfs.h>
#endif

#include <stdio.h>

#include "support/jsonlib.h"
#include "agent/agentclass.h"
#include "support/cosmos-defs.h"
#include "physics/physicslib.h"
#include "math/mathlib.h"
#include "support/jsonlib.h"
#include "device/arduino/arduino_lib.h"

typedef struct {
    int 	controllerFlag ;
    int 	motorFlag [4] ;
    float	pidRoll[3] ;
    float   pidPitch[3] ;
    float   pidYaw[3] ;
    float 	angles[3] ;
    float	throttle ;
} copterstruc ;



int agent_arduino();

// request functions 
int32_t request_mjd(char *request, char* response, Agent *);
int32_t request_type(char *request, char* response, Agent *);
int32_t request_pitch(char *request, char* response, Agent *);
int32_t request_yaw(char *request, char* response, Agent *);
int32_t request_roll(char *request, char* response, Agent *);
int32_t request_motor(char *request, char* response, Agent *);
int32_t request_speed (char *request, char* response, Agent *);
int32_t request_throttle (char *request, char* response, Agent *);
int32_t request_throttlemin (char *request, char* response, Agent *);
int32_t request_throttlemax (char *request, char* response, Agent *);
int32_t request_printangles (char *request, char* response, Agent *);
int32_t request_printthrottle (char *request, char* response, Agent *);
int32_t request_pidroll (char *request, char* response, Agent *);
int32_t request_pidpitch (char *request, char* response, Agent *);
int32_t request_pidyaw (char *request, char* response, Agent *);
int32_t request_typepitch (char *request, char* response, Agent *);
int32_t request_typeroll (char *request, char* response, Agent *);
int32_t request_typeyaw (char *request, char* response, Agent *);
int32_t request_printpidpitch (char *request, char* response, Agent *);
int32_t request_printpidroll (char *request, char* response, Agent *);
int32_t request_printpidyaw (char *request, char* response, Agent *);
int32_t request_typeall (char *request, char* response, Agent *);
int32_t request_controller (char *request, char *response, Agent *agent) ;


void init_copterstruc () ;
void getAngles () ;
void getMotorSpeed () ;
void sendToSerial (char *istr, char *ostr) ;
void sendToSerial2 (char *istr, char *ostr, char *ostr1) ;
void parseOutstring (char *) ;
void flushport () ;



char agentname[COSMOS_MAX_NAME+1] = "arduino";
char ipaddress[16] = "192.168.150.1";
int waitsec = 5;
copterstruc myCopter ;
Agent *agent;
float tiltval = 0 ;

// flags that will be used
// PID Types 0-pitch 1-roll 2-yaw
int setSpeedFlag, setThrottleFlag, setAnglesFlag, setPIDFlag, setPIDTypeFlag ;
int setControllerFlag, setMotorFlag ;
// strings for passing requests to the loop thread
char pidstr[100], reqstring[100], contstring[40], motorstring[40], throttlestring[40], speedstring[40], pidtypestring[40] ;

int myfd ;  // this is the arduino fileDescriptor returne from serialport_init



#define MAXBUFFERSIZE 256

#define REQUEST_RUN_PROGRAM 0

// Here are some variables we will map to JSON names
uint16_t myport;

int main(int argc, char *argv[])
{
    int 	iretn ;

    setSpeedFlag = 0 ;
    setAnglesFlag = 0 ;
    setThrottleFlag = 0 ;
    setControllerFlag = 0 ;
    setMotorFlag = 0 ;

    init_copterstruc () ;

    // Initialize Agent
    if (!(agent = new Agent("arduino", agentname, .1, MAXBUFFERSIZE)) != 0)
        exit (AGENT_ERROR_JSON_CREATE);

    agent->cinfo->pdata.node.loc.pos.geod.v.lat = agent->cinfo->pdata.node.loc.pos.geod.v.lon = agent->cinfo->pdata.node.loc.pos.geod.v.h = 0.;
    agent->cinfo->pdata.node.loc.pos.geod.a.lat = agent->cinfo->pdata.node.loc.pos.geod.a.lon = agent->cinfo->pdata.node.loc.pos.geod.a.h = 0.;
    //agent->cinfo->pdata.node.loc.pos.geod.s = agent->cinfo->pdata.node.track[0].loc;
    agent->cinfo->pdata.node.loc.pos.geod.utc = currentmjd(0);
    agent->cinfo->pdata.node.loc.pos.geod.s.h += 100.;
    pos_geod(&agent->cinfo->pdata.node.loc);

    // Check for other instance of this agent
    if (argc == 2)
        strcpy(agentname,argv[1]);


    // Initialization stuff
    // arduino connection
    myfd = serialport_init ((char const *)"/dev/ttyACM1", 57600) ;
    // wait a second


    // Add internal requests
    if ((iretn=agent->add_request("printroll",request_roll)) != 0)
        exit (iretn);
    if ((iretn=agent->add_request("printyaw",request_yaw)) != 0)
        exit (iretn);
    if ((iretn=agent->add_request("printtype",request_type)) != 0)
        exit (iretn);
    if ((iretn=agent->add_request("printpitch",request_pitch)) != 0)
        exit (iretn);
    if ((iretn=agent->add_request("mjd", request_mjd)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("printangles", request_printangles)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("printthrottle", request_printthrottle)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("speed", request_speed)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("motor", request_motor)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("throttle", request_throttle)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("throttle_min", request_throttlemin)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("throttle_max", request_throttlemax)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("pid_roll", request_pidroll)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("pid_pitch", request_pidpitch)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("pid_yaw", request_pidyaw)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("typeyaw", request_typeyaw)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("typeroll", request_typeroll)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("typepitch", request_typepitch)) !=0)
        exit (iretn) ;

    if ((iretn=agent->add_request("printpidpitch", request_printpidpitch)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("printpidroll", request_printpidroll)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("printpidyaw", request_printpidyaw)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("typeall", request_typeall)) !=0)
        exit (iretn) ;
    if ((iretn=agent->add_request("controller", request_controller)) !=0)
        exit (iretn) ;
    // Start our own thread
    iretn = agent_arduino();
}

int agent_arduino()
{
    char 	outstr [240], outstr1[240], fname[120] ;
    int		oldday, iyear, iday ;
    double 	year, day, cmjd, nmjd, period;
    unsigned long usec;
    std::string jstring;
    oldday = -1 ;

    //FILE *flog = NULL ;
    // Initialize loop timing
    period = .1/86400.;
    cmjd = currentmjd();
    nmjd = cmjd + period;
    oldday = -1 ;

    // Start performing the body of the agent
    // Information gathering thing....
    //
    //
    // activate all motors ...
    //sendToSerial ("#SET,MOTOR,5*", outstr) ;
    //sendToSerial ("#SET,CTRL_STATUS,1*", outstr) ;
    FILE *fout =  NULL ;

    while(agent->running())
    {
        // Calculate time and publish it
        strcpy (outstr1,"")  ;
        cmjd = currentmjd (0) ;
        agent->cinfo->pdata.node.loc.utc = cmjd ;

        year = mjd2year(cmjd);
        iyear = (int)year;
        day = 365.26 * (year-iyear);
        iday = (int)day;
        //sprintf (fname, "/home/harold/workdir/data/%04d_%03d_copter_telem.txt", iyear, iday) ;
        if (iday != oldday)
        {
            sprintf (fname, "/home/harold/workdir/data/%04d_%03d_copter_telem.txt", iyear, iday) ;
            oldday = iday ;
            if (fout != NULL) fclose (fout) ;
            fout = fopen (fname, "a+") ;
        }
        //sprintf(fname,"%s.%4d%03d%05d.telemetry",satname,iyear,iday+1,iseconds);
        // check to update values
        //
        if (setControllerFlag) {
            sendToSerial2 (contstring, outstr, outstr1) ;
            parseOutstring (outstr1) ;
            setControllerFlag = 0 ;
        }
        if (setMotorFlag) {
            sendToSerial2 (motorstring, outstr, outstr1) ;
            parseOutstring (outstr1) ;
            setMotorFlag = 0 ;
        }
        if (setPIDTypeFlag){
            sendToSerial2 (pidtypestring, outstr, outstr1) ;
            parseOutstring (outstr1) ;
            setPIDTypeFlag = 0 ;
        }

        if (setSpeedFlag) {
            sendToSerial2 (speedstring, outstr, outstr1) ;
            parseOutstring (outstr1) ;
            setSpeedFlag = 0 ;
        }
        if (setPIDFlag) {
            //sendToSerial ("#SET,PID_PITCH,100,200.,300*", outstr) ;
            sendToSerial2 (pidstr, outstr, outstr1) ;
            printf ("%s", outstr1) ;
            if (strlen (outstr1) >2) {
                parseOutstring (outstr1) ;
            }
            setPIDFlag = 0 ;
        }

        if (setThrottleFlag) {
            sendToSerial2 (throttlestring, outstr, outstr1) ;
            printf ("%s", outstr1) ;
            if (strlen (outstr1) >2) {
                parseOutstring (outstr1) ;
            }
            setThrottleFlag = 0 ;
        }


        getAngles () ;
        getMotorSpeed() ;


        agent->cinfo->pdata.node.loc.pos.geod.utc = currentmjd(0);
        pos_geod(&agent->cinfo->pdata.node.loc);

        agent->post(Agent::AGENT_MESSAGE_SOH,json_of_soh(jstring, agent->cinfo->meta, agent->cinfo->pdata));
        fprintf (fout, "%s", jstring.c_str()) ;
        agent->post(Agent::AGENT_MESSAGE_TIME,json_of_time(jstring, agent->cinfo->meta, agent->cinfo->pdata));
        fprintf (fout, "%s", jstring.c_str()) ;

        //json_startout (&jstring) ;
        //json_out (jstring,  "timestamp") ;
        //json_out_1d (jstring,  "imu_att", 0) ;
        //strcpy (outstrbig, jstring) ;
        //iretn = agent->post(Agent::AGENT_MESSAGE_GENERIC,outstrbig) ;
        //fprintf (fout, "%s", outstrbig) ;
        //json_out (jstring,  "motr_cnt") ;
        //for (i=0; i<4; i++)
        //json_out_1d (jstring,  "motr_spd", i) ;
        //strcpy (outstrbig, jstring) ;
        //iretn = agent->post(Agent::AGENT_MESSAGE_GENERIC,outstrbig) ;
        //fprintf (fout, "%s\r\n", outstrbig) ;
        fflush (fout) ;

        /*
    iretn = agent->post(Agent::AGENT_MESSAGE_GENERIC,json_out(jstring, "motr_cnt"));
    for (i=0; i<4; i++)
    iretn = agent->post(Agent::AGENT_MESSAGE_GENERIC,json_out_1d(jstring, "motr_spd",i));
    */
        //fprintf (flog, "%s\r\n", outstrbig) ;

        cmjd = currentmjd();
        if (nmjd > cmjd)
        {
            usec = (unsigned long)((nmjd-cmjd)*86400*1e6+.5);
            COSMOS_USLEEP(usec);
        }
        nmjd += period;
    }


    fclose (fout) ;
    return 0;
}

int32_t request_mjd(char *request, char* output, Agent *agent)
{

    sprintf(output,"%f",agent->cinfo->pdata.node.loc.utc);
    return(0);
}

int32_t request_controller(char *request, char* output, Agent *agent)
{
    int onval ;
    char locstr [30] ;
    setControllerFlag = 1 ;
    sscanf (request, "controller %d", &onval) ;
    sprintf (contstring, "#SET,CTRL_STATUS,%1d*", onval) ;
    sprintf (locstr, "Requesting : %s", contstring) ;
    strcpy (output, locstr) ;
    return(0);
}

int32_t request_speed (char *request, char *output, Agent *agent)
{
    float fval ;
    sscanf (request, "speed %f", &fval) ;
    setSpeedFlag = 1 ;
    sprintf (speedstring, "#SET,SPEED,%f*", fval) ;
    sprintf (output, "Requesting : %s", speedstring) ;
    return(0) ;

}

int32_t request_motor(char *request, char *output, Agent *agent)
{
    // activates motors
    int  motor_number ;
    setMotorFlag = 1 ;
    sscanf (request, "motor %d", &motor_number) ;
    sprintf (motorstring, "#SET,MOTOR,%1d*", motor_number) ;
    sprintf (output, "Requesting : %s", motorstring) ;
    return(0) ;
}

int32_t request_throttle(char *request, char *output, Agent *agent)
{
    float val ;
    sscanf (request, "throttle %f", &val) ;
    sprintf (throttlestring, "#SET,THROTTLE,%f*", val) ;
    sprintf (output, "Requesting throttle of %f", val) ;
    setThrottleFlag = 1 ;
    return(0) ;
}

int32_t request_throttlemin(char *request, char *output, Agent *agent)
{
    float val ;
    sscanf (request, "throttle_min %f", &val) ;
    sprintf (throttlestring, "#SET,THROTTLE_MIN,%f*", val) ;
    sprintf (output, "Requesting throttle min of %f", val) ;
    setThrottleFlag = 1 ;
    return(0) ;
}

int32_t request_throttlemax(char *request, char *output, Agent *agent)
{
    float val ;
    sscanf (request, "throttle_max %f", &val) ;
    sprintf (throttlestring, "#SET,THROTTLE_MAX,%f*", val) ;
    sprintf (output, "Requesting throttle max of %f", val) ;
    setThrottleFlag = 1 ;
    return(0) ;
}


int32_t request_pidpitch(char *request, char *output, Agent *agent)
{
    float fval, fval1, fval2 ;
    sscanf (request, "pid_pitch %f %f %f", &fval, &fval1, &fval2) ;
    setPIDFlag = 1 ;
    sprintf (pidstr, "#SET,PID_PITCH,%f,%f,%f*", fval, fval1, fval2) ;


    printf ("%s\r\n", pidstr) ;
    sprintf (output, "Requesting : %s", pidstr) ;

    return(0) ;
}

int32_t request_printpidpitch (char*request, char *output, Agent *agent)
{
    sprintf (output,"$PID_PITCH,%f,%f,%f*", myCopter.pidPitch[0], myCopter.pidPitch[1],
            myCopter.pidPitch[2]) ;
    return(0) ;
}

int32_t request_printpidroll (char*request, char *output, Agent *agent)
{
    sprintf (output,"$PID_ROLL,%f,%f,%f*", myCopter.pidRoll[0], myCopter.pidRoll[1],
            myCopter.pidRoll[2]) ;
    return(0) ;
}

int32_t request_printpidyaw(char*request, char *output, Agent *agent)
{
    sprintf (output,"$PID_YAW,%f,%f,%f*", myCopter.pidYaw[0], myCopter.pidYaw[1],
            myCopter.pidYaw[2]) ;
    return(0) ;
}

int32_t request_pidroll(char *request, char *output, Agent *agent)
{
    float fval, fval1, fval2 ;
    sscanf (request, "pid_roll %f %f %f", &fval, &fval1, &fval2) ;
    setPIDFlag = 1 ;
    sprintf (output, "Requesting pid roll of %f %f %f", fval, fval1, fval2) ;
    sprintf (pidstr, "#SET,PID_ROLL,%f,%f,%f*", fval, fval1, fval2) ;

    return(0) ;
}

int32_t request_pidyaw (char *request, char *output, Agent *agent)
{
    float fval, fval1, fval2 ;
    sscanf (request, "pid_yaw %f %f %f", &fval, &fval1, &fval2) ;
    setPIDFlag = 1 ;
    sprintf (output, "Requesting pid yaw of %f %f %f", fval, fval1, fval2) ;
    sprintf (pidstr, "#SET,PID_YAW,%f,%f,%f*", fval, fval1, fval2) ;

    return(0) ;
}

int32_t request_typepitch (char *request, char *output, Agent *agent)
{
    int onval ;
    sscanf (request, "typepitch %d", &onval) ;
    setPIDTypeFlag = 1 ;
    sprintf(pidtypestring, "#SET,PID_TYPE,PITCH,%1d*", onval) ;
    sprintf (output,"Requesting : %s", pidtypestring) ;
    return(0) ;
}

int32_t request_typeroll (char *request, char *output, Agent *agent)
{
    int onval ;
    sscanf (request, "typeroll %d", &onval) ;
    setPIDTypeFlag = 1 ;
    sprintf(pidtypestring, "#SET,PID_TYPE,ROLL,%1d*", onval) ;
    sprintf (output,"Requesting : %s", pidtypestring) ;
    return(0) ;
}

int32_t request_typeyaw (char *request, char *output, Agent *agent)
{
    int onval ;
    sscanf (request, "typeyaw %d", &onval) ;
    setPIDTypeFlag = 1 ;
    sprintf(pidtypestring, "#SET,PID_TYPE,YAW,%1d*", onval) ;
    sprintf (output,"Requesting : %s", pidtypestring) ;
    return(0) ;
}

int32_t request_typeall (char *request, char *output, Agent *agent)
{
    int onval ;
    sscanf (request, "typeall %d", &onval) ;
    setPIDTypeFlag = 1 ;
    sprintf(pidtypestring, "#SET,PID_TYPE,ALL,%1d*", onval) ;
    sprintf (output,"Requesting : %s", pidtypestring) ;
    return(0) ;
}



int32_t request_pitch (char *request, char *output, Agent *agent)
{
    //sendToSerial ("#PRINT,PID_PITCH,1*", output) ;
    sprintf (output,"PID_PITCH,%f,%f,%f*",myCopter.pidPitch[0],myCopter.pidPitch[1], myCopter.pidPitch[2]) ;
    return(0) ;
}

int32_t request_roll (char *request, char *output, Agent *agent)
{
    sprintf (output,"PID_ROLL,%f,%f,%f*",myCopter.pidRoll[0],myCopter.pidRoll[1],
            myCopter.pidRoll[2]) ;
    return(0) ;
}

int32_t request_yaw (char *request, char *output, Agent *agent)
{
    sprintf (output,"PID_YAW,%f,%f,%f*",myCopter.pidYaw[0],myCopter.pidYaw[1],
            myCopter.pidYaw[2]) ;
    return(0) ;
}

int32_t request_type (char *request, char *output, Agent *agent)
{
    //sendToSerial ("#PRINT,PID_TYPE,1*", output) ;
    //sprintf (output,"PID_YAW,%f,%f,%f*",myCopter.pid_yaw[0],myCopter.pid_yaw[1],
    //	pid_yaw[2]) ;
    return(0) ;
}

int32_t request_printangles (char *request, char *output, Agent *agent) {
    sprintf (output,"ANGLES,%f,%f,%f*",myCopter.angles[0],myCopter.angles[1],
            myCopter.angles[2]) ;
    return(0) ;
}

int32_t request_printthrottle (char *request, char *output, Agent *agent) {
    sprintf (output,"THROTTLE,%f*", myCopter.throttle) ;
    return(0) ;
}


void getMotorSpeed () {
    char outstr[240], outstr1[240] ;
    char *tmpstr, *result ;
    float s0, s1, s2, s3 ;
    result = NULL ;

    sendToSerial2 ((char *)"#PRINT,SPEED_MOTORS,1*", outstr, outstr1) ;
    printf ("%s", outstr1) ;
    // find the returned string from print angles
    tmpstr = strstr (outstr, (char *)"$SPEED_MOTORS") ;
    if (tmpstr == NULL) return ;
    // get speeds
    result = strtok (tmpstr, ",") ;
    if (result == NULL) return ;
    // get the first angle
    result = strtok (NULL, ",") ;
    s0 = atof (result) ;
    result = strtok (NULL, ",") ;
    s1 = atof (result) ;
    result = strtok (NULL, ",") ;
    s2 = atof (result) ;
    result = strtok (NULL, "*") ;
    s3 = atof (result) ;

    agent->cinfo->pdata.devspec.motr[0]->spd = s0 ;
    agent->cinfo->pdata.devspec.motr[1]->spd = s1 ;
    agent->cinfo->pdata.devspec.motr[2]->spd = s2 ;
    agent->cinfo->pdata.devspec.motr[3]->spd = s3 ;


}



void getAngles () {
    char outstr[240], outstr1[240] ;
    char *tmpstr, *result ;
    float roll, pitch, yaw ;
    avector rpw  ;

    sendToSerial2 ((char *)"#PRINT,ANGLES,1*", outstr, outstr1) ;
    printf ("%s", outstr1) ;
    // find the returned string from print angles
    tmpstr = strstr (outstr, "$ANGLES") ;
    if (tmpstr == NULL) return ;

    // get angles
    result = strtok (tmpstr, ",") ;
    if (result == NULL) return ;
    // get the first angle
    result = strtok (NULL, ",") ;
    roll = atof (result) ;
    result = strtok (NULL, ",") ;
    pitch = atof (result) ;
    result = strtok (NULL, "*") ;
    yaw = atof (result) ;
    rpw.h = roll ;
    rpw.e = pitch ;
    rpw.b = yaw ;
    myCopter.angles[0] = roll ;
    myCopter.angles[1] = pitch ;
    myCopter.angles[2] = yaw ;
    agent->cinfo->pdata.devspec.stt[0]->att = q_euler2quaternion (rpw) ;

}


void sendToSerial (char *istr, char *ostr) {

    int  nbytes = 0 ;
    char inchar [1], inbuf[120] ;

    flushport () ;
    serialport_write (myfd, istr) ;

    while ((nbytes = serialport_read_until (myfd, inbuf, '\n'))<0) {
        COSMOS_USLEEP (200*1000) ;
    }
    strcpy (ostr, inbuf) ;
    nbytes = read (myfd, inchar, 1) ;
    if (nbytes==1)
    {
        inbuf[0] = inchar[0] ;
        while ((nbytes = serialport_read_until (myfd, (char *)&inbuf[1], '\n'))<0) {
            COSMOS_USLEEP (200*1000) ;
        }
        strcat (ostr, inbuf) ;

    }
#if !defined(COSMOS_MAC_OS) && !defined(COSMOS_WIN_OS)
    ioctl (myfd, TCFLSH, TCIFLUSH) ;
    ioctl (myfd, TCFLSH, TCOFLUSH) ;
#endif

    return ;
}

void sendToSerial2 (char *istr, char *ostr0, char *ostr1) {

    int  nbytes = 0 ;
    char inchar [1], inbuf[120] ;

    flushport () ;
    serialport_write (myfd, istr) ;

    strcpy (ostr0, "") ;
    strcpy (ostr1, "") ;
    while ((nbytes = serialport_read_until (myfd, inbuf, '\n'))<0) {
        COSMOS_USLEEP (200*1000) ;
    }
    strcpy (ostr0, inbuf) ;
    nbytes = read (myfd, inchar, 1) ;
    if (nbytes==1)
    {
        inbuf[0] = inchar[0] ;
        while ((nbytes = serialport_read_until (myfd, (char *)&inbuf[1], '\n'))<0) 		{
            COSMOS_USLEEP (200*1000) ;
        }
        strcpy (ostr1, inbuf) ;

    }
#if !defined(COSMOS_MAC_OS) && !defined(COSMOS_WIN_OS)
    ioctl (myfd, TCFLSH, TCIFLUSH) ;
    ioctl (myfd, TCFLSH, TCOFLUSH) ;
#endif
    return ;
}

void parseOutstring (char *outstr) {
    int  i, mval ;
    char *str0 ;
    float fval, fval1, fval2 ;

    if (strstr (outstr, "PID ROLL SET") ) {
        str0 = strtok (outstr, ":") ;
        str0 = strtok (NULL, ",") ;
        fval = atof(str0) ;
        str0 = strtok (NULL, ",") ;
        fval1 = atof(str0) ;
        str0 = strtok (NULL, "*\r\n") ;
        fval2 = atof(str0) ;
        myCopter.pidRoll[0] = fval ;
        myCopter.pidRoll[1] = fval1 ;
        myCopter.pidRoll[2] = fval2 ;
    }
    if (strstr (outstr, "PID PITCH SET") ) {
        str0 = strtok (outstr, ":") ;
        str0 = strtok (NULL, ",") ;
        fval = atof(str0) ;
        str0 = strtok (NULL, ",") ;
        fval1 = atof(str0) ;
        str0 = strtok (NULL, "*\r\n") ;
        fval2 = atof(str0) ;
        myCopter.pidPitch[0] = fval ;
        myCopter.pidPitch[1] = fval1 ;
        myCopter.pidPitch[2] = fval2 ;
    }

    if (strstr (outstr, "PID YAW SET") ) {
        str0 = strtok (outstr, ":") ;
        str0 = strtok (NULL, ",") ;
        fval = atof(str0) ;
        str0 = strtok (NULL, ",") ;
        fval1 = atof(str0) ;
        str0 = strtok (NULL, "*\r\n") ;
        fval2 = atof(str0) ;
        myCopter.pidYaw[0] = fval ;
        myCopter.pidYaw[1] = fval1 ;
        myCopter.pidYaw[2] = fval2 ;
    }
    if (strstr (outstr, "THROTTLE SET") ) {
        str0 = strtok (outstr, ":") ;
        str0 = strtok (NULL, "*\r\n") ;
        fval = atof(str0) ;
        myCopter.throttle = fval ;
    }
    if (strstr (outstr, "MOTOR SET") ) {
        str0 = strtok (outstr, ":") ;
        str0 = strtok (NULL, "*\r\n") ;
        mval = atoi(str0) ;
        if (mval == 0) {
            for (i=0; i<4; i++)
                myCopter.motorFlag [i] = 0 ;
        } else if (mval == 5) {
            for (i=0; i<4; i++)
                myCopter.motorFlag [i] = 1 ;
        } else
            myCopter.motorFlag [mval-1] = 1 ;
    }

    if (strstr (outstr, "SPEED SET") ) {
        // motors get read and filled in the cosmos data struct
        str0 = strtok (outstr, ":") ;
        str0 = strtok (NULL, "*\r\n") ;
        fval = atof(str0) ;
    }

    if (strstr (outstr, "CTRL") ) {
        myCopter.controllerFlag = 0 ;
        if (strstr(outstr, "ON") >0)
            myCopter.controllerFlag = 1 ;
    }

    if (strstr (outstr, "TYPE SET")) {

    }

}




void init_copterstruc () {
    int i ;
    myCopter.controllerFlag = 0 ;

    for (i=0; i<3; i++) {
        myCopter.pidRoll[i] = 0. ;
        myCopter.pidPitch[i] = 0. ;
        myCopter.pidYaw[i] = 0. ;
        myCopter.motorFlag[i]=0 ;
    }
    myCopter.motorFlag[3] = 0 ;
    myCopter.throttle = 0. ;
}

void flushport () {
#if !defined(COSMOS_MAC_OS) && !defined(COSMOS_WIN_OS)
    ioctl (myfd, TCFLSH, TCIFLUSH) ;
    ioctl (myfd, TCFLSH, TCOFLUSH) ;
#endif
}

