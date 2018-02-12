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

// the SparkFun breakout board defaults to 1, set to 0 if SA0 jumper on the bottom of the board is set
// see the Table 10. I2C Device Address Sequence in Freescale MMA8452Q pdf

//#define SA0 1
//#if SA0
  //#define MMA8452_ADDRESS 0x3A  // SA0 is high, 0x1C if low - it should be 0x1D, but we shift now to save shifting in the code
//#else
  //#define MMA8452_ADDRESS 0x1C
//#endif

//#ifndef MBED_MMA8452

//#define MBED_MMA8452
 
#include "mbed.h"
 
/** Accelerometer MMA8452 class 
 *
 * Example:
 * @code
 * 
 * #include "mbed.h"
 * #include "MMA8452.h"
 * 
 * 
 * Accelerometer_MMA8452 Acc(p28, p27);
 * serial pc(USBTX, USBRX);
 *
 * int main() 
 * {
 *   Acc.init(); 
 *      while(1)
 *      {
 *          int x=0, y=0, z=0;
 *          Acc.read_Tilt(&x, &y, &z);
 *          pc.printf("Tilt x: %2.2f degree \n", x);                    // Print the tilt orientation of the X axis
 *          pc.printf("Tilt y: %2.2f degree \n", y);                    // Print the tilt orientation of the Y axis
 *          pc.printf("Tilt z: %2.2f degree \n", z);                    // Print the tilt orientation of the Z axis
 *          wait(1);       
 *      }
 * }
 * @endcode
 */ 

// More info on MCU Master address can be found on section 5.10.1 of http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
#define SA0 1
#if SA0
  #define MMA8452_ADDRESS 0x1D  // SA0 is high, 0x1C if low - 
#else
  #define MMA8452_ADDRESS 0x1C
#endif

// Register descriptions found in section 6 of pdf
#define STATUS 0x00                         // Type 'read' : Real time status, should return 0x00
#define OUT_X_MSB 0x01                      // Type 'read' : x axis - 8 most significatn bit of a 12 bit sample
#define OUT_X_LSB 0x02                      // Type 'read' : x axis - 4 least significatn bit of a 12 bit sample
#define OUT_Y_MSB 0x03                      // Type 'read' : y axis - 8 most significatn bit of a 12 bit sample
#define OUT_Y_LSB 0x04                      // Type 'read' : y axis - 4 least significatn bit of a 12 bit sample
#define OUT_Z_MSB 0x05                      // Type 'read' : z axis - 8 most significatn bit of a 12 bit sample
#define OUT_Z_LSB 0x06                      // Type 'read' : z axis - 4 least significatn bit of a 12 bit sample

#define SYSMOD 0x0B                         // Type 'read' : This tells you if device is active, sleep or standy 0x00=STANDBY 0x01=WAKE 0x02=SLEEP
#define WHO_AM_I 0x0D                       // Type 'read' : This should return the device id of 0x2A

#define PL_STATUS 0x10                      // Type 'read' : This shows portrait landscape mode orientation
#define PL_CFG 0x11                         // Type 'read/write' : This allows portrait landscape configuration
#define PL_COUNT 0x12                       // Type 'read' : This is the portraint landscape debounce counter
#define PL_BF_ZCOMP 0x13                    // Type 'read' :
#define PL_THS_REG 0x14                     // Type 'read' :

#define FF_MT_CFG 0X15                      // Type 'read/write' : Freefaul motion functional block configuration
#define FF_MT_SRC 0X16                      // Type 'read' : Freefaul motion event source register
#define FF_MT_THS 0X17                      // Type 'read' : Freefaul motion threshold register
#define FF_COUNT 0X18                       // Type 'read' : Freefaul motion debouce counter

#define ASLP_COUNT 0x29                     // Type 'read/write' : Counter settings for auto sleep
#define CTRL_REG_1 0x2A                     // Type 'read/write' :
#define CTRL_REG_2 0x2B                     // Type 'read/write' :
#define CTRL_REG_3 0x2C                     // Type 'read/write' :
#define CTRL_REG_4 0x2D                     // Type 'read/write' :
#define CTRL_REG_5 0x2E                     // Type 'read/write' :

// Defined in table 13 of the Freescale PDF
#define STANDBY 0x00                        // State value returned after a SYSMOD request, it can be in state STANDBY, WAKE or SLEEP
#define WAKE 0x01                           // State value returned after a SYSMOD request, it can be in state STANDBY, WAKE or SLEEP
#define SLEEP 0x02                          // State value returned after a SYSMOD request, it can be in state STANDBY, WAKE or SLEEP
#define ACTIVE 0x01                         // Stage value returned and set in Control Register 1, it can be STANDBY=00, or ACTIVE=01


 
#define TILT_STATUS 0x03        // Tilt Status (Read only)
#define SRST_STATUS 0x04        // Sample Rate Status Register (Read only)
#define SPCNT_STATUS 0x05       // Sleep Count Register (Read/Write)
#define INTSU_STATUS 0x06       // Interrupt Setup Register
#define MODE_STATUS 0x07        // Mode Register (Read/Write)
#define SR_STATUS 0x08          // Auto-Wake and Active Mode Portrait/Landscape Samples per Seconds Register (Read/Write)
#define PDET_STATUS 0x09        // Tap/Pulse Detection Register (Read/Write)
#define PD_STATUS 0xA           // Tap/Pulse Debounce Count Register (Read/Write)





 
class Accelerometer_MMA8452         
{        
    public:
        
        
       /** Create an accelerometer object connected to the specified I2C object
        *
        * @param sda I2C data port
        * @param scl I2C8452 clock port
        * 
        */ 
      Accelerometer_MMA8452(PinName sda, PinName scl, int frequency);
      //Accelerometer_MMA8452(PinName sda, PinName scl);
       
       /** Destroys an MMA8452 object
        *
        */
      ~Accelerometer_MMA8452();
      
      
      
      /** Get system mode of the MMA8452 (not required)
        *   returns 0 for success in reading the system mode of the chip
        *   returns 1 for failure in reading the system mode of the chip
        *   -currently no retries or waiting is done, this method tries 1 time then exits.
        *
        *   This method is used to find out the system mode of the chip ACTIVE = 0x00 or STANDBY = 0x01
      */
      int get_SystemMode(int& deviceSystemMode);
      
      
      
      /** Get status of the MMA8452 (not required)
        *   returns 0 for success in reading the status of the chip
        *   returns 1 for failure in reading the status of  the chip
        *   -currrently no retries or waiting is done, this method tries 1 time then exits.
        *
        *   This method is used to find out the real time status of the device it shows if
        *   x,y and z values have been overwritten before they have been read since a change happened.
        *   
      */
      int get_Status(int& deviceStatus);
     
      
      
      /** Activate the MMA8452 (required)
        *   returns 0 for success in activating the chip
        *   returns 1 for failure in activating the chip
        *   -currrently no retries or waiting is done, this method tries 1 time the exits.
        *
        *   This will set the device 'active' even if it's already active. It's just a way to force that state.
      */
      int activate();
 
 
 
       /** Standby the MMA8452 (not required)
        *   returns 0 for success in activating the chip
        *   returns 1 for failure in activating the chip
        *   -currrently no retries or waiting is done, this method tries 1 time the exits.
        *
        *   This will set the device 'standby' even if it's already in standby. It's just a way to force that state.
      */
      int standby();

 
       /** get_CTRL_Reg1 the MMA8452 (not required)
        *   returns 0 for success in activating the chip
        *   returns 1 for failure in activating the chip
        *   -currrently no retries or waiting is done, this method tries 1 time the exits.
        *
        *   This will return the state of the control register 1. This holds and sets values for auto wake, sleep mode
        *   output data rate, fast read mode and active/standby. More info on 6.7 of pdf for MMA8452 Freescale doc.
      */
      int get_CTRL_Reg1(int& CTRL_Reg); 
      
      
       /** Initialization of device MMA8452 (required)
        */
      void init();
    
       /** Read the device ID from the accelerometer
        *
        * @param *deviceID Value of device - is should be 0x2A
        * return 0 for success or
        * return 1 for failure.
        */        
      int get_DeviceID(int& deviceID);  
      
      int read_y();
      
      int read_z();  

      /** Read the x register of the MMA8452
        *
        * @returns The value of x acceleration
        */
      int read_x_raw(char *xaxis);
      //int read_x(int& xaxisLSB);
      /** Read the y register of the MMA8452
        *
        * @returns The value of y acceleration
        */
      int read_y_raw(char *yaxis);
      
      /** Read the z register of the MMA8452
        *
        * @returns The value of z acceleration
        */
       int read_z_raw(char * zaxis);
      
      /** Read the x,y and z registers of the MMA8452
       *
       * takes a 2 byte char buffer to store the value
       * for each axis.
       * returns 0 for success, and 1 for failure
       *
       */ 
      int read_xyz(char *x, char *y, char *z); 
            
        /** Read from specified MMA8452 register
         *
         * @param addr The internal registeraddress of the MMA8452
         * @returns The value of the register
         */
      char read_reg(char addr);
        
        /** Write to specified MMA8452 register
        *
        * @param addr The internal registeraddress of the MMA8452
        * @param data New value of the register
        */    
      void write_reg(char addr, char data); 
      
   
    private:
      I2C m_i2c;
      int m_frequency;
         
};

//#endif 
