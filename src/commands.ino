#ifdef ENABLE_SERIAL_COMMANDS
/*
   stewy
   Copyright (C) 2018  Philippe Desrosiers

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "commands.h"
#include "config.h"

void processCommands()
{
    // This should always be called to process user input
    shell_task();
}

/**
   Function to read data from serial port
   Functions to read from physical media should use this prototype:
   int my_reader_function(char * data)
 */
int shell_reader(char * data)
{
    // Wrapper for Serial.read() method
    if (Serial.available())
    {
        *data = Serial.read();
        return 1;
    }
    return 0;
}

/**
   Function to write data to serial port
   Functions to write to physical media should use this prototype
   void my_writer_function(char data)
 */
void shell_writer(char data)
{
    Serial.write(data);
}

int handleDemo(int argc, char** argv)
{
    const int d = 500;

    const int demo_values[][6]= {
        //sway
        {MAX_SWAY, 0, 0, 0, 0, 0},
        {MIN_SWAY, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},

        //surge
        {0, MAX_SURGE, 0, 0, 0, 0},
        {0, MIN_SURGE, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},

        //heave
        {0, 0, MAX_HEAVE, 0, 0, 0},
        {0, 0, MIN_HEAVE, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},

        //pitch
        {0, 0, 0, MAX_PITCH, 0, 0},
        {0, 0, 0, MIN_PITCH, 0, 0},
        {0, 0, 0, 0, 0, 0},

        //roll
        {0, 0, 0, 0, MAX_ROLL, 0},
        {0, 0, 0, 0, MIN_ROLL, 0},
        {0, 0, 0, 0, 0, 0},

        //yaw
        {0, 0, 0, 0, 0, MAX_YAW},
        {0, 0, 0, 0, 0, MIN_YAW},
        {0, 0, 0, 0, 0, 0}
    };

    int ccount=(int)sizeof(demo_values) / sizeof(demo_values[0]);

    for (int i=0; i<ccount; i++)
    {
        stu.moveTo(sp_servo,
                   demo_values[i][0],
                   demo_values[i][1],
                   demo_values[i][2],
                   demo_values[i][3],
                   demo_values[i][4],
                   demo_values[i][5]);
        updateServos();
        delay(d);
    }

    return SHELL_RET_SUCCESS;
}


/**
   set a single servo (#1-6) to the specified value in degrees.
 */
int handleSet(int argc, char** argv)
{
    if (argc == 1)
    {
        Serial.print("Usage: set <servo> min|mid|max|<angle>");
        return SHELL_RET_FAILURE;
    }

    int srv;
    srv = atoi(argv[1]);
    if (srv > 0 && srv < 7)
    {
        int val;
        if (!strcmp(argv[2], "mid"))
        {
            val = SERVO_MID_ANGLE;
        }
        else if (!strcmp(argv[2], "min"))
        {
            val = SERVO_MIN_ANGLE;
        }
        else if (!strcmp(argv[2], "max"))
        {
            val = SERVO_MAX_ANGLE;
        }
        else
        {
            val = atoi(argv[2]);
        }


        if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE)
        {
            setServo(srv - 1, val);
        }
        else
        {
            Serial.print("Invalid servo value for Servo #%d. Valid values are (min, mid, max), or a number between %d and %d.", srv, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
            Serial.print("Usage: set <servo> min|mid|max|<angle>");
        }
    }
    else
    {
        Serial.print("Invalid Servo number. Servo number must be between 1 and 6.");
        Serial.print("Usage: set <servo> min|mid|max|<angle>");
    }
    // updateServos();

    return SHELL_RET_SUCCESS;
}

/**
   set a single servo (#1-6) to the specified value in microsecond pulse width.
 */
int handleMSet(int argc, char** argv)
{
    if (argc == 1)
    {
        Serial.print("Usage: set <servo> min|mid|max|<microseconds>");
        return SHELL_RET_FAILURE;
    }

    int srv;
    srv = atoi(argv[1]);
    if (srv > 0 && srv < 7)
    {
        int val;
        if (!strcmp(argv[2], "mid"))
        {
            val = SERVO_MID_US;
        }
        else if (!strcmp(argv[2], "min"))
        {
            val = SERVO_MIN_US;
        }
        else if (!strcmp(argv[2], "max"))
        {
            val = SERVO_MAX_US;
        }
        else
        {
            val = atoi(argv[2]);
        }


        if (val >= SERVO_MIN_US && val <= SERVO_MAX_US)
        {
            setServoMicros(srv-1,val);
#ifdef ENABLE_SERVOS
            if (SERVO_REVERSE[srv-1])
            {
                val = SERVO_MIN_US + (SERVO_MAX_US - val);
            }
            servos[srv-1].writeMicroseconds((int)constrain(val + SERVO_TRIM[srv-1],SERVO_MIN_US, SERVO_MAX_US));
#endif
        }
        else
        {
            Serial.print("Invalid servo value for Servo #%d. Valid values are (min, mid, max), or a number between %d and %d.", srv, SERVO_MIN_US, SERVO_MAX_US);
            Serial.print("Usage: set <servo> min|mid|max|<angle>");
        }
    }
    else
    {
        Serial.print("Invalid Servo number. Servo number must be between 1 and 6.");
        Serial.print("Usage: set <servo> min|mid|max|<angle>");
    }

    return SHELL_RET_SUCCESS;
}

/**
   Print some help.
 */
int handleHelp(int argc, char** argv)
{
    int size=*(&commands + 1) - commands;
    for (int i=0; i<size; i++)
    {
        Serial.print("%s\t\t%s",commands[i].shell_command_string, commands[i].shell_help_string);
    }
    return SHELL_RET_SUCCESS;
}

int handleSetAll(int argc, char** argv)
{
    if (argc == 1)
    {
        Serial.print("Usage: setall <servo> min|mid|max|<angle>");
        return SHELL_RET_FAILURE;
    }

    char* token = argv[1];
    int val = 0;

    if (token != NULL)
    {
        bool special = false;

        if (strncmp(token, "mid", 3) != 0 &&
            strncmp(token, "min", 3) != 0 &&
            strncmp(token, "max", 3) != 0)
        {
            val = atoi(token);
        }
        else
        {
            special = true;
        }

        for (int i = 0; i < 6; i++)
        {

            if (special)
            {
                if (strncmp(token, "mid", 3) == 0)
                {
                    val = SERVO_MID_ANGLE;
                }
                else if (strncmp(token, "min", 3) == 0)
                {
                    val = SERVO_MIN_ANGLE;
                }
                else if (strncmp(token, "max", 3) == 0)
                {
                    val = SERVO_MAX_ANGLE;
                }
            }

            if (val >= SERVO_MIN_ANGLE && val <= SERVO_MAX_ANGLE)
            {
                setServo(i, val);
            }
            else
            {
                Serial.print("Invalid servo value for Servo #%d. Valid values are min|mid|max, or a number between %d and %d.", i + 1, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
                Serial.print("Usage: setall min|mid|max|<angle>");
            }
        }
    }
    else
    {
        Serial.print("Usage: setall min|mid|max|<angle>");
    }

    // updateServos();
    return SHELL_RET_SUCCESS;
}


int handleMSetAll(int argc, char** argv)
{
    if (argc == 1)
    {
        Serial.print("Usage: msetall min|mid|max|<microseconds>");
        return SHELL_RET_FAILURE;
    }

    char* token = argv[1];
    int val = 0;

    if (token != NULL)
    {
        bool special = false;

        if (strncmp(token, "mid", 3) != 0 &&
            strncmp(token, "min", 3) != 0 &&
            strncmp(token, "max", 3) != 0)
        {
            val = atoi(token);
        }
        else
        {
            special = true;
        }

        for (int i = 0; i < 6; i++)
        {

            if (special)
            {
                if (strncmp(token, "mid", 3) == 0)
                {
                    val = SERVO_MID_US;
                }
                else if (strncmp(token, "min", 3) == 0)
                {
                    val = SERVO_MIN_US;
                }
                else if (strncmp(token, "max", 3) == 0)
                {
                    val = SERVO_MAX_US;
                }
            }

            if (val >= SERVO_MIN_US && val <= SERVO_MAX_US)
            {
                setServoMicros(i,val);
#ifdef ENABLE_SERVOS
                if (SERVO_REVERSE[i])
                {
                    val = SERVO_MIN_US + (SERVO_MAX_US - val);
                }
                servos[i].writeMicroseconds((int)constrain(val + SERVO_TRIM[i], SERVO_MIN_US, SERVO_MAX_US));
#endif
            }
            else
            {
                Serial.print("Invalid servo value for Servo #%d. Valid values are min|mid|max, or a number between %d and %d.", i + 1, SERVO_MIN_US, SERVO_MAX_US);
                Serial.print("Usage: msetall min|mid|max|<microseconds>");
            }
        }
    }
    else
    {
        Serial.print("Usage: msetall min|mid|max|<microseconds>");
    }

    // updateServos();
    return SHELL_RET_SUCCESS;
}

int handleDump(int argc, char** argv)
{

    Serial.print("===== Config =====");
    Serial.print("px = %.3f", PX);
    Serial.print("ix = %.3f", IX);
    Serial.print("dx = %.3f", DX);
    Serial.print("py = %.3f", PY);
    Serial.print("iy = %.3f", IY);
    Serial.print("dy = %.3f", DY);

    Serial.print("===== Platform =====");
    Serial.print("platform.sway = %d", stu.getSway());
    Serial.print("platform.surge = %d", stu.getSurge());
    Serial.print("platform.heave = %d", stu.getHeave());
    Serial.print("platform.roll = %.2f", stu.getRoll());
    Serial.print("platform.pitch = %.2f", stu.getPitch());
    Serial.print("platform.yaw = %.2f", stu.getYaw());

    Serial.print("\n===== Servos =====");
    for (int i = 0; i < 6; i++)
    {
#ifdef ENABLE_SERVOS
        Serial.print("s%d (physical, setpoint, us) = (%d, %.2f, %.2f)", i, servos[i].read(), sp_servo[i], _toUs(servos[i].read()));
#else
        Serial.print("s%d (physical, setpoint, us) = (N/A, %.2f, N/A)", i, sp_servo[i]);
#endif
    }

#ifdef ENABLE_NUNCHUCK
    Serial.print("\n===== Nunchuck =====");
    Serial.print("nunchuck.ok = %d",nc.isOk());
    if (nc.isOk())
    {
        Serial.print("nunchuck.buttons.c = %s",nc.getButtonC() ? "true" : "false");
        Serial.print("nunchuck.buttons.z = %s",nc.getButtonC() ? "true" : "false");
        Serial.print("nunchuck.joystick.x = %d",nc.getJoyX());
        Serial.print("nunchuck.joystick.y = %d",nc.getJoyY());
        Serial.print("nunchuck.tilt.x = %.2f",nc.getTiltX());
        Serial.print("nunchuck.tilt.y = %.2f",nc.getTiltY());
        Serial.print("nunchuck.tilt.z = %.2f",nc.getTiltZ());
        Serial.print("nunchuck.accel.x = %d",nc.getAccelX());
        Serial.print("nunchuck.accel.y = %d",nc.getAccelY());
        Serial.print("nunchuck.accel.z = %d",nc.getAccelZ());
    }
  #endif

#ifdef ENABLE_TOUCHSCREEN
    Serial.print("\n===== Touch screen =====");

    TSPoint p = ts.getPoint();

    Serial.print("touchscreen.x = %d",p.x);
    Serial.print("touchscreen.y = %d",p.y);
    Serial.print("touchscreen.z = %d",p.z);
  #endif

    return SHELL_RET_SUCCESS;
}

int handleReset(int argc, char** argv)
{
    CPU_RESTART;
    return SHELL_RET_SUCCESS;     //unreachable?
}

int handleMoveTo(int argc, char** argv)
{
    if (argc == 1)
    {
        Serial.print("Usage: moveto home | <pitch angle> <roll angle>");
        return SHELL_RET_FAILURE;
    }

    double pitch, roll;
    char* token = argv[1];     //pitch

    if (strncmp(token, "home", 4) == 0 ||
        strncmp(token, "zero", 4) == 0)
    {
        stu.home(sp_servo);
    }
    else
    {
        pitch = atof(token);
        if (argc < 3)
        {
            Serial.print("Usage: moveto home | <pitch angle> <roll angle>");
            return SHELL_RET_FAILURE;
        }
        token = argv[2];         //roll
        roll = atof(token);
        stu.moveTo(sp_servo, pitch, roll);
    }

    // updateServos();
    return SHELL_RET_SUCCESS;
}

#ifdef ENABLE_TOUCHSCREEN
int handlePID(int argc, char** argv)
{
    char* cmd = argv[0];
    if (argc!=2)
    {
        Serial.print("Usage: %s <value>",cmd);
        return SHELL_RET_FAILURE;
    }

    // char* token = argv[1];
    double val = atof(argv[1]);
    if (strncmp(cmd, "px",2)==0)
    {
        PX=val;
        rollPID.SetTunings(PX,IX,DX);
    }
    else if (strncmp(cmd, "ix",2)==0)
    {
        IX=val;
        rollPID.SetTunings(PX,IX,DX);
    }
    else if (strncmp(cmd, "dx",2)==0)
    {
        DX=val;
        rollPID.SetTunings(PX,IX,DX);
    }
    else if (strncmp(cmd, "py",2)==0)
    {
        PY=val;
        pitchPID.SetTunings(PY,IY,DY);
    }
    else if (strncmp(cmd, "iy",2)==0)
    {
        IY=val;
        pitchPID.SetTunings(PY,IY,DY);
    }
    else if (strncmp(cmd, "dy",2)==0)
    {
        DY=val;
        pitchPID.SetTunings(PY,IY,DY);
    }
    return SHELL_RET_SUCCESS;
}
#endif

#endif  //ENABLE_SERIAL_COMMANDS
