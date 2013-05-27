//!
//! @file 		Pid.h
//! @author 	Geoffrey Hunter <gbmhunter@gmail.com> (www.cladlab.com)
//! @edited 	n/a
//! @date 		2012/10/01
//! @brief 		Header file for Pid.c
//! @details
//!				See README.rst



//===============================================================================================//
//====================================== HEADER GUARD ===========================================//
//===============================================================================================//

#ifndef __cplusplus
	#error Please build with C++ compiler
#endif

#ifndef PID_H
#define PID_H

//===============================================================================================//
//================================== PRECOMPILER CHECKS =========================================//
//===============================================================================================//


#define pidENABLE_FP_SUPPORT 0

//! @brief		Set to 1 if you want debug information printed
#define pidPRINT_DEBUG			1

//! @brief		Size (in bytes) of the debug buffer (only valid if
//!				pidPRINT_DEBUG is set to 1).
#define pidDEBUG_BUFF_SIZE		200

//===============================================================================================//
//======================================== NAMESPACE ============================================//
//===============================================================================================//

#if(pidENABLE_FP_SUPPORT == 1)
	#include "./FixedPoint/include/Fp.h"
#endif

#include <stdint.h>


namespace Pid
{
	#if(pidENABLE_FP_SUPPORT == 1)
		using namespace Fp;
	#endif
	//===============================================================================================//
	//=================================== PUBLIC TYPEDEFS ===========================================//
	//===============================================================================================//



	//! @brief		Base PID class. 
	class PidDataBase
	{
	
		public:
			
			typedef enum			//!< Enumerates the controller direction modes
			{
				PID_DIRECT,			//!< Direct drive (+error gives +output)
				PID_REVERSE			//!< Reverse driver (+error gives -output)
			} ctrlDir_t;

			//! Used to determine wether the output shouldn't be accumulated (distance control),
			//! or accumulated (velocity control).
			typedef enum
			{
				DONT_ACCUMULATE_OUTPUT,
				ACCUMULATE_OUTPUT,
				DISTANCE_PID = DONT_ACCUMULATE_OUTPUT,
				VELOCITY_PID = ACCUMULATE_OUTPUT
			} outputMode_t;
		
			ctrlDir_t controllerDir;
			outputMode_t outputMode;	//!< The output mode (non-accumulating vs. accumulating) for the control loop
	};

	#if(pidENABLE_FP_SUPPORT == 1)
	//! @brief		PID class that uses fixed-point numbers for it's arithmetic.
	//! @details	The fixed-point library can be downloaded from BitBucket also.
	class PidFp : public PidDataBase
	{
		public:
		
			//! @brief 		Init function
			//! @details   	The parameters specified here are those for for which we can't set up 
			//!    			reliable defaults, so we need to have the user set them.
			void Init(
				fp<CDP> kp, fp<CDP> ki, fp<CDP> kd, 
				ctrlDir_t controllerDir, outputMode_t outputMode, fp<CDP> samplePeriodMs, 
				fp<CDP> minOutput, fp<CDP> maxOutput, fp<CDP> setPoint);
		
			//! @brief 		Computes new PID values
			//! @details 	Call once per sampleTimeMs. Output is stored in the pidData structure.
			void Run(fp<CDP> input);
			
			void SetOutputLimits(fp<CDP> min, fp<CDP> max);
			
			//! @details	The PID will either be connected to a direct acting process (+error leads to +output, aka inputs are positive) 
			//!				or a reverse acting process (+error leads to -output, aka inputs are negative)
			void SetControllerDirection(ctrlDir_t controllerDir);
			
			//! @brief		This function allows the controller's dynamic performance to be adjusted. 
			//! @details	It's called automatically from the init function, but tunings can also
			//! 			be adjusted on the fly during normal operation
			void SetTunings(fp<CDP> kp, fp<CDP> ki, fp<CDP> kd);
		
			fp<CDP> zKp;					//!< Time-step scaled proportional constant for quick calculation (equal to actualKp)
			fp<CDP> zKi;					//!< Time-step scaled integral constant for quick calculation
			fp<CDP> zKd;					//!< Time-step scaled derivative constant for quick calculation
			fp<CDP> actualKp;			//!< Actual (non-scaled) proportional constant
			fp<CDP> actualKi;			//!< Actual (non-scaled) integral constant
			fp<CDP> actualKd;			//!< Actual (non-scaled) derivative constant
			fp<CDP> prevInput;			//!< Actual (non-scaled) proportional constant
			fp<CDP> inputChange;			//!< The change in input between the current and previous value
			fp<CDP> setPoint;			//!< The set-point the PID control is trying to make the output converge to.
			fp<CDP> error;				//!< The error between the set-point and actual output (set point - output, positive
										//!< when actual output is lagging set-point
			fp<CDP> output;				//!< The control output. This is updated when Pid_Run() is called
			fp<CDP> prevOutput;			//!< The output value calculated the previous time Pid_Run was called
			fp<CDP> samplePeriodMs;		//!< The sample period (in milliseconds) between successive Pid_Run() calls.
										//!< The constants with the z prefix are scaled according to this value.
			fp<CDP> pTerm;				//!< The proportional term that is summed as part of the output (calculated in Pid_Run())
			fp<CDP> iTerm;				//!< The integral term that is summed as part of the output (calculated in Pid_Run())
			fp<CDP> dTerm;				//!< The derivative term that is summed as part of the output (calculated in Pid_Run())
			fp<CDP> outMin;				//!< The minimum output value. Anything lower will be limited to this floor.
			fp<CDP> outMax;				//!< The maximum output value. Anything higher will be limited to this ceiling.
	};	
	#endif	// #if(pidENABLE_FP_SUPPORT == 1)
	
	//! @brief		PID class that uses doubles for it's arithmetic
	class PidDbl : public PidDataBase
	{
		public:
		
			//! @brief 		Init function
			//! @details   	The parameters specified here are those for for which we can't set up 
			//!    			reliable defaults, so we need to have the user set them.
			void Init(
				double kp, double ki, double kd, 
				ctrlDir_t controllerDir, outputMode_t outputMode, double samplePeriodMs, 
				double minOutput, double maxOutput, double setPoint);
			
			//! @brief 		Computes new PID values
			//! @details 	Call once per sampleTimeMs. Output is stored in the pidData structure.
			void Run(double input);
			
			void SetOutputLimits(double min, double max);
		
			//! @details	The PID will either be connected to a direct acting process (+error leads to +output, aka inputs are positive) 
			//!				or a reverse acting process (+error leads to -output, aka inputs are negative)
			void SetControllerDirection(ctrlDir_t controllerDir);
			
			//! @brief		Changes the sample time
			void SetSamplePeriod(uint32_t newSamplePeriodMs);
			
			//! @brief		This function allows the controller's dynamic performance to be adjusted. 
			//! @details	It's called automatically from the init function, but tunings can also
			//! 			be adjusted on the fly during normal operation
			void SetTunings(double kp, double ki, double kd);

			double GetKp();

			double GetKi();

			double GetKd();

			double GetZp();

			double GetZi();

			double GetZd();

			//! @brief		Prints debug information to the desired output
			void PrintDebug(const char* msg);

			//! @brief 		The set-point the PID control is trying to make the output converge to.
			double setPoint;			

			//! @brief		The control output. 
			//! @details	This is updated when Pid_Run() is called.
			double output;				
		
		private:
			#if(pidPRINT_DEBUG == 1)
				char debugBuff[pidDEBUG_BUFF_SIZE];
			#endif

			//! Time-step scaled proportional constant for quick calculation (equal to actualKp)
			double zKp;		
			
			//! Time-step scaled integral constant for quick calculation
			double zKi;	
			
			//! Time-step scaled derivative constant for quick calculation
			double zKd;					
			
			//! Actual (non-scaled) proportional constant
			double actualKp;			
			
			//! Actual (non-scaled) integral constant
			double actualKi;			
			
			//! Actual (non-scaled) derivative constant
			double actualKd;			
			
			//! Actual (non-scaled) proportional constant
			double prevInput;			
			
			//! The change in input between the current and previous value
			double inputChange;			
	
			double error;				//!< The error between the set-point and actual output (set point - output, positive
											//!< when actual output is lagging set-point
			
			//! @brief 		The output value calculated the previous time Pid_Run() was called.
			//! @details	Used in ACCUMULATE_OUTPUT mode.
			double prevOutput;		

			//! @brief		The sample period (in milliseconds) between successive Pid_Run() calls.
			//! @details	The constants with the z prefix are scaled according to this value.
			double samplePeriodMs;	
											
			double pTerm;				//!< The proportional term that is summed as part of the output (calculated in Pid_Run())
			double iTerm;				//!< The integral term that is summed as part of the output (calculated in Pid_Run())
			double dTerm;				//!< The derivative term that is summed as part of the output (calculated in Pid_Run())
			double outMin;				//!< The minimum output value. Anything lower will be limited to this floor.
			double outMax;				//!< The maximum output value. Anything higher will be limited to this ceiling.	

			//! @brief		Counts the number of times that Run() has be called. Used to stop
			//!				derivative control from influencing the output on the first call.
			//! @details	Safely stops counting once it reaches 2^32-1 (rather than overflowing). 
			uint32_t numTimesRan;
	};

} // namespace Pid

#endif // #ifndef PID_H

// EOF