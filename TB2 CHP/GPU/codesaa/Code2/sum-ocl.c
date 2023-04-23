#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "reduction.cl"

#define NUM_KERNELS 2

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

double wtime(void);
int output_device_info(cl_device_id device_id);

double wtime(void)
{
    /* Use a generic timer */
    static int sec = -1;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (sec < 0) sec = tv.tv_sec;
    return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

int output_device_info(cl_device_id device_id)
{
    int err;                            // error code returned from OpenCL calls
    cl_device_type device_type;         // Parameter defining the type of the compute device
    cl_uint comp_units;                 // the max number of compute units on a device
    cl_char vendor_name[1024] = {0};    // string to hold vendor name for compute device
    cl_char device_name[1024] = {0};    // string to hold name of compute device
#ifdef VERBOSE
    cl_uint          max_work_itm_dims;
    size_t           max_wrkgrp_size;
    size_t          *max_loc_size;
#endif
    
    
    err = clGetDeviceInfo(device_id, CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to access device name!\n");
        return EXIT_FAILURE;
    }
    printf(" \n Device is  %s ",device_name);
    
    err = clGetDeviceInfo(device_id, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to access device type information!\n");
        return EXIT_FAILURE;
    }
    if(device_type  == CL_DEVICE_TYPE_GPU)
        printf(" GPU from ");
    
    else if (device_type == CL_DEVICE_TYPE_CPU)
        printf("\n CPU from ");
    
    else
        printf("\n non  CPU or GPU processor from ");
    
    
    err = clGetDeviceInfo(device_id, CL_DEVICE_VENDOR, sizeof(vendor_name), &vendor_name, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to access device vendor name!\n");
        return EXIT_FAILURE;
    }
    printf(" %s ",vendor_name);
    
    err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &comp_units, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to access device number of compute units !\n");
        return EXIT_FAILURE;
    }
    printf(" with a max of %d compute units \n",comp_units);
    
#ifdef VERBOSE
    //
    // Optionally print information about work group sizes
    //
    err = clGetDeviceInfo( device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint),
                          &max_work_itm_dims, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to get device Info (CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS)!\n",
               err_code(err));
        return EXIT_FAILURE;
    }
    
    max_loc_size = (size_t*)malloc(max_work_itm_dims * sizeof(size_t));
    if(max_loc_size == NULL){
        printf(" malloc failed\n");
        return EXIT_FAILURE;
    }
    err = clGetDeviceInfo( device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, max_work_itm_dims* sizeof(size_t),
                          max_loc_size, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to get device Info (CL_DEVICE_MAX_WORK_ITEM_SIZES)!\n",err_code(err));
        return EXIT_FAILURE;
    }
    err = clGetDeviceInfo( device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t),
                          &max_wrkgrp_size, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to get device Info (CL_DEVICE_MAX_WORK_GROUP_SIZE)!\n",err_code(err));
        return EXIT_FAILURE;
    }
    printf("work group, work item information");
    printf("\n max loc dim ");
    for(int i=0; i< max_work_itm_dims; i++)
        printf(" %d ",(int)(*(max_loc_size+i)));
    printf("\n");
    printf(" Max work group size = %d\n",(int)max_wrkgrp_size);
#endif
    
    return CL_SUCCESS;
    
}

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

   cl_device_id device_id = NULL;
   int err;

   cl_uint nPlats;
   err  = clGetPlatformIDs( 0, NULL, &nPlats);
   cl_platform_id *platforms = (cl_platform_id *)malloc(sizeof(cl_platform_id) * nPlats);
   err  = clGetPlatformIDs( 1, platforms, &nPlats);
   printf("Num platforms IDs = %d\n",nPlats);

   /* Identify a platform */
    // Connect to a GPU (gpu=1) or a CPU (gpu=0)
    int gpu = 1;
    err = clGetDeviceIDs(platforms[0], gpu ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &device_id, NULL);
	free(platforms); platforms = NULL;

   // print context
   err = output_device_info(device_id);
    
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return device_id;
}

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

int main(int argc, char **argv) {
   int ARRAY_SIZE=1048576;

   if ( argc > 1 )
   {
      ARRAY_SIZE=atoi(argv[1]);
   }
   printf("ARRAY_SIZE=%d\n",ARRAY_SIZE);

   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel kernel[NUM_KERNELS];
   cl_command_queue queue;
   cl_event prof_event;
   cl_int i, j, err;
   size_t local_size, global_size;
   char kernel_names[NUM_KERNELS][20] = 
         {"reduction_scalar"};

   /* Data and buffers */
   float *data;
   data =(float *)malloc(ARRAY_SIZE*sizeof(float));
   float sum, actual_sum, *scalar_sum, *vector_sum;
   cl_mem data_buffer, scalar_sum_buffer;
   cl_int num_groups;
   cl_ulong time_start, time_end, total_time;

   /* Initialize data */
   for(i=0; i<ARRAY_SIZE; i++) {
      data[i] = 1.0f*i;
   }

   /* Create device and determine local size */
   device = create_device();
   err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, 	
         sizeof(local_size), &local_size, NULL);	
   printf("....local size = %d\n",local_size);
   if(err < 0) {
      perror("Couldn't obtain device information");
      exit(1);   
   }

   /* Allocate and initialize output arrays */
   num_groups = ARRAY_SIZE/local_size;
   scalar_sum = (float*) malloc(num_groups * sizeof(float));
   for(i=0; i<num_groups; i++) 
   {
      scalar_sum[i] = 0.0f;
   }
   
   /* Create a context */
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

   /* Build program */
   program = build_program(context, device, PROGRAM_FILE);

   /* Create data buffer */
   data_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY |
         CL_MEM_COPY_HOST_PTR, ARRAY_SIZE * sizeof(float), data, &err);
   scalar_sum_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_COPY_HOST_PTR, num_groups * sizeof(float), scalar_sum, &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   /* Create a command queue */
   queue = clCreateCommandQueue(context, device, 
         CL_QUEUE_PROFILING_ENABLE, &err);
   if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
   };

   for(i=0; i<1; i++) {

      /* Create a kernel */
      kernel[i] = clCreateKernel(program, kernel_names[i], &err);
      if(err < 0) {
         perror("Couldn't create a kernel");
         exit(1);
      };

      /* Create kernel arguments */
      err = clSetKernelArg(kernel[i], 0, sizeof(cl_mem), &data_buffer);
      if(i == 0) {
         global_size = ARRAY_SIZE;
         err |= clSetKernelArg(kernel[i], 1, local_size * sizeof(float), NULL);
         err |= clSetKernelArg(kernel[i], 2, sizeof(cl_mem), &scalar_sum_buffer);
      }
      if(err < 0) {
         perror("Couldn't create a kernel argument");
         exit(1);   
      }
       
      /* Enqueue kernel */
      err = clEnqueueNDRangeKernel(queue, kernel[i], 1, NULL, &global_size, 
            &local_size, 0, NULL, &prof_event); 
      if(err < 0) {
         perror("Couldn't enqueue the kernel");
         exit(1);   
      }

      /* Finish processing the queue and get profiling information */
      clFinish(queue);
      clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START,
            sizeof(time_start), &time_start, NULL);
      clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END,
            sizeof(time_end), &time_end, NULL);
      total_time = time_end - time_start;

      printf("Total time = %lu\n\n", total_time);
      printf("Total time = %le\n\n", total_time*1.0e-9);
      printf("Array Size = %ld\n", ARRAY_SIZE);
      
      FILE *file;
      file=fopen("reduction-ocl.txt","a+");
      fprintf(file,"%d %le\n",ARRAY_SIZE,total_time*1.0e-9);
      fclose(file);

      /* Deallocate event */
      clReleaseEvent(prof_event);
   }

   /* Deallocate resources */
   free(scalar_sum);
   clReleaseKernel(kernel[0]);
   clReleaseMemObject(scalar_sum_buffer);
   clReleaseMemObject(data_buffer);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
   return 0;
}
