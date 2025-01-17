#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <codecvt>

#ifdef USLDATA_EXPORTS
#define USLDATA_API __declspec(dllexport)
#else
#define USLDATA_API __declspec(dllimport)
#endif

#define USL_RETURN_CODE_STARTS 0x200 /* Starting return code */

/* API Return Code Values */
typedef enum _USL_RETURN_CODE
{
    USLNoOpenFile = -1,
    USLSuccess = 0,
    USLFailed = USL_RETURN_CODE_STARTS,    //512
    USLAccessDenied,            //513
    USLNotRecognisedFile,       //514
    USLScanIndexError,          //515
    USLTimeSlotError,           //516
    USLBufferSizeError,         //517
    USLFileLengthError,         //518
    USLBufferPtrError,          //519
    USLInVariablesError,        //520
    USLUninitialisedVariable,   //521
    USLFailedBytes,             //522
    USLFailedFormat,            //523
    USLNoLicence,               //524
    USLNoDongle,                //525
    USLIncorrectDongle,         //526

} USL_RETURN_CODE;


/* Amplifier Type Classification */
typedef enum _USL_AMP_MODE
{
    AMP_RAW,
    AMP_GREY_SCALE,
    AMP_SIGNED,
    AM_UNSIGNED,
    AMP_PERCENTAGE,
    AMP_LINEAR_DB,
    AMP_LOG_DB,
    AMP_FLOAT,
} USL_AMP_MODE;

typedef enum _AMP_FORMAT
{
    UNSIGNED,
    SIGNED,
} AMP_FORMAT;

typedef enum _TIME_FORMAT
{
    UNSIGNED_NS,
    UNSIGNED_MICRO_S,
    METRIC,
    IMPERIAL,
} TIME_FORMAT;

typedef struct _USLFileVersion
{
    int nMajor;
    int nMinor;
    int nSubMinor;
    int nBuild;
} USLFileVersion;


/*******************************************************************************************************************************************************************************************************/
/*
Description:        Populates the USLFileVersion structure variable with the SDK version identifier.

Parameters:         pVersion - Pointer to structure variable that gets populated with SDK version information.

Return:             USLSuccess - Variable was correctly populated.
                    USLUninitialisedVariable - Variable was not initialised before method call.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetLibraryVersion(USLFileVersion* pVersion);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Populates the USLFileVersion structure variable with the USLScanner version of the open file.
                    Populates a w-string variable with the USLScanner version of the open file in the format xx.xx.0.0.

Parameters:         pVersion - Pointer to structure variable that gets populated with the file version.
                    strDataVersion - Address of string that will hold the file version.

Return:             USLSuccess - Variables were populated correctly.
                    USLUninitialisedVariable - Variables were not initialised before method call.

TODO CHECK HEADER INITIALISED, RETURN NON INITIALISED HEADER
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetDataVersion(USLFileVersion* pVersion, std::wstring& strDataVersion);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Opens USL .dat file and loads the information from the file into the SDK.

Parameters:         lpszFileName - String holding the file path for the file that is to be opened.
                    pFile - Pointer that receives pointer to the opened file.
                    p_errno_t - Non-zero if something wrong with filename.

Return:             USLSuccess - File opened successfully.
                    USLAccessDenied - Failed to open file, check p_errno_t.
                    USLNotRecognisedFile - Could not recognise file type as USL data file.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLDataOpen(std::wstring lpszFileName, FILE** pFile);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Closes USL .dat file and removes the information of the file from the SDK.

Parameters:         pFile - Pointer that points to the pointer of the file that is being closed.
                    p_nError - Negative if something went wrong during file closing.

Return:             USLSuccess - File closed successfully.
                    USLFailed - Failed to close file, check p_nError.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLDataClose(FILE** pFile, int* p_nError);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Loads the settings from file that is currently open.

Parameters:         pFile - Pointer of the file that holds the Amplitude array that is being retrieved.
                    pnSamples - Pointer that will be loaded with the number of samples in a line.
                    pnLines - Pointer that will be loaded with the number of lines in a c-scan.
                    pnBytesPerSample - Pointer that will be loaded with the number of bytes used for each sample.

Return:             USLSuccess - Function retrieved information successfully.
                    USLNoOpenFile - Function called while no file was open.
                    USLUninitialisedVariable - Variables were not initialised before method call.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetAmplitudeCScanSettings(FILE* pFile, int* pnSamples, int* pnLines, int* pnBytesPerSample);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Loads the amplitude data for a specific gate in a specific timeslot.

Parameters:         pFile - Pointer of the file that holds the Amplitude array that is being retrieved.
                    nTimeSlotIndex - Index for desired timeslot.
                    nScanIndex -  Index for desired c-scan of selected timeslot.
                    pBuffer - Pointer to array that will hold the amplitude array that is being retrieved.
                    nBufferSizeBytes - Size of array in bytes.
                    eAmpMode - Identifier for amplifier type, see definition of USL_AMP_MODE for more information.
                    nMaxValueDesired - Maximum amplitude value, 2^15 = 32768.

Return:             USLSuccess - Function filled buffer with data.
                    USLFailed - Function failed to fill buffer with data.
                    USLNoOpenFile - Function called while no file was open.
                    USLTimeSlotError - nTimeSlotIndex set outside of bounds, ie. <0, >256, more than number of timeslots in file.
                    USLScanIndexError - nScanIndex set outside of bounds, ie. <0, more than number of c-scans for that timeslot.
                    USLBufferPtrError - pBuffer not initialised.
                    USLFileLengthError - Something wrong with file size, c-scan gets located in out of bounds region.
                    USLBufferSizeError - nBufferSizeBytes not big enough for the amplitude c-scan.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetAmplitudeCScan(FILE* pFile, int nTimeSlotIndex, int nScanIndex, void* pBuffer, unsigned int nBufferSizeBytes, USL_AMP_MODE eAmpMode, int nMaxValueDesired = 32767);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Loads the amplitude wave for the current c-scan.

Parameters:         pFile - Pointer of the file that holds the Amplitude array that is being retrieved.
                    nTimeSlotIndex - Index for desired timeslot.
                    nScanIndex -  Index for desired c-scan of selected timeslot.
                    pnTrace - Pointer to array that will hold the amplitude array that is being retrieved.
                    nBufferSizeBytes - Size of array in bytes.
                    eAmpMode - Identifier for amplifier type, see definition of USL_AMP_MODE for more information.
                    nMaxValueDesired - Maximum amplitude value, 2^15 = 32768.

Return:             USLSuccess - Function filled buffer with data.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetAScan(FILE* pFile, int nSampleIndex, int nLineIndex, int* pnTrace);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Loads the Time Of Flight (ToF) data for a specific gate in a specific timeslot.

Parameters:         pFile - Pointer of the file that holds the ToF array that is being retrieved.
                    nTimeSlotIndex - Index for desired timeslot.
                    nScanIndex -  Index for desired c-scan of selected timeslot.
                    pnBuffer - Pointer to array that will hold the ToF array that is being retrieved.
                    nBufferSizeBytes - Size of array in bytes.

Return:             USLSuccess - Function filled buffer with data.
                    USLFailed - Function failed to fill buffer with data.
                    USLNoOpenFile - Function called while no file was open.
                    USLTimeSlotError - nTimeSlotIndex set outside of bounds, ie. <0, >256, more than number of timeslots in file.
                    USLScanIndexError - nScanIndex set outside of bounds, ie. <0, more than number of c-scans for that timeslot.
                    USLBufferPtrError - pBuffer not initialised.
                    USLFileLengthError - Something wrong with file size, c-scan gets located in out of bounds region.
                    USLBufferSizeError - nBufferSizeBytes not big enough for the amplitude c-scan.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetTOFCScan(FILE* pFile, int nTimeSlotIndex, int nScanIndex, int* pnBuffer, unsigned int nBufferSizeBytes);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the number of timeslots in the file.

Parameters:         pFile - Pointer of the file that holds the ToF array that is being retrieved.
                    nTimeSlotCount - Pointer to variable where the function stores the number of timeslots available in the file.

Return:             USLSuccess - Function loaded value from file into the variable.
                    USLNoOpenFile - Function called while no file was open.
                    USLFailed - Function failed to load value into the variable.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetTimeSlotCount(FILE* pFile, int* nTimeSlotCount);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns information on the number of Amplitude c-scans in a specific timeslot.

Parameters:         pFile - Pointer of the file that holds the ToF array that is being retrieved.
                    nTimeSlotIndex - Index for desired timeslot.
                    nAmpImageCount - Pointer to variable where the function stores the number of amplitude c-scans available for the selected timeslot.

Return:             USLSuccess - Function loaded value from file into the variable.
                    USLNoOpenFile - Function called while no file was open.
                    USLFailed - Function failed to load value into the variable.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetAmplitudeCScanCount(FILE* pFile, int nTimeSlotIndex, int* nAmpImageCount);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns information on the number of ToF c-scans in a specfic timeslot.

Parameters:         pFile - Pointer of the file that holds the ToF array that is being retrieved.
                    nTimeSlotIndex - Index for desired timeslot.
                    nTOFImageCount -  Pointer to variable where the function stores the number of ToF c-scans available for the selected timeslot.

Return:             USLSuccess - Function loaded value from file into the variable.
                    USLNoOpenFile - Function called while no file was open.
                    USLFailed - Function failed to load value into the variable.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetTOFCScanCount(FILE* pFile, int nTimeSlotIndex, int* nTOFImageCount);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns amplitude formatting information.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    pnBytes - Pointer to variable where the function stores the number of bytes per c-scan point value.
                    pFormat - Pointer to variable where the function stores the format of the c-scan point value.

Return:             USLSuccess - Function loaded values into variables.
                    USLNoOpenFile - Function called while no file was open.
                    USLFailedBytes - Function failed to load value into pnBytes.
                    USLFailedFormat - Function failed to load value into pFormat.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetCScanAmplitudeFormat(FILE* pFile, int* pnBytes, AMP_FORMAT* pFormat);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the number of samples per line.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    nSamplesPerLine - Pointer to variable where the function stores the number of samples per line.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetCScanSamplesPerLine(FILE* pFile, int* nSamplesPerLine);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the number of lines in the c-scan.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    nNumberOfLines - Pointer to variable where the function stores the number of lines in a c-scan.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetCScanNumberOfLines(FILE* pFile, int* nNumberOfLines);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the number of bytes used per point in the c-scan.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    nAmpBytesPerSample - Pointer to variable where the function stores the number of bytes used to store the information in a value.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetCScanBytesPerSample(FILE* pFile, int* nAmpBytesPerSample);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the number of values stated as being stored as part of each a-scan.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    nAscanLength - Pointer to variable where the function stores the number of values in an a-scan as per the information in the file.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetAScanLengthInFile(FILE* pFile, int* nAscanLength);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the actual number of values stored as part of each a-scan.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    nRealAscanLength - Pointer to variable where the function stores the real number of values in an a-scan.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetAScanLength(FILE* pFile, int* nRealAscanLength);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the number of bytes used per sample in the a-scan.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    nAScanBytesPerSample - Pointer to variable where the function stores the number of bytes in an a-scan as per the information in the file.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetAScanBytesPerSample(FILE* pFile, int* nAScanBytesPerSample);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the value for the fast increment setting.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    fFastIncrement - Pointer to variable where the function stores the fast increment.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetCScanHorizontalSampleSize(FILE* pFile, float* fFastIncrement);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the value for the slow increment setting.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    fSlowIncrement - Pointer to variable where the function stores the slow increment.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetCScanVerticalSampleSize(FILE* pFile, float* fSlowIncrement);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns the sampling period for the a-scan data stored in the file.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    fAScanSamplePeriod - Pointer to variable where the function stores the sample period for the a-scan values.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLgetAScanSamplePeriod(FILE* pFile, float* fAScanSamplePeriod);

/*******************************************************************************************************************************************************************************************************/
/*
Description:        Function returns whether there are a-scans in the file.

Parameters:         pFile - Pointer of the file that holds the amplitude c-scans.
                    bAScanAvailable - Pointer to variable where the function stores whether there are a-scans available in the file.

Return:             USLSuccess - Function loaded value into the variable.
                    USLNoOpenFile - Function called while no file was open.
*/

extern "C" USLDATA_API USL_RETURN_CODE USLIsAScanAvailable(FILE* pFile, bool* bAScanAvailable);
