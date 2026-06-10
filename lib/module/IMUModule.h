#pragma once

#include <Arduino.h>
#include "Event.h"
#include "module/I2CModule.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

/* OUTPUT FORMAT DEFINITION-------------------------------------------------------------------------------------------
- Use "OUTPUT_READABLE_QUATERNION" for quaternion components in [w, x, y, z] format. Quaternion does not
suffer from gimbal lock problems but is harder to parse or process efficiently on a remote host or software
environment like Processing.

- Use "OUTPUT_READABLE_EULER" for Euler angles (in degrees) output, calculated from the quaternions coming
from the FIFO. EULER ANGLES SUFFER FROM GIMBAL LOCK PROBLEM.

- Use "OUTPUT_READABLE_YAWPITCHROLL" for yaw/pitch/roll angles (in degrees) calculated from the quaternions
coming from the FIFO. THIS REQUIRES GRAVITY VECTOR CALCULATION.
YAW/PITCH/ROLL ANGLES SUFFER FROM GIMBAL LOCK PROBLEM.

- Use "OUTPUT_READABLE_REALACCEL" for acceleration components with gravity removed. The accel reference frame
is not compensated for orientation. +X will always be +X according to the sensor.

- Use "OUTPUT_READABLE_WORLDACCEL" for acceleration components with gravity removed and adjusted for the world
reference frame. Yaw is relative if there is no magnetometer present.

-  Use "OUTPUT_TEAPOT" for output that matches the InvenSense teapot demo.
-------------------------------------------------------------------------------------------------------------------------------*/
// #define OUTPUT_READABLE_YAWPITCHROLL
// #define OUTPUT_READABLE_QUATERNION
// #define OUTPUT_READABLE_EULER
// #define OUTPUT_READABLE_REALACCEL
// #define OUTPUT_READABLE_WORLDACCEL
// #define OUTPUT_TEAPOT

class IMUModule : public I2CModule
{
public:
    IMUModule(std::string name, unsigned long cycleCheckTime) : I2CModule(name, cycleCheckTime)
    {
    }

    // void onEvent(const Event& e) override
    // {
    //     switch (e.type)
    //     {
    //     default:
    //         break;
    //     }
    // }

protected:
    bool setup() override
    {
        // ctx.bus->subscribe(this);
        return imuSetup();
    }

    void cycle() override
    {
        imuCycle();
    }

    virtual void onYawChanged(short yaw)
    {
        ctx.bus->publish({Event::YawChanged, this, yaw});
    }

    virtual void onPitchChanged(short pitch)
    {
        ctx.bus->publish({Event::PitchChanged, this, pitch});
    }

    virtual void onRollChanged(short roll)
    {
        ctx.bus->publish({Event::RollChanged, this, roll});
    }

private:
#ifdef IMU_INTERRUPT_PIN
    int const INTERRUPT_PIN = IMU_INTERRUPT_PIN;  // Define the interruption #0 pin
#endif
    /* MPU6050 default I2C address is 0x68*/
    MPU6050 imu;
    //MPU6050 imu(0x69); //Use for AD0 high
    //MPU6050 imu(0x68, &Wire1); //Use for AD0 low, but 2nd Wire (TWI/I2C) object.
    short previousYaw = 11111;      // these should never be larger than 360,
    short previousPitch = 11111;    //  so use a big number to ensure an event
    short previousRoll = 11111;     //  is invoked due to value change

    bool blinkState;

    /*---MPU6050 Control/Status Variables---*/
    unsigned short MPUIntStatus;   // Holds actual interrupt status byte from MPU
    unsigned short devStatus;      // Return status after each device operation (0 = success, !0 = error)
    unsigned short packetSize;    // Expected DMP packet size (default is 42 bytes)
    uint8_t FIFOBuffer[64]; // FIFO storage buffer

    /*---Orientation/Motion Variables---*/
    Quaternion q;           // [w, x, y, z]         Quaternion container
    VectorInt16 aa;         // [x, y, z]            Accel sensor measurements
    VectorInt16 gy;         // [x, y, z]            Gyro sensor measurements
    VectorInt16 aaReal;     // [x, y, z]            Gravity-free accel sensor measurements
    VectorInt16 aaWorld;    // [x, y, z]            World-frame accel sensor measurements
    VectorFloat gravity;    // [x, y, z]            Gravity vector
    float euler[3];         // [psi, theta, phi]    Euler angle container
    float ypr[3];           // [yaw, pitch, roll]   Yaw/Pitch/Roll container and gravity vector

#ifdef OUTPUT_TEAPOT
    /*-Packet structure for InvenSense teapot demo-*/
    uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };
#endif
#ifdef IMU_INTERRUPT_PIN
    /*------Interrupt detection routine------*/
    volatile bool MPUInterrupt = false;     // Indicates whether MPU6050 interrupt pin has gone high

    void DMPDataReady() {
        MPUInterrupt = true;
    }
#endif

    bool imuSetup()
    {
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        // Wire.begin(I2C_SDA, I2C_SCL);
        // Wire.setClock(400000); // 400kHz I2C clock. Comment on this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
#endif

        /*Initialize device*/
        // say(F("Initializing I2C devices..."));
        imu.initialize();
#ifdef IMU_INTERRUPT_PIN
        pinMode(IMU_INTERRUPT_PIN, INPUT);
#endif

        /*Verify connection*/
        // say(F("Testing MPU6050 connection..."));
        // if(imu.testConnection() == false){
        //   say("MPU6050 connection failed");
        //   // return false;
        // }
        // else {
        //   say("MPU6050 connection successful");
        // }

        /*Wait for Serial input*/
        /* Initializate and configure the DMP*/
        // say(F("Initializing DMP..."));
        devStatus = imu.dmpInitialize();

        /* Supply your imu offsets here, scaled for min sensitivity */
        imu.setXGyroOffset(0);
        imu.setYGyroOffset(0);
        imu.setZGyroOffset(0);
        imu.setXAccelOffset(0);
        imu.setYAccelOffset(0);
        imu.setZAccelOffset(0);

        /* Making sure it worked (returns 0 if so) */
        if (devStatus == 0)
        {
            imu.CalibrateAccel(6); // Calibration Time: generate offsets and calibrate our MPU6050
            imu.CalibrateGyro(6);
            say("These are the Active offsets: ");
            imu.PrintActiveOffsets();
            // say(F("Enabling DMP..."));   //Turning ON DMP
            imu.setDMPEnabled(true);

            /*Enable Arduino interrupt detection*/
            // say(F("Enabling interrupt detection (Arduino external interrupt "));
            // say(digitalPinToInterrupt(INTERRUPT_PIN));
            // say(F(")..."));
            // attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), DMPDataReady, RISING);
            // MPUIntStatus = imu.getIntStatus();

            /* Set the DMP Ready flag so the main loop() function knows it is okay to use it */
            // say(F("DMP ready! Waiting for first interrupt..."));
            packetSize = imu.dmpGetFIFOPacketSize(); //Get expected DMP packet size for later comparison
        }
        else
        {
            Serial.print(F("DMP Initialization failed (code ")); //Print the error code
            Serial.print(devStatus);
            Serial.print(F(")"));
            // 1 = initial memory load failed
            // 2 = DMP configuration updates failed
            return false;
        }
        return true;
    }

    void imuCycle()
    {
        /* Read a packet from FIFO */
        if (imu.dmpGetCurrentFIFOPacket(FIFOBuffer))
        {
            // Get the Latest packet
            imu.dmpGetQuaternion(&q, FIFOBuffer);
            imu.dmpGetGravity(&gravity, &q);
            imu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            imu.dmpGetEuler(euler, &q);
            imu.dmpGetAccel(&aa, FIFOBuffer);
            imu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            imu.dmpConvertToWorldFrame(&aaWorld, &aaReal, &q);
            short dat;
            dat = round(ypr[0] * 180.0 / M_PI * 10.0);
            if (dat != previousYaw)
            {
                onYawChanged(dat);
                previousYaw = dat;
            }
            dat = round(ypr[1] * 180.0 / M_PI * 10.0);
            if (dat != previousPitch)
            {
                onPitchChanged(dat);
                previousPitch = dat;
            }
            dat = round(ypr[2] * 180.0 / M_PI * 10.0);
            if (dat != previousRoll)
            {
                onRollChanged(dat);
                previousRoll = dat;
            }

#ifdef OUTPUT_READABLE_YAWPITCHROLL
            /* Display Euler angles in degrees */
            imu.dmpGetQuaternion(&q, FIFOBuffer);
            imu.dmpGetGravity(&gravity, &q);
            imu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            Serial.print("ypr\t");
            Serial.print(ypr[0] * 180 / M_PI);
            Serial.print("\t");
            Serial.print(ypr[1] * 180 / M_PI);
            Serial.print("\t");
            Serial.println(ypr[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_QUATERNION
            /* Display Quaternion values in easy matrix form: [w, x, y, z] */
            imu.dmpGetQuaternion(&q, FIFOBuffer);
            say("quat\t");
            say(q.w);
            say("\t");
            say(q.x);
            say("\t");
            say(q.y);
            say("\t");
            say(q.z);
#endif

#ifdef OUTPUT_READABLE_EULER
            /* Display Euler angles in degrees */
            imu.dmpGetQuaternion(&q, FIFOBuffer);
            imu.dmpGetEuler(euler, &q);
            say("euler\t");
            say(euler[0] * 180 / M_PI);
            say("\t");
            say(euler[1] * 180 / M_PI);
            say("\t");
            say(euler[2] * 180 / M_PI);
#endif

#ifdef OUTPUT_READABLE_REALACCEL
            /* Display real acceleration, adjusted to remove gravity */
            imu.dmpGetQuaternion(&q, FIFOBuffer);
            imu.dmpGetAccel(&aa, FIFOBuffer);
            imu.dmpGetGravity(&gravity, &q);
            imu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            say("areal\t");
            say(aaReal.x);
            say("\t");
            say(aaReal.y);
            say("\t");
            say(aaReal.z);
#endif

#ifdef OUTPUT_READABLE_WORLDACCEL
            /* Display initial world-frame acceleration, adjusted to remove gravity and rotated based on known orientation from Quaternion */
            imu.dmpGetQuaternion(&q, FIFOBuffer);
            imu.dmpGetAccel(&aa, FIFOBuffer);
            imu.dmpGetGravity(&gravity, &q);
            imu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            imu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            say("aworld\t");
            say(aaWorld.x);
            say("\t");
            say(aaWorld.y);
            say("\t");
            say(aaWorld.z);
#endif

#ifdef OUTPUT_TEAPOT
            /* Display quaternion values in InvenSense Teapot demo format */
            teapotPacket[2] = FIFOBuffer[0];
            teapotPacket[3] = FIFOBuffer[1];
            teapotPacket[4] = FIFOBuffer[4];
            teapotPacket[5] = FIFOBuffer[5];
            teapotPacket[6] = FIFOBuffer[8];
            teapotPacket[7] = FIFOBuffer[9];
            teapotPacket[8] = FIFOBuffer[12];
            teapotPacket[9] = FIFOBuffer[13];
            Serial.write(teapotPacket, 14);
            teapotPacket[11]++; // PacketCount, loops at 0xFF on purpose
#endif
        }
    }
};
