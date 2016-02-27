/*
 * demo_optimized.cpp
 *
 *  Created on: 2016年1月10日
 *      Author: qq95538
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "MPU6050_4Edison.hpp"
#include "mraa.h"

#include <sys/types.h>
#include <sys/stat.h>

MPU6050 mpu_head;
//MPU6050 mpu_body;

int PIN_PWM_A = 6;
int PIN_PWM_B = 5;
int PIN_6612_EA = 10;
int PIN_6612_EB = 11;
int PIN_6612_STBY = 4;

mraa::Pwm* PWM_A = new mraa::Pwm(PIN_PWM_A);
mraa::Pwm* PWM_B = new mraa::Pwm(PIN_PWM_B);
mraa::Gpio* EA_6612 = new mraa::Gpio(PIN_6612_EA);
mraa::Gpio* EB_6612 = new mraa::Gpio(PIN_6612_EB);
mraa::Gpio* STBY_6612 = new mraa::Gpio(PIN_6612_STBY);

const char *NAME_FIFO = "/tmp/kr_fifo";
int FD_FIFO = -1;
int FIFO_OPEN_MODE = O_RDONLY;
int FIFO_WRITE_MODE = 0x777 | S_IFIFO;
#define BUF_FIFO_SIZE 32
unsigned char BUF_FIFO[BUF_FIFO_SIZE];

enum TYPE_PIPE_CMD{
    PIPE_CMD_STOP = 0,
    PIPE_CMD_MOVE_FORWARD,
    PIPE_CMD_MOVE_BACKWARD,
    PIPE_CMD_TURN_LEFT,
    PIPE_CMD_TURN_RIGHT
};

struct MOTOR_DIRECTION {
    unsigned char a;
    unsigned char b;
    unsigned char stby;
};

struct TYPE_CMD_ARG {
    enum TYPE_PIPE_CMD cmd;
    union {
       float speed;
       float angle;
       float arg1_f;
    };
    union {
       float distance;
       float arg2_f;
    };
    unsigned int enable;
};

struct MOTOR_DIRECTION MOTOR_DIR[] = {
    [PIPE_CMD_STOP]		= { 0, 0, 0},
    [PIPE_CMD_MOVE_FORWARD]	= { 0, 1, 1},
    [PIPE_CMD_MOVE_BACKWARD]	= { 1, 0, 1},
    [PIPE_CMD_TURN_LEFT]	= { 0, 0, 1},
    [PIPE_CMD_TURN_RIGHT]	= { 1, 1, 1},
};

#define get_motor_direction(pipe_cmd, ea, eb, stby)               \
    do {                                                          \
         ea = MOTOR_DIR[pipe_cmd].a;                              \
         eb = MOTOR_DIR[pipe_cmd].b;                              \
         stby = MOTOR_DIR[pipe_cmd].stby;                         \
                                                                  \
    } while(0)

#define OUTPUT_READABLE_QUATERNION
#define OUTPUT_READABLE_EULER
#define OUTPUT_READABLE_YAWPITCHROLL
#define OUTPUT_READABLE_REALACCEL
#define OUTPUT_READABLE_WORLDACCEL
//#define OUTPUT_TEAPOT
struct TYPE_CMD_ARG
get_cmd_args(void) 
{
    struct TYPE_CMD_ARG tmp_cmd_arg;
    sscanf((const char *)BUF_FIFO, "%d %f %f %d",
        &tmp_cmd_arg.cmd, &tmp_cmd_arg.arg1_f, &tmp_cmd_arg.arg2_f, &tmp_cmd_arg.enable);

//    printf("BUF_FIFO, cmd: %d arg1: %f arg2: %f enable: %d", 
//        tmp_cmd_arg.cmd, tmp_cmd_arg.arg1_f, tmp_cmd_arg.arg2_f, tmp_cmd_arg.enable);

    return tmp_cmd_arg;

}

void drive_car(struct TYPE_CMD_ARG drive_cmd_arg, float pwma, float pwmb)
{
    STBY_6612->write(MOTOR_DIR[drive_cmd_arg.cmd].stby);
    EA_6612->write(MOTOR_DIR[drive_cmd_arg.cmd].a);
    EB_6612->write(MOTOR_DIR[drive_cmd_arg.cmd].b);

    PWM_A->write(pwma);  //Right
    PWM_B->write(pwmb);  //Left

}

// MPU control/status vars
bool dmpHeadReady = false;  // set true if DMP init was successful
//bool dmpBodyReady = false;  // set true if DMP init was successful

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void setup() {
    dmpHeadReady = mpu_head.dmpStartDevice(0x68, 0, 0, 0);
    EA_6612->dir(mraa::DIR_OUT);
    EB_6612->dir(mraa::DIR_OUT);
    STBY_6612->dir(mraa::DIR_OUT);
    
    PWM_A->enable(true);
    PWM_B->enable(true);

    //dmpBodyReady = mpu_body.dmpStartDevice(0x69, 0, 0, 0);
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
void loop() {
    if(mpu_head.dmpGetData())
    {
        //printf("quat %7.2f %7.2f %7.2f %7.2f    ", dmpQuat.w,dmpQuat.x,dmpQuat.y,dmpQuat.z);
        //printf("accell %6d %6d %6d    ", dmpAccel.x, dmpAccel.y, dmpAccel.z);
        //printf("areal %6d %6d %6d    ", dmpAccelReal.x, dmpAccelReal.y, dmpAccelReal.z);
        //printf("aworld %6d %6d %6d    ", dmpAccelWorld.x, dmpAccelWorld.y, dmpAccelWorld.z);
        //printf("gravity %7.2f %7.2f %7.2f    ", dmpGravity.x, dmpGravity.y, dmpGravity.z);
        //printf("euler %7.2f %7.2f %7.2f    ", dmpEuler[0], dmpEuler[1], dmpEuler[2]);
        //printf("ypr  %7.2f %7.2f %7.2f    ", dmpYawPitchRoll[0], dmpYawPitchRoll[1], dmpYawPitchRoll[2]);
        //printf("\n");
    }

   //if(mpu_body.dmpGetData())
    //{
        /*
        //printf("quat %7.2f %7.2f %7.2f %7.2f    ", dmpQuat.w,dmpQuat.x,dmpQuat.y,dmpQuat.z);
        //printf("accell %6d %6d %6d    ", dmpAccel.x, dmpAccel.y, dmpAccel.z);
        //printf("areal %6d %6d %6d    ", dmpAccelReal.x, dmpAccelReal.y, dmpAccelReal.z);
        printf("aworld %6d %6d %6d    ", dmpAccelWorld.x, dmpAccelWorld.y, dmpAccelWorld.z);
        //printf("gravity %7.2f %7.2f %7.2f    ", dmpGravity.x, dmpGravity.y, dmpGravity.z);
        printf("euler %7.2f %7.2f %7.2f    ", dmpEuler[0], dmpEuler[1], dmpEuler[2]);
        printf("ypr  %7.2f %7.2f %7.2f    ", dmpYawPitchRoll[0], dmpYawPitchRoll[1], dmpYawPitchRoll[2]);
        printf("\n");
          */
    //}
}
int initial_pipe(){
    remove( NAME_FIFO);
    mknod( NAME_FIFO, FIFO_WRITE_MODE, (dev_t)0);

    return 0;
}

int block_till_pipe_connected(){

    printf("Waiting for a pipe connnection %s\n", NAME_FIFO);
    FD_FIFO = open(NAME_FIFO, FIFO_OPEN_MODE);
    return 0;
}

int flush_motors(int ea, int eb, int stby, float pwma, float pwmb){
    STBY_6612->write(stby);
    EA_6612->write(ea);
    EB_6612->write(eb);

    PWM_A->write(pwma);  //Right
    PWM_B->write(pwmb);  //Left
 
}
int main() {
    setup();
    usleep(100000);
    float ypr, pwma = 0.5, pwmb = 0.5;
    float start_angle, stop_angle, tmp_angle, a, b;

    unsigned char ea, eb, stby;
    enum TYPE_PIPE_CMD pipe_cmd = PIPE_CMD_MOVE_FORWARD;
    struct TYPE_CMD_ARG cmd_arg;
    
    int ret = 0;

    STBY_6612->write(0);

    initial_pipe();
    //The main program can be blocked here till the pipe connection is made!
    block_till_pipe_connected();

////////////////////////////////////////////////////////
// Get the start angle from 6050
// The theory is that when the closed 2 data are very near
// we take it as the initial 6050 angle
//
/////////////////////////////////////////////////////////
    start_angle = mpu_head.dmpGetFirstYPRData();
    while (true)
    {
        while(true)
	  if ((a = mpu_head.dmpGetFirstYPRData()) != -255) break; 

	while(true)
	  if ((b = mpu_head.dmpGetFirstYPRData()) != -255) break; 

	if( fabs(a - b) < 0.05) break;
    }

    start_angle = a;
    printf("start_angle %f\n", start_angle);

    // The initial speed
    // FIXME: PLEASE do not use imediately number!!
    
    bool run_cmd = false;
    bool cmd_self_flag = false;

    while(1){

	//STEP-1: get command from fifo pipe.
        ret = read(FD_FIFO, BUF_FIFO, BUF_FIFO_SIZE);

	//STEP-2: paser command.
//        pipe_cmd = TYPE_PIPE_CMD(atoi((const char*) &BUF_FIFO[0]));
        cmd_arg = get_cmd_args();
	//STEP-3: translate the pipe command
        get_motor_direction(pipe_cmd, ea, eb, stby);


	//STEP-4: logic for tuning the two PWM dynamically!
	//TODO: Implement the other 3 directions and stop function!
        ypr = mpu_head.dmpGetFirstYPRData();

	if (cmd_arg.enable == 1) {
            run_cmd = true; 
	    continue;
        }
        if (run_cmd) {
            if (cmd_arg.cmd == PIPE_CMD_MOVE_FORWARD)
            {
        	    if( pwmb != cmd_arg.speed)
        	    {
        		pwmb = cmd_arg.speed;
                        pwma = cmd_arg.speed;
        	    }
        	    if (int(ypr) != -255 ){
        	    	if(ypr - start_angle > 0.2)
        	    		pwma += 0.05;
        	    	else if (ypr - start_angle < -0.2)
        	    		pwma -= 0.05;
        	    }
            }
            else if (cmd_arg.cmd == PIPE_CMD_MOVE_BACKWARD)
            {
                if( pwmb != cmd_arg.speed)
                {
                    pwmb = cmd_arg.speed;
                    pwma = cmd_arg.speed;
                }
                if (int(ypr) != -255 ){
                    if(ypr - start_angle > 0.2)
                            pwma += 0.05;
                    else if (ypr - start_angle < -0.2)
                            pwma -= 0.05;
                }
            }
            else if (cmd_arg.cmd == PIPE_CMD_TURN_LEFT)
            {
                drive_car(cmd_arg, pwma, pwmb);

                printf("Turn Left: from %f -%f\n", start_angle, cmd_arg.angle);
    	        while(1){
                    ypr = mpu_head.dmpGetFirstYPRData();
                    if (int(ypr) != -255 ){

                        if ( start_angle - cmd_arg.angle < -180)
                            stop_angle = start_angle - cmd_arg.angle +360;
                        else
                            stop_angle = start_angle - cmd_arg.angle;

                        if ( fabs(ypr - stop_angle) <= 1) {
                            cmd_arg.cmd = PIPE_CMD_STOP;
                            start_angle = ypr;
                            printf("Complete turn Left: current angle: %f\n", start_angle);
                            break;
                        }
                    }
                }
            }
            else if (cmd_arg.cmd == PIPE_CMD_TURN_RIGHT)
            {

                drive_car(cmd_arg, pwma, pwmb);

                printf("Turn Right: from %f -%f\n", start_angle, cmd_arg.angle);
    	        while(1){
                    ypr = mpu_head.dmpGetFirstYPRData();
                    if (int(ypr) != -255 ){

                        if ( start_angle + cmd_arg.angle > 180)
                            stop_angle = start_angle + cmd_arg.angle - 360;
                        else
                            stop_angle = start_angle + cmd_arg.angle;

                        if ( fabs(ypr - stop_angle) <= 1) {
                            cmd_arg.cmd = PIPE_CMD_STOP;
                            start_angle = ypr;
                            printf("Complete turn Right: current angle: %f\n", start_angle);
                            break;
                        }
                    }
                }
            }

	    //FINAL: drive motors as we wanted
            drive_car(cmd_arg, pwma, pwmb);
            run_cmd = false; 
        }
/*
	if (cmd_arg.enable == 0) {
            run_cmd = false; 
        }
*/
//    	printf("cmd_arg.enable%d\n", cmd_arg.enable);
	memset(BUF_FIFO, 0, BUF_FIFO_SIZE);
	//FINAL: drive motors as we wanted
//  	printf("cmd_arg.cmd %d\n", cmd_arg.cmd);
        //flush_motors(MOTOR_DIR[cmd_arg.cmd].a, MOTOR_DIR[cmd_arg.cmd].b, MOTOR_DIR[cmd_arg.cmd].stby, pwma, pwmb);
	sleep(0.01);
    }
    return 0;
}
