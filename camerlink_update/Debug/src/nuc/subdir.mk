################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/nuc/DeFPN.c \
../src/nuc/DeNU_2.c \
../src/nuc/DeNoise.c \
../src/nuc/DeShading.c \
../src/nuc/DeStrip.c \
../src/nuc/IRNUC.c \
../src/nuc/InitParam.c \
../src/nuc/SSN.c \
../src/nuc/file_op.c \
../src/nuc/main.c \
../src/nuc/shareOp.c \
../src/nuc/utils.c 

OBJS += \
./src/nuc/DeFPN.o \
./src/nuc/DeNU_2.o \
./src/nuc/DeNoise.o \
./src/nuc/DeShading.o \
./src/nuc/DeStrip.o \
./src/nuc/IRNUC.o \
./src/nuc/InitParam.o \
./src/nuc/SSN.o \
./src/nuc/file_op.o \
./src/nuc/main.o \
./src/nuc/shareOp.o \
./src/nuc/utils.o 

C_DEPS += \
./src/nuc/DeFPN.d \
./src/nuc/DeNU_2.d \
./src/nuc/DeNoise.d \
./src/nuc/DeShading.d \
./src/nuc/DeStrip.d \
./src/nuc/IRNUC.d \
./src/nuc/InitParam.d \
./src/nuc/SSN.d \
./src/nuc/file_op.d \
./src/nuc/main.d \
./src/nuc/shareOp.d \
./src/nuc/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/nuc/%.o: ../src/nuc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O2 -g3 -c -fmessage-length=0 -MT"$@" -ID:/project/vitis_final/lwip_modi_3/export/lwip_modi_3/sw/lwip_modi_3/standalone_on_psu_cortexa53_1/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


