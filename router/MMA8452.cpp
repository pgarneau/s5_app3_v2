// Author: Nicholas Herriot
/* Copyright (c) 2013 Vodafone, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

# include "MMA8452.h"






// Connect module at I2C address using I2C port pins sda and scl
Accelerometer_MMA8452::Accelerometer_MMA8452(PinName sda, PinName scl,int frequency) : m_i2c(sda, scl) , m_frequency(frequency)
{
    //m_i2c.frequency(m_frequency);
}


// Destroys instance
Accelerometer_MMA8452::~Accelerometer_MMA8452()
{

}

// Setting the control register bit 1 to true to activate the MMA8452
int Accelerometer_MMA8452::activate()
{
    char mcu_address = (MMA8452_ADDRESS<<1);
    char init[2];
    init[0] = CTRL_REG_1;                 // control register 1
    init[1] = ACTIVE;                     // set to active
    
    if(m_i2c.write(mcu_address,init,2) == 0)
    {
         return 0;  // return 0 to indicate success
    }
    else
    {
        return 1;   // crumbs it failed!!!
    }
         
}


// Get 'Fast Read Mode' called F_READ. If bit 1 is set '1' then F_READ is active. Fast read will skip LSB when reading xyz
// resisters from 0x01 to 0x06. When F_READ is '0' then all 6 registers will be read.

int Accelerometer_MMA8452::get_CTRL_Reg1(int& CTRL_Reg)
{
{
    char mcu_address = (MMA8452_ADDRESS<<1);
    m_i2c.start();
    if( m_i2c.write( mcu_address & 0xFE) == 0)          // just good practice to force bit 1 to a '0' by ANDing with 0xFE
    {
        return 1;                                       // we failed to write the mcu address on the bus to initiate dialogue 
    }
    if( m_i2c.write( CTRL_REG_1) == 0) 
    {
        return 1;                                       // we failed to write 'status' to the chip
    }
    m_i2c.start();
    if( m_i2c.write( mcu_address | 0x01) == 0)          // this is asking to read the slave mcu address - even though it's a 'write' method!!! Crap API...
    {
        return 1;                                       // we failed to request a read from that mcu - this really is just writing the mcu vaule on the bus
    }
    CTRL_Reg  = m_i2c.read(0);
    m_i2c.stop();
    return 0;
    

}




}

// Setting the control register bit 1 to true to activate the MMA8452
int Accelerometer_MMA8452::standby()
{
    char mcu_address = (MMA8452_ADDRESS<<1);
    char init[2];
    init[0] = CTRL_REG_1;                 // control register 1
    init[1] = STANDBY;                    // set to standby
    
    if(m_i2c.write(mcu_address,init,2) == 0)
    {
         // pc.printf("The initialisation worked");
         return 0;  // return 0 to indicate success
    }
    else
    {
        // pc.printf("The initialisation failed");
        return 1;   // crumbs it failed!!!
    }
         
}



// Device initialization
void Accelerometer_MMA8452::init()
{
    
    write_reg(INTSU_STATUS, 0x10);      // automatic interrupt after every measurement
    write_reg(SR_STATUS, 0x00);         // 120 Samples/Second
    write_reg(MODE_STATUS, 0x01);       // Active Mode
    
}

// Get real time status of device - it can be STANDBY, WAKE or SLEEP
int Accelerometer_MMA8452::get_SystemMode(int& deviceSystemMode)
{
    char mcu_address = (MMA8452_ADDRESS<<1);
    m_i2c.start();
    if( m_i2c.write( mcu_address & 0xFE) == 0)          // just good practice to force bit 1 to a '0' by ANDing with 0xFE
    {
        return 1;                                       // we failed to write the mcu address on the bus to initiate dialogue 
    }
    if( m_i2c.write( SYSMOD) == 0) 
    {
        return 1;                                       // we failed to write 'status' to the chip
    }
    m_i2c.start();
    if( m_i2c.write( mcu_address | 0x01) == 0)          // this is asking to read the slave mcu address - even though it's a 'write' method!!! Crap API...
    {
        return 1;                                       // we failed to request a read from that mcu - this really is just writing the mcu vaule on the bus
    }
    deviceSystemMode  = m_i2c.read(0);
    m_i2c.stop();
    return 0;
    

}



// Get real time status of device - it can be STANDBY, WAKE or SLEEP
int Accelerometer_MMA8452::get_Status(int& deviceStatus)
{
    char mcu_address = (MMA8452_ADDRESS<<1);
    m_i2c.start();
    if( m_i2c.write( mcu_address & 0xFE) == 0)          // just good practice to force bit 1 to a '0' by ANDing with 0xFE
    {
        return 1;                                       // we failed to write the mcu address on the bus to initiate dialogue 
    }
    if( m_i2c.write( STATUS) == 0) 
    {
        return 1;                                       // we failed to write 'status' to the chip
    }
    m_i2c.start();
    if( m_i2c.write( mcu_address | 0x01) == 0)          // this is asking to read the slave mcu address - even though it's a 'write' method!!! Crap API...
    {
        return 1;                                       // we failed to request a read from that mcu - this really is just writing the mcu vaule on the bus
    }
    deviceStatus  = m_i2c.read(0);
    m_i2c.stop();
    return 0;
    

}


// Get device ID 
int Accelerometer_MMA8452::get_DeviceID(int& deviceID)
{
    char mcu_address = (MMA8452_ADDRESS<<1);
    m_i2c.start();
    if( m_i2c.write( mcu_address & 0xFE) == 0)          // just good practice to force bit 1 to a '0' by ANDing with 0xFE
    {
        return 1;                                       // we failed to write the mcu address on the bus to initiate dialogue 
    }
    if( m_i2c.write( WHO_AM_I) == 0) 
    {
        return 1;                                       // we failed to write 'who am i' to the chip
    }
    m_i2c.start();
    if( m_i2c.write( mcu_address | 0x01) == 0)          // this is asking to read the slave mcu address - even though it's a 'write' method!!! Crap API...
    {
        return 1;                                       // we failed to request a read from that mcu - this really is just writing the mcu vaule on the bus
    }
    deviceID  = m_i2c.read(0);
    m_i2c.stop();
    return 0;

}


/*
// Reads x data
int Accelerometer_MMA8452::read_x(int& xaxisLSB)
{
    char mcu_address = (MMA8452_ADDRESS<<1);
    m_i2c.start();
    if( m_i2c.write( mcu_address & 0xFE) == 0)          // just good practice to force bit 1 to a '0' by ANDing with 0xFE
    {
        return 1;                                       // we failed to write the mcu address on the bus to initiate dialogue 
    }
    if( m_i2c.write( OUT_X_MSB) == 0) 
    {
        return 1;                                       // we failed to write 'X axis LSB' to the chip
    }
    m_i2c.start();
    if( m_i2c.write( mcu_address | 0x01) == 0)          // this is asking to read the slave mcu address - even though it's a 'write' method!!! Crap API...
    {
        return 1;                                       // we failed to request a read from that mcu - this really is just writing the mcu vaule on the bus
    }
    xaxisLSB  = m_i2c.read(0);
    m_i2c.stop();
    return 0;
}
*/


// Reads x data. This method reads two registers containing the x-axis values from the accelerometer. 
// It takes a 2 byte char array and copies the register values into the buffer. If it fails the char array
// is set to '0'. It returns '0' success '1' fail. This method does nothing to the registers - just returns
// the raw data.
//int Accelerometer_MMA8452::read_x(int& xaxisLSB)
int Accelerometer_MMA8452::read_x_raw(char *xaxis)
{
    char mcu_address = (MMA8452_ADDRESS<<1);                // this is the slave address on the bus we want data from
    char xaxis_buffer[2];                                   // this will contain data from that register
    char xaxis_register[1];                     
    xaxis_register[0] = OUT_X_MSB;                          // this is the register we want to get data from
    //signed short s = 0;
    
    if(m_i2c.write(mcu_address,xaxis_register,1,true) == 0)
    {
        if(m_i2c.read(mcu_address,xaxis_buffer,2) == 0)
        {
            //strcpy(xaxis, xaxis_buffer);
            memcpy(xaxis, xaxis_buffer, 2);
            //xaxis[0] = 0x00;                        // make sure the array is set to zero
            //xaxis[1] = 0x00;
            //s = *reinterpret_cast<short*>(&xaxis);
            return 0;                               // great we got the two octets
        }
        else
        {
            xaxis[0] = 0x00;                        // make sure the array is set to zero
            xaxis[1] = 0x00;
            return 1;                               // failed to read the 12 bit x value
        }
    }
    else
    {
        xaxis[0] = 0x00;                            // make sure the array is set to zero
        xaxis[1] = 0x00;
        return 1;                                   // failed to write and request the OUT_X_MSB bit
    }    
}


// Reads y data. This method reads two registers containing the x-axis values from the accelerometer. 
// It takes a 2 byte char array and copies the register values into the buffer. If it fails the char array
// is set to '0'. It returns '0' success '1' fail. This method does nothing to the registers - just returns
// the raw data.

int Accelerometer_MMA8452::read_y_raw(char *yaxis)
{
    char mcu_address = (MMA8452_ADDRESS<<1);                // this is the slave address on the bus we want data from
    char yaxis_buffer[2];                                   // this will contain data from that register
    char yaxis_register[1];                     
    yaxis_register[0] = OUT_Y_MSB;                          // this is the register we want to get data from
    //signed short s = 0;
    
    if(m_i2c.write(mcu_address,yaxis_register,1,true) == 0)
    {
        if(m_i2c.read(mcu_address,yaxis_buffer,2) == 0)
        {
            //strcpy(yaxis, yaxis_buffer);
            memcpy(yaxis, yaxis_buffer, 2);
            //yaxis[0] = 0x00;                        // make sure the array is set to zero
            //yaxis[1] = 0x00;
            //s = *reinterpret_cast<short*>(&xaxis);
            return 0;                               // great we got the two octets
        }
        else
        {
            yaxis[0] = 0x00;                        // make sure the array is set to zero
            yaxis[1] = 0x00;
            return 1;                               // failed to read the 12 bit y value
        }
    }
    else
    {
        yaxis[0] = 0x00;                            // make sure the array is set to zero
        yaxis[1] = 0x00;
        return 1;                                   // failed to write and request the OUT_Y_MSB bit
    }    
}



// Reads z data. This method reads two registers containing the x-axis values from the accelerometer. 
// It takes a 2 byte char array and copies the register values into the buffer. If it fails the char array
// is set to '0'. It returns '0' success '1' fail. This method does nothing to the registers - just returns
// the raw data.

int Accelerometer_MMA8452::read_z_raw(char *zaxis)
{
    char mcu_address = (MMA8452_ADDRESS<<1);                // this is the slave address on the bus we want data from
    char zaxis_buffer[2];                                   // this will contain data from that register
    char zaxis_register[1];                     
    zaxis_register[0] = OUT_Z_MSB;                          // this is the register we want to get data from
    //signed short s = 0;
    
    if(m_i2c.write(mcu_address,zaxis_register,1,true) == 0)
    {
        //if(m_i2c.read(mcu_address,zaxis_buffer,2) == 0)
        if(m_i2c.read(mcu_address,zaxis,2) == 0)
        {
            //strcpy(yaxis, yaxis_buffer);
            //memcpy(zaxis, zaxis_buffer, 2);
            //yaxis[0] = 0x00;                        // make sure the array is set to zero
            //yaxis[1] = 0x00;
            //s = *reinterpret_cast<short*>(&xaxis);
            return 0;                               // great we got the two octets
        }
        else
        {
            zaxis[0] = 0x00;                        // make sure the array is set to zero
            zaxis[1] = 0x00;
            return 1;                               // failed to read the 12 bit y value
        }
    }
    else
    {
        zaxis[0] = 0x00;                            // make sure the array is set to zero
        zaxis[1] = 0x00;
        return 1;                                   // failed to write and request the OUT_Y_MSB bit
    }    
}



// Reads y data
int Accelerometer_MMA8452::read_y()
{
    char mcu_address = (MMA8452_ADDRESS <<1);

    m_i2c.start();                  // Start
    m_i2c.write(mcu_address);              // A write to device 0x98
    m_i2c.write(OUT_Y_MSB);             // Register to read
    m_i2c.start();                  
    m_i2c.write(mcu_address);              // Read from device 0x99
    int y = m_i2c.read(0);         // Read the data
    m_i2c.stop();
    
    return y; 

}


// Reads z data
int Accelerometer_MMA8452::read_z()
{
    char mcu_address = (MMA8452_ADDRESS <<1);
    
    m_i2c.start();                  // Start
    m_i2c.write(mcu_address);              // A write to device 0x98
    m_i2c.write(OUT_Z_MSB);             // Register to read
    m_i2c.start();                  
    m_i2c.write(mcu_address);              // Read from device 0x99
    int z = m_i2c.read(0);         // Read the data
    m_i2c.stop();
    
    return z;

}


// Reads xyz
int Accelerometer_MMA8452::read_xyz(char *x, char *y, char *z) 
{
    
    
    char mcu_address = (MMA8452_ADDRESS <<1);
    char register_buffer[6] ={0,0,0,0,0,0};
    const char Addr_X = OUT_X_MSB;
    m_i2c.write(mcu_address);              // A write to device 0x98
    m_i2c.write(MMA8452_ADDRESS, &Addr_X, 1);         // Pointer to the OUT_X_MSB register
    
    if(m_i2c.write(mcu_address,&Addr_X,1) == 0)
    {
        if(m_i2c.read(mcu_address,register_buffer,6) == 0)
        {
            *x = register_buffer[1];
            *y = register_buffer[3];
            *z = register_buffer[5];
            return 0;           // yahoooooo
        }
        else
        {
            return 1;           // failed oh nooo!
        }    
    }
    else
    {
        return 1;               // failed oh nooo!
    }

}

        // Write register (The device must be placed in Standby Mode to change the value of the registers) 
void Accelerometer_MMA8452::write_reg(char addr, char data)
{

    char cmd[2] = {0, 0};
    
    cmd[0] = MODE_STATUS;
    cmd[1] = 0x00;                      // Standby Mode on
    m_i2c.write(MMA8452_ADDRESS, cmd, 2);
  
    cmd[0] = addr;
    cmd[1] = data;                      // New value of the register
    m_i2c.write(MMA8452_ADDRESS, cmd, 2); 
      
    cmd[0] = MODE_STATUS;
    cmd[1] = 0x01;                      // Active Mode on
    m_i2c.write(MMA8452_ADDRESS, cmd, 2);
                  
}



        // Read from specified MMA7660FC register
char Accelerometer_MMA8452::read_reg(char addr)
{
    
    m_i2c.start();                  // Start
    m_i2c.write(0x98);              // A write to device 0x98
    m_i2c.write(addr);              // Register to read
    m_i2c.start();                  
    m_i2c.write(0x99);              // Read from device 0x99
    char c = m_i2c.read(0);         // Read the data
    m_i2c.stop();                   
 
    return c;
    
}






