################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/xdm/dma.c 

OBJS += \
./src/xdm/dma.o 

C_DEPS += \
./src/xdm/dma.d 


# Each subdirectory must supply rules for building sources it contributes
src/xdm/%.o: ../src/xdm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O1 -g3 -c -fmessage-length=0 -MT"$@" -ID:/project/vitis_final/lwip_modi_3/export/lwip_modi_3/sw/lwip_modi_3/standalone_domain/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


