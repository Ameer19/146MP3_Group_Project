################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../L5_Application/LabSPI.cpp \
../L5_Application/MP3DecoderDriver.cpp \
../L5_Application/MP3Driver.cpp \
../L5_Application/main.cpp 

OBJS += \
./L5_Application/LabSPI.o \
./L5_Application/MP3DecoderDriver.o \
./L5_Application/MP3Driver.o \
./L5_Application/main.o 

CPP_DEPS += \
./L5_Application/LabSPI.d \
./L5_Application/MP3DecoderDriver.d \
./L5_Application/MP3Driver.d \
./L5_Application/main.d 


# Each subdirectory must supply rules for building sources it contributes
L5_Application/%.o: ../L5_Application/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -Os -fmessage-length=0 -ffunction-sections -fdata-sections -Wall -Wshadow -Wlogical-op -Wfloat-equal -DBUILD_CFG_MPU=0 -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\newlib" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L0_LowLevel" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L1_FreeRTOS" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L1_FreeRTOS\include" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L1_FreeRTOS\portable" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L1_FreeRTOS\portable\no_mpu" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L2_Drivers" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L2_Drivers\base" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L3_Utils" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L3_Utils\tlm" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L4_IO" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L4_IO\fat" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L4_IO\wireless" -I"C:\sjsudev_v2\projects\lpc1758_helloworld_MP3Project\L5_Application" -std=gnu++11 -fabi-version=0 -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


