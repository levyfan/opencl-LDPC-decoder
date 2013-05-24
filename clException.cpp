#include "clException.h"

clException::clException(cl_int error, const std::string& msg) : std::runtime_error(msg)
{
	this->error = error;
}

const char* clException::what() const throw()
{
	std::string msg(runtime_error::what());
	switch (error)
	{
		case CL_SUCCESS:                            return ("Success. "+msg).c_str();
		case CL_DEVICE_NOT_FOUND:                   return ("Device not found. "+msg).c_str();
		case CL_DEVICE_NOT_AVAILABLE:               return ("Device not available. "+msg).c_str();
		case CL_COMPILER_NOT_AVAILABLE:             return ("Compiler not available. "+msg).c_str();
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return ("Memory object allocation failure. "+msg).c_str();
		case CL_OUT_OF_RESOURCES:                   return ("Out of resources. "+msg).c_str();
		case CL_OUT_OF_HOST_MEMORY:                 return ("Out of host memory. "+msg).c_str();
		case CL_PROFILING_INFO_NOT_AVAILABLE:       return ("Profiling information not available. "+msg).c_str();
		case CL_MEM_COPY_OVERLAP:                   return ("Memory copy overlap. "+msg).c_str();
		case CL_IMAGE_FORMAT_MISMATCH:              return ("Image format mismatch. "+msg).c_str();
		case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return ("Image format not supported. "+msg).c_str();
		case CL_BUILD_PROGRAM_FAILURE:              return ("Program build failure. "+msg).c_str();
		case CL_MAP_FAILURE:                        return ("Map failure. "+msg).c_str();
		case CL_INVALID_VALUE:                      return ("Invalid value. "+msg).c_str();
		case CL_INVALID_DEVICE_TYPE:                return ("Invalid device type. "+msg).c_str();
		case CL_INVALID_PLATFORM:                   return ("Invalid platform. "+msg).c_str();
		case CL_INVALID_DEVICE:                     return ("Invalid device. "+msg).c_str();
		case CL_INVALID_CONTEXT:                    return ("Invalid context. "+msg).c_str();
		case CL_INVALID_QUEUE_PROPERTIES:           return ("Invalid queue properties. "+msg).c_str();
		case CL_INVALID_COMMAND_QUEUE:              return ("Invalid command queue. "+msg).c_str();
		case CL_INVALID_HOST_PTR:                   return ("Invalid host pointer. "+msg).c_str();
		case CL_INVALID_MEM_OBJECT:                 return ("Invalid memory object. "+msg).c_str();
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return ("Invalid image format descriptor. "+msg).c_str();
		case CL_INVALID_IMAGE_SIZE:                 return ("Invalid image size. "+msg).c_str();
		case CL_INVALID_SAMPLER:                    return ("Invalid sampler. "+msg).c_str();
		case CL_INVALID_BINARY:                     return ("Invalid binary. "+msg).c_str();
		case CL_INVALID_BUILD_OPTIONS:              return ("Invalid build options. "+msg).c_str();
		case CL_INVALID_PROGRAM:                    return ("Invalid program. "+msg).c_str();
		case CL_INVALID_PROGRAM_EXECUTABLE:         return ("Invalid program executable. "+msg).c_str();
		case CL_INVALID_KERNEL_NAME:                return ("Invalid kernel name. "+msg).c_str();
		case CL_INVALID_KERNEL_DEFINITION:          return ("Invalid kernel definition. "+msg).c_str();
		case CL_INVALID_KERNEL:                     return ("Invalid kernel. "+msg).c_str();
		case CL_INVALID_ARG_INDEX:                  return ("Invalid argument index. "+msg).c_str();
		case CL_INVALID_ARG_VALUE:                  return ("Invalid argument value. "+msg).c_str();
		case CL_INVALID_ARG_SIZE:                   return ("Invalid argument size. "+msg).c_str();
		case CL_INVALID_KERNEL_ARGS:                return ("Invalid kernel arguments. "+msg).c_str();
		case CL_INVALID_WORK_DIMENSION:             return ("Invalid work dimension. "+msg).c_str();
		case CL_INVALID_WORK_GROUP_SIZE:            return ("Invalid work group size. "+msg).c_str();
		case CL_INVALID_WORK_ITEM_SIZE:             return ("Invalid work item size. "+msg).c_str();
		case CL_INVALID_GLOBAL_OFFSET:              return ("Invalid global offset. "+msg).c_str();
		case CL_INVALID_EVENT_WAIT_LIST:            return ("Invalid event wait list. "+msg).c_str();
		case CL_INVALID_EVENT:                      return ("Invalid event. "+msg).c_str();
		case CL_INVALID_OPERATION:                  return ("Invalid operation. "+msg).c_str();
		case CL_INVALID_GL_OBJECT:                  return ("Invalid OpenGL object. "+msg).c_str();
		case CL_INVALID_BUFFER_SIZE:                return ("Invalid buffer size. "+msg).c_str();
		case CL_INVALID_MIP_LEVEL:                  return ("Invalid mip-map level. "+msg).c_str();
		default:                                    return ("Unknown. "+msg).c_str();
	}
}