/* =================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20                                                  
------------------------------------------------------------------------------*/
#ifndef __PARK_H__
#define __PARK_H__
typedef struct {  _iq  As;  		// Input: phase-a stator variable
				  _iq  Bs;			// Input: phase-b stator variable 
				  _iq  Alpha;		// Output: stationary d-axis stator variable 
				  _iq  Beta;		// Output: stationary q-axis stator variable
		 	 	  void  (*calc)();	// Pointer to calculation function
				 } CLARKE;	            

typedef CLARKE *CLARKE_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the CLARKE object.
-----------------------------------------------------------------------------*/                     
#define CLARKE_DEFAULTS { 0, \
                          0, \
                          0, \
                          0, \
              			  (void (*)(Uint32))clarke_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in CLARKE.C
------------------------------------------------------------------------------*/
void clarke_calc(CLARKE_handle);
//
typedef struct {  _iq  Alpha;  		// Input: stationary d-axis stator variable 
				  _iq  Beta;	 	// Input: stationary q-axis stator variable 
				  _iq  Angle;		// Input: rotating angle (pu) 
				  _iq  Ds;			// Output: rotating d-axis stator variable 
				  _iq  Qs;			// Output: rotating q-axis stator variable 
		 	 	  void  (*calc)();	// Pointer to calculation function
				 } PARK;	            

typedef PARK *PARK_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the PARK object.
-----------------------------------------------------------------------------*/                     
#define PARK_DEFAULTS {  0, \
                          0, \
                          0, \
                          0, \
                          0, \
              			  (void (*)(Uint32))park_calc }
/*------------------------------------------------------------------------------
Prototypes for the functions in PARK.C
------------------------------------------------------------------------------*/
void park_calc(PARK_handle);

typedef struct {  _iq  Alpha;  		// Output: stationary d-axis stator variable
				  _iq  Beta;		// Output: stationary q-axis stator variable
				  _iq  Angle;		// Input: rotating angle (pu)
				  _iq  Ds;			// Input: rotating d-axis stator variable
				  _iq  Qs;			// Input: rotating q-axis stator variable
		 	 	  void  (*calc)();	// Pointer to calculation function 
				 } IPARK;	            

typedef IPARK *IPARK_handle;

/*-----------------------------------------------------------------------------
Default initalizer for the IPARK object.
-----------------------------------------------------------------------------*/                     
#define IPARK_DEFAULTS {  0, \
                          0, \
                          0, \
                          0, \
                          0, \
              			  (void (*)(Uint32))ipark_calc }

/*------------------------------------------------------------------------------
 Prototypes for the functions in IPARK.C
------------------------------------------------------------------------------*/
void ipark_calc(IPARK_handle);

typedef struct {  _iq  Ref;   			// Input: Reference input 
				  _iq  Fdb;   			// Input: Feedback input 
				  _iq  Err;				// Variable: Error
				  _iq  Kp;				// Parameter: Proportional gain
				  _iq  Up;				// Variable: Proportional output 
				  _iq  Ui;				// Variable: Integral output 
				  _iq  Ud;				// Variable: Derivative output 	
				  _iq  OutPreSat; 		// Variable: Pre-saturated output
				  _iq  OutMax;		    // Parameter: Maximum output 
				  _iq  OutMin;	    	// Parameter: Minimum output
				  _iq  Out;   			// Output: PID output 
				  _iq  SatErr;			// Variable: Saturated difference
				  _iq  Ki;			    // Parameter: Integral gain
				  _iq  Kc;		     	// Parameter: Integral correction gain
				  _iq  Kd; 		        // Parameter: Derivative gain
				  _iq  ErrOld;		   	// History: Previous proportional output
		 	 	  _iq  Up1;				// History:
		 	 	  void  (*calc)();	  	// Pointer to calculation function
				 } PIDREG3;	            

typedef PIDREG3 *PIDREG3_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the PIDREG3 object.
-----------------------------------------------------------------------------*/                     
#define PIDREG3_DEFAULTS { 0, \
                           0, \
                           0, \
                           _IQ(1.3), \
                           0, \
                           0, \
                           0, \
                           0, \
                           _IQ(1), \
                           _IQ(-1), \
                           0, \
                           0, \
                           _IQ(0.02), \
                           _IQ(0.5), \
                           _IQ(1.05), \
                           0,\
                           0,\
              			  (void (*)(Uint32))pid_reg3_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in PIDREG3.C
------------------------------------------------------------------------------*/
void pid_reg3_calc(PIDREG3_handle);

typedef struct 	{ _iq  Ualpha; 			// Input: reference alpha-axis phase voltage 
				  _iq  Ubeta;			// Input: reference beta-axis phase voltage 
				  _iq  Ta;				// Output: reference phase-a switching function		
				  _iq  Tb;				// Output: reference phase-b switching function 
				  _iq  Tc;				// Output: reference phase-c switching function
				  void (*calc)();	    // Pointer to calculation function
				} SVGENDQ;
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
typedef SVGENDQ *SVGENDQ_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the SVGENDQ object.
-----------------------------------------------------------------------------*/                     
#define SVGENDQ_DEFAULTS { 0,0,0,0,0, \
                       (void (*)(Uint32))svgendq_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in SVGEN_DQ.C
------------------------------------------------------------------------------*/
void svgendq_calc(SVGENDQ_handle);

//--------------------
typedef struct {  _iq  Ref;   			// Input: Reference input 
				  _iq  Fdb;   			// Input: Feedback input 
				  _iq  Err;				// Variable: Error
				  _iq  Kp;				// Parameter: Proportional gain
				  _iq  Up;				// Variable: Proportional output 
				  _iq  Ui;				// Variable: Integral output 
				  _iq  Ud;				// Variable: Derivative output 	
				  _iq  OutPreSat; 		// Variable: Pre-saturated output
				  _iq  OutMax;		    // Parameter: Maximum output 
				  _iq  OutMin;	    	// Parameter: Minimum output
				  _iq  Out;   			// Output: PID output 
				  _iq  SatErr;			// Variable: Saturated difference
				  _iq  Ki;			    // Parameter: Integral gain
				  _iq  Kc;		     	// Parameter: Integral correction gain
				  _iq  Kv; 		        // Parameter: Derivative gain
				  _iq  ErrOld;		   	    // History: Previous proportional output
		 	 	  void  (*calc)();	  	// Pointer to calculation function
				 } PID;	            

typedef PID *PID_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the PIDREG3 object.
-----------------------------------------------------------------------------*/                     
#define PID_DEFAULTS { 0, \
                           0, \
                           0, \
                           _IQ(1.3), \
                           0, \
                           0, \
                           0, \
                           0, \
                           _IQ(1), \
                           _IQ(-1), \
                           0, \
                           0, \
                           _IQ(0.02), \
                           _IQ(0.5), \
                           _IQ(1.05), \
                           0, \
              			  (void (*)(Uint32))pid_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in PIDREG3.C
------------------------------------------------------------------------------*/
void pid_calc(PID_handle);

#endif // __PARK_H__
