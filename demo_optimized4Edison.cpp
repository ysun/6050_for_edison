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


//#define OUTPUT_READABLE_QUATERNION
#define OUTPUT_READABLE_EULER
#define OUTPUT_READABLE_YAWPITCHROLL
#define OUTPUT_READABLE_REALACCEL
#define OUTPUT_READABLE_WORLDACCEL
//#define OUTPUT_TEAPOT

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

   // if(mpu_body.dmpGetData())
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

int main() {
    setup();
    usleep(100000);
    float ypr, pwma = 0.5, pwmb = 0.5;
    STBY_6612->write(0);
    EA_6612->write(0);
    EB_6612->write(1);


    float start_angle;
    start_angle = mpu_head.dmpGetFirstYPRData();
    float tmp_angle, a, b;
    for (;;)
    {
        //tmp_angle = mpu_head.dmpGetFirstYPRData();
        //if ((int(start_angle) == -255) || ((start_angle - tmp_angle) > 5) || ((start_angle - tmp_angle) < -5))
/*
        start_angle = mpu_head.dmpGetFirstYPRData();
        while(1)
        {
	    tmp_angle = mpu_head.dmpGetFirstYPRData();
            if (int(tmp_angle) == -255)
            {
                continue;
            }
            else
                break;
        }
        if (int(start_angle) == -255)
        {
            start_angle = mpu_head.dmpGetFirstYPRData();
            continue;
        }
        else if (((start_angle - tmp_angle) < 5) && ((start_angle - tmp_angle) > -5))
        {
            printf("start_angle %f - tmp_angle %f", start_angle, tmp_angle);
            usleep(100000);
            break;
        }
        else
            continue;
*/
        for(;;)
	  if ((a = mpu_head.dmpGetFirstYPRData()) != -255) break; 

	for(;;)
	  if ((b = mpu_head.dmpGetFirstYPRData()) != -255) break; 

	if( fabs(a - b) < 0.05) break;
    }

    start_angle = a;

    PWM_A->write(a);  //Right
    PWM_B->write(b);  //Left

    printf("start_angle %f\n", start_angle);
    STBY_6612->write(1);

    for (;;)
    {
        ypr = mpu_head.dmpGetFirstYPRData();
	if (int(ypr) != -255 ){
		printf("ypr %f\n", ypr);

		if(ypr - start_angle > 0.2)
			pwma += 0.05;
		else if (ypr - start_angle < -0.2)
			pwma -= 0.05;

	       PWM_A->write(a);  //Right
	       PWM_B->write(b);  //Left
		sleep(10);
	}
    }
    return 0;
}




