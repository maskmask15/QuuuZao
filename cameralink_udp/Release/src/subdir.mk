################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/debug.c \
../src/display_demo.c \
../src/echo.c \
../src/lwip_app.c \
../src/platform_zynq.c \
../src/platform_zynqmp.c \
../src/udp_init.c \
../src/vdma.c \
../src/zynq_interrupt.c 

OBJS += \
./src/debug.o \
./src/display_demo.o \
./src/echo.o \
./src/lwip_app.o \
./src/platform_zynq.o \
./src/platform_zynqmp.o \
./src/udp_init.o \
./src/vdma.o \
./src/zynq_interrupt.o 

C_DEPS += \
./src/debug.d \
./src/display_demo.d \
./src/echo.d \
./src/lwip_app.d \
./src/platform_zynq.d \
./src/platform_zynqmp.d \
./src/udp_init.d \
./src/vdma.d \
./src/zynq_interrupt.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O2 -c -fmessage-length=0 -MT"$@" -ID:/project/vitis_final/lwip_modi_3/export/lwip_modi_3/sw/lwip_modi_3/standalone_domain/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


