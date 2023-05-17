# Example code in C to demonstrate selectivity outputs in BME688 with BSEC2 library
 * Example for using of BSEC library in a fixed configuration with the BME68x sensor.
 * This works by running an endless loop in the bsec_iot_loop() function.

# Hardware test details
 * Default sensor is number 0 and can be declared in SENS_NUM if you have more than one sensor on board.
 * The communication mode to the sensor is via SPI in this illustration.

# Algorithm inclusion
 * The static library path needs to be included in the platform.txt file.
 * In the platforms.txt search ‘recipe.c.combine.pattern’ and adding where your library is located after ‘{object_files}’, just like this:
   recipe.c.combine.pattern="{compiler.path}{compiler.c.elf.cmd}" "-Wl,--Map={build.path}/{build.project_name}.map" "-L{compiler.sdk.path}/lib" "-L{compiler.sdk.path}/ld" "-L{compiler.sdk.path}/{build.memory_type}" {compiler.c.elf.flags} {compiler.c.elf.extra_flags} {build.extra_flags} -Wl,--start-group {object_files} "C:\Users\Name\Documents\Projects\Bsec_Library\libalgobsec.a" "{archive_file_path}" {build.extra_libs} {compiler.c.elf.libs} {compiler.libraries.ldflags} -Wl,--end-group -Wl,-EL -o "{build.path}/{build.project_name}.elf"
 * Configurations can be copied to files Selectivity_Config.c & Selectivity_Config.h which is included by default
 * Note to not change the variable name as they are already linked in the repositrory.

# Code compilation
 * You have to bring all the necessary dependent files from src folder(bme68x, config and inc) in order to compile the example code.

# Stack Overflow
 * Few hardwares may have limited stack size by default and needs to be modified inorder to include larger algorithms
 * For arduino environment the stack size is declared in sdkconfig.h file under definition CONFIG_ARDUINO_LOOP_STACK_SIZE

