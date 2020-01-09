################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/LoadShaders.cpp \
../src/cCamera.cpp \
../src/cFrustum.cpp \
../src/cLineIntersector.cpp \
../src/cLines.cpp \
../src/cOpenGL_BaseApp.cpp \
../src/cProjectionManager.cpp \
../src/cQuad.cpp \
../src/framebufferObject.cpp \
../src/glx_x11_events.cpp \
../src/main.cpp 

OBJS += \
./src/LoadShaders.o \
./src/cCamera.o \
./src/cFrustum.o \
./src/cLineIntersector.o \
./src/cLines.o \
./src/cOpenGL_BaseApp.o \
./src/cProjectionManager.o \
./src/cQuad.o \
./src/framebufferObject.o \
./src/glx_x11_events.o \
./src/main.o 

CPP_DEPS += \
./src/LoadShaders.d \
./src/cCamera.d \
./src/cFrustum.d \
./src/cLineIntersector.d \
./src/cLines.d \
./src/cOpenGL_BaseApp.d \
./src/cProjectionManager.d \
./src/cQuad.d \
./src/framebufferObject.d \
./src/glx_x11_events.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/benjha/cuda-workspace/LineIntersection/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


