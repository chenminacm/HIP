/*
Copyright (c) 2015-2016 Advanced Micro Devices, Inc. All rights reserved.
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANNTY OF ANY KIND, EXPRESS OR
IMPLIED, INNCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANNY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER INN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR INN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "hip_runtime.h"
#include "hcc_detail/hip_hcc.h"
#include "hcc_detail/trace_helper.h"

//-------------------------------------------------------------------------------------------------
//Devices
//-------------------------------------------------------------------------------------------------
//---
/**
 * @return  #hipSuccess
 */
hipError_t hipGetDevice(int *device)
{
    HIP_INIT_API(device);

    *device = tls_defaultDevice;
    return ihipLogStatus(hipSuccess);
}


//---
/**
 * @return  #hipSuccess, #hipErrorNoDevice
 */
hipError_t hipGetDeviceCount(int *count)
{
    HIP_INIT_API(count);

    *count = g_deviceCnt;

    if (*count > 0) {
        return ihipLogStatus(hipSuccess);
    } else {
        return ihipLogStatus(hipErrorNoDevice);
    }
}


//---
/**
 * @returns #hipSuccess
 */
hipError_t hipDeviceSetCacheConfig ( hipFuncCache cacheConfig )
{
    HIP_INIT_API(cacheConfig);

    // Nop, AMD does not support variable cache configs.

    return ihipLogStatus(hipSuccess);
}


//---
/**
 * @returns #hipSuccess
 */
hipError_t hipDeviceGetCacheConfig ( hipFuncCache *cacheConfig )
{
    HIP_INIT_API(cacheConfig);

    *cacheConfig = hipFuncCachePreferNone;

    return ihipLogStatus(hipSuccess);
}


//---
/**
 * @returns #hipSuccess
 */
hipError_t hipFuncSetCacheConfig ( hipFuncCache cacheConfig )
{
    HIP_INIT_API(cacheConfig);

    // Nop, AMD does not support variable cache configs.

    return ihipLogStatus(hipSuccess);
}



//---
/**
 * @returns #hipSuccess
 */
hipError_t hipDeviceSetSharedMemConfig ( hipSharedMemConfig config )
{
    HIP_INIT_API(config);

    // Nop, AMD does not support variable shared mem configs.

    return ihipLogStatus(hipSuccess);
}



//---
/**
 * @returns #hipSuccess
 */
hipError_t hipDeviceGetSharedMemConfig ( hipSharedMemConfig * pConfig )
{
    HIP_INIT_API(pConfig);

    *pConfig = hipSharedMemBankSizeFourByte;

    return ihipLogStatus(hipSuccess);
}

//---
/**
 * @return #hipSuccess, #hipErrorInvalidDevice
 */
hipError_t hipSetDevice(int device)
{
    HIP_INIT_API(device);
    if ((device < 0) || (device >= g_deviceCnt)) {
        return ihipLogStatus(hipErrorInvalidDevice);
    } else {
        tls_defaultDevice = device;
        return ihipLogStatus(hipSuccess);
    }
}


//---
/**
 * @return #hipSuccess
 */
hipError_t hipDeviceSynchronize(void)
{
    HIP_INIT_API();

    ihipGetTlsDefaultDevice()->locked_waitAllStreams(); // ignores non-blocking streams, this waits for all activity to finish.

    return ihipLogStatus(hipSuccess);
}


//---
/**
 * @return @ref hipSuccess
 */
hipError_t hipDeviceReset(void)
{
    HIP_INIT_API();

    ihipDevice_t *device = ihipGetTlsDefaultDevice();

    // TODO-HCC
    // This function currently does a user-level cleanup of known resources.
    // It could benefit from KFD support to perform a more "nuclear" clean that would include any associated kernel resources and page table entries.


    if (device) {
        // Release device resources (streams and memory):
        device->locked_reset(); 
    }

    return ihipLogStatus(hipSuccess);
}

/**
 *
 */
hipError_t hipDeviceGetAttribute(int* pi, hipDeviceAttribute_t attr, int device)
{
    HIP_INIT_API(attr, device);

    hipError_t e = hipSuccess;

    ihipDevice_t * hipDevice = ihipGetDevice(device);
    hipDeviceProp_t *prop = &hipDevice->_props;
    if (hipDevice) {
        switch (attr) {
        case hipDeviceAttributeMaxThreadsPerBlock:
            *pi = prop->maxThreadsPerBlock; break;
        case hipDeviceAttributeMaxBlockDimX:
            *pi = prop->maxThreadsDim[0]; break;
        case hipDeviceAttributeMaxBlockDimY:
            *pi = prop->maxThreadsDim[1]; break;
        case hipDeviceAttributeMaxBlockDimZ:
            *pi = prop->maxThreadsDim[2]; break;
        case hipDeviceAttributeMaxGridDimX:
            *pi = prop->maxGridSize[0]; break;
        case hipDeviceAttributeMaxGridDimY:
            *pi = prop->maxGridSize[1]; break;
        case hipDeviceAttributeMaxGridDimZ:
            *pi = prop->maxGridSize[2]; break;
        case hipDeviceAttributeMaxSharedMemoryPerBlock:
            *pi = prop->sharedMemPerBlock; break;
        case hipDeviceAttributeTotalConstantMemory:
            *pi = prop->totalConstMem; break;
        case hipDeviceAttributeWarpSize:
            *pi = prop->warpSize; break;
        case hipDeviceAttributeMaxRegistersPerBlock:
            *pi = prop->regsPerBlock; break;
        case hipDeviceAttributeClockRate:
            *pi = prop->clockRate; break;
        case hipDeviceAttributeMemoryClockRate:
            *pi = prop->memoryClockRate; break;
        case hipDeviceAttributeMemoryBusWidth:
            *pi = prop->memoryBusWidth; break;
        case hipDeviceAttributeMultiprocessorCount:
            *pi = prop->multiProcessorCount; break;
        case hipDeviceAttributeComputeMode:
            *pi = prop->computeMode; break;
        case hipDeviceAttributeL2CacheSize:
            *pi = prop->l2CacheSize; break;
        case hipDeviceAttributeMaxThreadsPerMultiProcessor:
            *pi = prop->maxThreadsPerMultiProcessor; break;
        case hipDeviceAttributeComputeCapabilityMajor:
            *pi = prop->major; break;
        case hipDeviceAttributeComputeCapabilityMinor:
            *pi = prop->minor; break;
        case hipDeviceAttributePciBusId:
            *pi = prop->pciBusID; break;
        case hipDeviceAttributeConcurrentKernels:
            *pi = prop->concurrentKernels; break;
        case hipDeviceAttributePciDeviceId:
            *pi = prop->pciDeviceID; break;
        case hipDeviceAttributeMaxSharedMemoryPerMultiprocessor:
            *pi = prop->maxSharedMemoryPerMultiProcessor; break;
        case hipDeviceAttributeIsMultiGpuBoard:
            *pi = prop->isMultiGpuBoard; break;
        default:
            e = hipErrorInvalidValue; break;
        }
    } else {
        e = hipErrorInvalidDevice;
    }
    return ihipLogStatus(e);
}


/**
 * @return #hipSuccess, #hipErrorInvalidDevice
 * @bug HCC always returns 0 for maxThreadsPerMultiProcessor
 * @bug HCC always returns 0 for regsPerBlock
 * @bug HCC always returns 0 for l2CacheSize
 */
hipError_t hipGetDeviceProperties(hipDeviceProp_t* props, int device)
{
    HIP_INIT_API(props, device);

    hipError_t e;

    ihipDevice_t * hipDevice = ihipGetDevice(device);
    if (hipDevice) {
        // copy saved props
        *props = hipDevice->_props;
        e = hipSuccess;
    } else {
        e = hipErrorInvalidDevice;
    }

    return ihipLogStatus(e);
}


hipError_t hipSetDeviceFlags( unsigned int flags)
{
    HIP_INIT_API(flags);

    hipError_t e;

    ihipDevice_t * hipDevice = ihipGetDevice(tls_defaultDevice);
    if(hipDevice){
       hipDevice->_device_flags = hipDevice->_device_flags | flags;
       e = hipSuccess;
    }else{
       e = hipErrorInvalidDevice;
    }

    return ihipLogStatus(e);
}


