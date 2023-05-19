///
/// @file hV_Constants.h
/// @brief Constants for Pervasive Displays Library Suite
///
/// @details Project Pervasive Displays Library Suite
/// @n Based on highView technology
///
/// @author Rei Vilo
/// @date 02 May 2023
/// @version 609
///
/// @copyright (c) Rei Vilo, 2010-2023
/// @copyright All rights reserved
///
/// * Basic edition: for hobbyists and for basic usage
/// @n Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
///
/// * Evaluation edition: for professionals or organisations, evaluation only, no commercial usage
/// @n All rights reserved
///
/// * Commercial edition: for professionals or organisations, commercial usage
/// @n All rights reserved
///

// SDK
#include "stdint.h"

#ifndef hV_CONSTANTS_RELEASE
///
/// @brief Release
///
#define hV_CONSTANTS_RELEASE 609
#endif // hV_CONSTANTS_RELEASE

///
/// @name 1- List of supported Pervasive Displays screens
/// @
/// @note Legacy screens are listed in the corresponding screen libraries
/// @{
///
/// * General parameters
///
/// @brief Specific features
/// @note Numbers are bit-based and or-combinable
/// @{
#define FEATURE_FAST 0x01 ///< With embedded fast update
#define FEATURE_TOUCH 0x02 ///< With capacitive touch panel
#define FEATURE_OTHER 0x04 ///< With other feature
#define FEATURE_WIDE_TEMPERATURE 0x08 ///< With wide operating temperature
#define FEATURE_RED 0x10 ///< With red colour
/// @}

///
/// @brief Update mode
/// @note Numbers are sequential and exclusive
/// @{
#define UPDATE_NONE 0x00 ///< No update
#define UPDATE_GLOBAL 0x01 ///< Global update, default
#define UPDATE_FAST 0x02 ///< Fast update
#define UPDATE_PARTIAL 0x03 ///< Partial update
/// @}

///
/// @brief Large screens sub-panels selection
/// @note Numbers are sequential and exclusive 
/// @{
#define PANEL_CS_MAIN 0x01 ///< Large screens sub-panels: first panel only
#define PANEL_CS_SECOND 0x02 ///< Large screens sub-panels: second panel only
#define PANEL_CS_BOTH 0x03 ///< Large screens sub-panels: both panels
/// @}

///
/// @brief Power state
/// @note Numbers are sequential and exclusive 
/// @{
#define FSM_POWER_UNKNOWN 0x00 ///< Unknown
#define FSM_POWER_ON 0x01 ///< On
#define FSM_POWER_SLEEP 0x02 ///< Off or sleep
/// @}

///
/// @brief Partial update state
/// @note Numbers are sequential and exclusive 
/// @{
#define PARTIAL_OFF 0x00 ///< Non-activated
#define PARTIAL_ON 0x01 ///< Activated but not populated
#define PARTIAL_READY 0x02 ///< Activated and populated
/// @}

///
/// @brief Continuity mode state
/// @note Numbers are sequential and exclusive 
/// @{
#define CONTINUITY_OFF 0x00 ///< Non-activated
#define CONTINUITY_ON 0x01 ///< Activated but not initialised
#define CONTINUITY_READY 0x02 ///< Activated and initialised
/// @}

///
/// @brief Touch events
/// @note Numbers are sequential and exclusive 
/// @{
#define TOUCH_EVENT_NONE 0 ///< No touch event
#define TOUCH_EVENT_PRESS 1 ///< Touch press event
#define TOUCH_EVENT_RELEASE 2 ///< Touch release event
#define TOUCH_EVENT_MOVE 3 ///< Touch move event
/// @}
///

///
/// @brief Results
/// @note Numbers are sequential and exclusive 
/// @{
#define RESULT_SUCCESS 0 ///< Success
#define RESULT_ERROR 1 ///< Error
/// @}
///

///
/// @brief Orientation constants
/// @{
///
#define ORIENTATION_PORTRAIT 6 ///< Portrait or vertical, higher than large
#define ORIENTATION_LANDSCAPE 7 ///< Landscape or horizontal, larger than high
/// @}
///
