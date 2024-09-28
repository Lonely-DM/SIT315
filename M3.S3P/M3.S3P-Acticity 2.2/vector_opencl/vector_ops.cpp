#include <stdio.h>
#include <stdlib.h>

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#define PRINT 1

int SZ = 8;
int *v;

// OpenCL device to execute the kernel on (e.g., CPU or GPU)
cl_mem bufV;

// OpenCL memory object for storing the vector data on the device
cl_device_id device_id;
// OpenCL memory object for storing the vector data on the device
cl_context context;
// OpenCL memory object for storing the vector data on the device
cl_program program;
// OpenCL memory object for storing the vector data on the device
cl_kernel kernel;
// OpenCL memory object for storing the vector data on the device
cl_command_queue queue;
cl_event event = NULL;

int err;

// Creates and returns an OpenCL device for kernel execution
cl_device_id create_device();
// Creates and returns an OpenCL device for kernel execution
void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname);
// Creates and returns an OpenCL device for kernel execution
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);
// Creates and returns an OpenCL device for kernel execution
void setup_kernel_memory();
// Creates and returns an OpenCL device for kernel execution
void copy_kernel_args();
// Creates and returns an OpenCL device for kernel execution
void free_memory();

void init(int *&A, int size);
void print(int *A, int size);

int main(int argc, char **argv)
{
    if (argc > 1)
        SZ = atoi(argv[1]);

    init(v, SZ);


    // OpenCL memory object for storing the vector data on the device
    size_t global[1] = {(size_t)SZ};

    //initial vector
    print(v, SZ);

    setup_openCL_device_context_queue_kernel((char *)"./vector_ops.cl", (char *)"square_magnitude");

    setup_kernel_memory();
    copy_kernel_args();

    // clEnqueueNDRangeKernel schedules the execution of a kernel on the OpenCL device. 
    // It takes the command queue, kernel object, number of dimensions, global and local work sizes, and synchronization events as arguments.
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, NULL, 0, NULL, &event);
    clWaitForEvents(1, &event);

    // clEnqueueReadBuffer reads the results from the device memory (bufV) back to the host (v). 
    // Arguments: command queue, buffer object, blocking mode, offset, size of data, pointer to the host memory, event wait list, and event.
    clEnqueueReadBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);

    //result vector
    print(v, SZ);

    //frees memory for device, kernel, queue, etc.
    //you will need to modify this to free your own buffers
    free_memory();
}

void init(int *&A, int size)
{
    A = (int *)malloc(sizeof(int) * size);

    for (long i = 0; i < size; i++)
    {
        A[i] = rand() % 100; // any number less than 100
    }
}

void print(int *A, int size)
{
    if (PRINT == 0)
    {
        return;
    }

    if (PRINT == 1 && size > 15)
    {
        for (long i = 0; i < 5; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
        printf(" ..... ");
        for (long i = size - 5; i < size; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
    }
    else
    {
        for (long i = 0; i < size; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
    }
    printf("\n----------------------------\n");
}

void free_memory()
{
    //free the buffers
    clReleaseMemObject(bufV);

    //free opencl objects
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    free(v);
}


void copy_kernel_args()
{
    // clSetKernelArg sets the arguments for the kernel. 
    // Arguments: kernel object, argument index, size of the argument, and pointer to the argument value.
    clSetKernelArg(kernel, 0, sizeof(int), (void *)&SZ);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufV);

    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

void setup_kernel_memory()
{
    // clCreateBuffer creates a memory buffer object on the device. 
    // Arguments: context, memory flags, size of buffer, host pointer (optional), and error code.
    // The second parameter, cl_mem_flags, defines the usage of the buffer (e.g., read-write or read-only).
    bufV = clCreateBuffer(context, CL_MEM_READ_WRITE, SZ * sizeof(int), NULL, NULL);

    // Copy matrices to the GPU
    clEnqueueWriteBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);
}

void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname)
{
    device_id = create_device();
    cl_int err;

    // clCreateContext creates an OpenCL context, which manages devices and resources.
    // Arguments: properties, number of devices, list of devices, error callback, user data, and error code.
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    program = build_program(context, device_id, filename);

    // clCreateCommandQueueWithProperties creates a command queue for the device.
    // Arguments: context, device, queue properties, and error code.
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err < 0)
    {
        perror("Couldn't create a command queue");
        exit(1);
    };


    kernel = clCreateKernel(program, kernelname, &err);
    if (err < 0)
    {
        perror("Couldn't create a kernel");
        printf("error =%d", err);
        exit(1);
    };
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{

    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;

    /* Read program file and place content into buffer */
    program_handle = fopen(filename, "r");
    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    // clCreateProgramWithSource creates an OpenCL program object from source code. It takes the context, number of strings, an array of source code strings, and their lengths as arguments.
    program = clCreateProgramWithSource(ctx, 1,
                                        (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    /* Build program 

   The fourth parameter accepts options that configure the compilation. 
   These are similar to the flags used by gcc. For example, you can 
   define a macro with the option -DMACRO=VALUE and turn off optimization 
   with -cl-opt-disable.
   */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0)
    {

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              0, NULL, &log_size);
        program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    return program;
}

cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   // Access a device
   // GPU
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // CPU
      printf("GPU not found\n");
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}