################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mat/mat.c \
../src/mat/matCalGaussian_f32.c \
../src/mat/matCalGaussian_s32.c \
../src/mat/matCalGaussian_s64.c \
../src/mat/matCalHist.c \
../src/mat/matCalMedCover.c \
../src/mat/matCal_s32.c \
../src/mat/matCal_s64.c 

OBJS += \
./src/mat/mat.o \
./src/mat/matCalGaussian_f32.o \
./src/mat/matCalGaussian_s32.o \
./src/mat/matCalGaussian_s64.o \
./src/mat/matCalHist.o \
./src/mat/matCalMedCover.o \
./src/mat/matCal_s32.o \
./src/mat/matCal_s64.o 

C_DEPS += \
./src/mat/mat.d \
./src/mat/matCalGaussian_f32.d \
./src/mat/matCalGaussian_s32.d \
./src/mat/matCalGaussian_s64.d \
./src/mat/matCalHist.d \
./src/mat/matCalMedCover.d \
./src/mat/matCal_s32.d \
./src/mat/matCal_s64.d 


# Each subdirectory must supply rules for building sources it contributes
src/mat/%.o: ../src/mat/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v8 gcc compiler'
	aarch64-none-elf-gcc -Wall -O1 -g3 -c -fmessage-length=0 -MT"$@" -ID:/project/vitis_final/lwip_modi_3/export/lwip_modi_3/sw/lwip_modi_3/standalone_domain/bspinclude/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


