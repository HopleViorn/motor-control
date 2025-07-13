/*  =================================================================================
File name:        F2812QEP.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments
Description:  
Header file containing data type and object definitions and 
initializers. Also contains prototypes for the functions in F280XQEP.C.
====================================================================================
History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Using DSP280x v. 1.10 or higher 
----------------------------------------------------------------------------------*/

#ifndef __QEP_H__
#define __QEP_H__


//==============================================================================
// more
//
typedef struct {int16 ElecTheta;        // Output: Motor Electrical angle (Q24)
                int16 MechTheta;        // Output: Motor Mechanical Angle (Q24) 
                int16 EncInc;			// Output: 
				int32 Encoder;			// Output: 
                int16 QPOSCNT_old;		//
				int16 MechQPos;
				
                //Uint16 DirectionQep;    // Output: Motor rotation direction (Q0)
                Uint16 MechScaler;      // Parameter: 0.9999/total count (Q30) 
                //Uint16 LineEncoder;     // Parameter: Number of line encoder (Q0) 
                Uint16 PolePairs;       // Parameter: Number of pole pairs (Q0) 
                int16  CalibratedAngle; // Parameter: Raw angular offset between encoder index and phase a (Q0)
                void (*calc)();         // Pointer to the calc function 
                }QEP;

/*-----------------------------------------------------------------------------
Define a QEP_handle
-----------------------------------------------------------------------------*/
typedef QEP *QEP_handle;
/*-----------------------------------------------------------------------------
Default initializer for the QEP Object.注意修改某些参数
-----------------------------------------------------------------------------*/
// Applied-motion PMSM motor: 24-v, 8-pole, 2000 line encoder, CalibratedAngle = -1250
// MechScaler = 1/8000 = 0x00020C4A (Q30) 

// PacSci 1-hp PMSM motor: 320-v, 4-pole, 1000 line encoder, CalibratedAngle = -2365
// MechScaler = 1/4000 = 0x00041893 (Q30)

#define QEP_DEFAULTS { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,4,1250,  \
                (void (*)(Uint32))QEP_Calc }

/*-----------------------------------------------------------------------------
Prototypes for the functions in F280XQEP.C                                 
-----------------------------------------------------------------------------*/
//void F280X_QEP_Init(QEP_handle);                                              
void QEP_Calc(QEP_handle);


#endif // __QEP_H__ 

//===========================================================================
// End of file.
//===========================================================================


