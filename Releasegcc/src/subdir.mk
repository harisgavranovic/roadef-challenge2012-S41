################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/IO.cpp \
../src/data.cpp \
../src/machine.cpp \
../src/machine_resource.cpp \
../src/process.cpp \
../src/search.cpp \
../src/service.cpp \
../src/solution.cpp \
../src/solverGoogle.cpp \
../src/testovi.cpp 

OBJS += \
./src/IO.o \
./src/data.o \
./src/machine.o \
./src/machine_resource.o \
./src/process.o \
./src/search.o \
./src/service.o \
./src/solution.o \
./src/solverGoogle.o \
./src/testovi.o 

CPP_DEPS += \
./src/IO.d \
./src/data.d \
./src/machine.d \
./src/machine_resource.d \
./src/process.d \
./src/search.d \
./src/service.d \
./src/solution.d \
./src/solverGoogle.d \
./src/testovi.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -march=native -mtune=native -ffast-math -ftree-vectorize -fpermissive -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


