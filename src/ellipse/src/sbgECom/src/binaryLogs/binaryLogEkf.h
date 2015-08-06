/*!
 *	\file		binaryLogEkf.h
 *  \author		SBG Systems (Raphael Siryani)
 *	\date		25 February 2013
 *
 *	\brief		This file is used to parse received EKF compued data binary logs.
 *
 *	\section CodeCopyright Copyright Notice 
 *	Copyright (C) 2007-2013, SBG Systems SAS. All rights reserved.
 *	
 *	This source code is intended for use only by SBG Systems SAS and
 *	those that have explicit written permission to use it from
 *	SBG Systems SAS.
 *	
 *	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *	KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *	IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 *	PARTICULAR PURPOSE.
 */
#ifndef __BINARY_LOG_EKF_H__
#define __BINARY_LOG_EKF_H__

#include "../sbgCommon.h"

//----------------------------------------------------------------------//
//- Solution status definitions                                        -//
//----------------------------------------------------------------------//

/*!
 * Solution status mode definitions.
 */
#define SBG_ECOM_SOLUTION_MODE_SHIFT		(0u)					/*!< Shift used to extract the clock status part. */
#define SBG_ECOM_SOLUTION_MODE_MASK			(0x0000000Fu)			/*!< Mask used to keep only the clock status part. */

/*!
 * Solution bit masks definitions.
 */
#define SBG_ECOM_SOL_ATTITUDE_VALID			(0x00000001u << 4)		/*!< Set to 1 if attitude data is reliable (Roll/Pitch error < 0,5�). */
#define SBG_ECOM_SOL_HEADING_VALID			(0x00000001u << 5)		/*!< Set to 1 if geading data is reliable (Heading error < 1�). */
#define SBG_ECOM_SOL_VELOCITY_VALID			(0x00000001u << 6)		/*!< Set to 1 if velocity data is reliable (velocity error < 1.5 m/s). */
#define SBG_ECOM_SOL_POSITION_VALID			(0x00000001u << 7)		/*!< Set to 1 if position data is reliable (Position error < 10m). */
#define SBG_ECOM_SOL_VERT_REF_USED			(0x00000001u << 8)		/*!< Set to 1 if vertical reference is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_MAG_REF_USED			(0x00000001u << 9)		/*!< Set to 1 if magnetometer is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_GPS1_VEL_USED			(0x00000001u << 10)		/*!< Set to 1 if GPS1 velocity is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_GPS1_POS_USED			(0x00000001u << 11)		/*!< Set to 1 if GPS1 Position is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_GPS1_COURSE_USED		(0x00000001u << 12)		/*!< Set to 1 if GPS1 Course is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_GPS1_HDT_USED			(0x00000001u << 13)		/*!< Set to 1 if GPS1 True Heading is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_GPS2_VEL_USED			(0x00000001u << 14)		/*!< Set to 1 if GPS2 velocity is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_GPS2_POS_USED			(0x00000001u << 15)		/*!< Set to 1 if GPS2 Position is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_GPS2_COURSE_USED		(0x00000001u << 16)		/*!< Set to 1 if GPS2 Course is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_GPS2_HDT_USED			(0x00000001u << 17)		/*!< Set to 1 if GPS2 True Heading is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_ODO_USED				(0x00000001u << 18)		/*!< Set to 1 if Odometer is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_DVL_BT_USED			(0x00000001u << 19)		/*!< Set to 1 if DVL Bottom Tracking is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_DVL_WT_USED			(0x00000001u << 20)		/*!< Set to 1 if DVL Water Tracking is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_USER_POS_USED			(0x00000001u << 21)		/*!< Set to 1 if user velocity is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_USER_VEL_USED			(0x00000001u << 22)		/*!< Set to 1 if user Position is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_USER_HEADING_USED		(0x00000001u << 23)		/*!< Set to 1 if user Course is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_USBL_USED				(0x00000001u << 24)		/*!< Set to 1 if USBL / LBL is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_PRESSURE_USED			(0x00000001u << 25)		/*!< Set to 1 if pressure (depth or altimeter) is used in solution (data used and valid since 3s). */
#define SBG_ECOM_SOL_ZUPT_USED				(0x00000001u << 26)		/*!< Set to 1 if a ZUPT is used in solution (data used and valid since 3s). */

/*!
 * Solution filter mode enum.
 */
typedef enum _SbgEComSolutionMode
{
	SBG_ECOM_SOL_MODE_UNINITIALIZED			= 0,					/*!< The Kalman filter is not initialized and the returned data are all invalid. */
	SBG_ECOM_SOL_MODE_VERTICAL_GYRO			= 1,					/*!< The Kalman filter only rely on a vertical reference to compute roll and pitch angles. Heading and navigation data drift freely. */
	SBG_ECOM_SOL_MODE_AHRS					= 2,					/*!< A heading reference is available, the Kalman filter provides full orientation but navigation data drift freely. */
	SBG_ECOM_SOL_MODE_NAV_VELOCITY			= 3,					/*!< The Kalman filter computes orientation and velocity. Position is freely integrated from velocity estimation. */
	SBG_ECOM_SOL_MODE_NAV_POSITION			= 4						/*!< Nominal mode, the Kalman filter computes all parameters (attitude, velocity, position). Absolute position is provided. */
} SbgEComSolutionMode;

//----------------------------------------------------------------------//
//- Solution status helpers methods                                    -//
//----------------------------------------------------------------------//

/*!
 * Method used to read the solution mode from a solution status field.
 * \param[in]	status				Status uint32 value to extract the solution mode from it.
 * \return							The extracted solution mode.
 */
SBG_INLINE SbgEComSolutionMode sbgEComLogEkfGetSolutionMode(uint32 status)
{
	return (SbgEComSolutionMode)((status >> SBG_ECOM_SOLUTION_MODE_SHIFT) & SBG_ECOM_SOLUTION_MODE_MASK);
}

/*!
 * Method used to write the solution status field.
 * \param[in]	solutionMode		The solution mode to set.
 * \param[in]	masks				Bit mask to set.
 * \return							The build solution status field.
 */
SBG_INLINE uint32 sbgEComLogEkfBuildSolutionStatus(SbgEComSolutionMode solutionMode, uint32 masks)
{
	//
	// Create the combined status field
	//
	return	((((uint32)solutionMode)&SBG_ECOM_SOLUTION_MODE_MASK) << SBG_ECOM_SOLUTION_MODE_SHIFT) | masks;
}


//----------------------------------------------------------------------//
//- Log structure definitions                                          -//
//----------------------------------------------------------------------//

/*!
 * EKF computed orientation using euler angles.
 */
typedef struct _SbgLogEkfEulerData
{
	uint32	timeStamp;				/*!< Time in us since the sensor power up. */
	float	euler[3];				/*!< Roll, Pitch and Yaw angles in rad. */
	float	eulerStdDev[3];			/*!< Roll, Pitch and Yaw angles 1 sigma standard deviation in rad. */
	uint32	status;					/*!< EKF solution status bitmask and enum. */
} SbgLogEkfEulerData;

/*!
 * EFK computed orientation using quaternion.
 */
typedef struct _SbgLogEkfQuatData
{
	uint32	timeStamp;				/*!< Time in us since the sensor power up. */
	float	quaternion[4];			/*!< Orientation quaternion stored in W, X, Y, Z form. */
	float	eulerStdDev[3];			/*!< Roll, Pitch and Yaw angles 1 sigma standard deviation in rad. */
	uint32	status;					/*!< EKF solution status bitmask and enum. */
} SbgLogEkfQuatData;

/*!
 * EFK computed navigation data.
 */
typedef struct _SbgLogEkfNavData
{
	uint32	timeStamp;				/*!< Time in us since the sensor power up. */
	float	velocity[3];			/*!< North, East, Down velocity in m.s^-1. */
	float	velocityStdDev[3];		/*!< North, East, Down velocity 1 sigma standard deviation in m.s^-1. */
	double	position[3];			/*!< Latitude, Longitude in degrees positive North and East.
										 Altitude above Mean Sea Level in meters. */
	float	undulation;				/*!< Altitude difference between the geoid and the Ellipsoid in meters (Height above Ellipsoid = altitude + undulation). */
	float	positionStdDev[3];		/*!< Latitude, longitude and altitude 1 sigma standard deviation in meters. */
	uint32	status;					/*!< EKF solution status bitmask and enum. */
} SbgLogEkfNavData;

//----------------------------------------------------------------------//
//- Operations                                                         -//
//----------------------------------------------------------------------//

/*!
 *	Parse data for the SBG_ECOM_LOG_EKF_EULER message and fill the corresponding structure.
 *	\param[in]	pPayload					Read only pointer on the payload buffer.
 *	\param[in]	payloadSize					Payload size in bytes.
 *	\param[out]	pOutputData					Pointer on the output structure that stores parsed data.
 *	\return									SBG_NO_ERROR if the payload has been parsed.
 */
SbgErrorCode sbgEComBinaryLogParseEkfEulerData(const void *pPayload, uint32 payloadSize, SbgLogEkfEulerData *pOutputData);

/*!
 *	Parse data for the SBG_ECOM_LOG_EKF_QUAT message and fill the corresponding structure.
 *	\param[in]	pPayload					Read only pointer on the payload buffer.
 *	\param[in]	payloadSize					Payload size in bytes.
 *	\param[out]	pOutputData					Pointer on the output structure that stores parsed data.
 *	\return									SBG_NO_ERROR if the payload has been parsed.
 */
SbgErrorCode sbgEComBinaryLogParseEkfQuatData(const void *pPayload, uint32 payloadSize, SbgLogEkfQuatData *pOutputData);

/*!
 *	Parse data for the SBG_ECOM_LOG_EKF_NAV message and fill the corresponding structure.
 *	\param[in]	pPayload					Read only pointer on the payload buffer.
 *	\param[in]	payloadSize					Payload size in bytes.
 *	\param[out]	pOutputData					Pointer on the output structure that stores parsed data.
 *	\return									SBG_NO_ERROR if the payload has been parsed.
 */
SbgErrorCode sbgEComBinaryLogParseEkfNavData(const void *pPayload, uint32 payloadSize, SbgLogEkfNavData *pOutputData);

#endif