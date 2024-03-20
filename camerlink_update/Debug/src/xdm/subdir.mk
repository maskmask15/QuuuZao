################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/xdm/dma.c \
../src/xdm/dma_DeNU_2.c 

OBJS += \
./src/xdm/dma.o \
./src/xdm/dma_DeNU_2.o 

C_DEPS += \
./src/xdm/dma.d \
./src/xdm/dma_DeNU_2.d 


# Each subdirectory must supply rules for building sources it contributes
src/xdm/%.o: ../src/xdm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O2 -g3 -c -fmessage-length=0 -MT"$@" -ID:/project/vitis_final/lwip_modi_3/export/lwip_modi_3/sw/lwip_modi_3/standalone_on_psu_cortexa53_1/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


