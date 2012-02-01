############################################################################
#                                                                          #
#   This program is free software; you can redistribute it and/or modify   #
#   it under the terms of the GNU General Public License as published by   #
#   the Free Software Foundation; either version 2 of the License, or      #
#   (at your option) any later version.                                    #
#                                                                          #
#   This program is distributed in the hope that it will be useful,        #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of         #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
#   GNU General Public License for more details.                           #
#                                                                          #
#   You should have received a copy of the GNU General Public License      #
#   along with this program; if not, write to the Free Software            #
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111 USA    #
#                                                                          #
############################################################################
#                                                                          #
#                (c) Copyright, 2011, Elijah Brown                         #
#                                                                          #
############################################################################
#                                                                          #
#  Filename:     makefile                                                  #
#                                                                          #
############################################################################


##########################################################################
# Start of default section
#

TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CPP  = $(TRGT)g++
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
SIZE = $(TRGT)size -B -x
HEX  = $(CP) -O ihex 
BIN  = $(CP) -O binary 

MCU  = arm7tdmi

# List all default C defines here, like -D_DEBUG=1
DDEFS = 

# List all default ASM defines here, like -D_DEBUG=1
DADEFS = 

# List all default directories to look for include files here
DINCDIR = 

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS = -lm

#
# End of default section
##########################################################################

##########################################################################
# Start of user section
#

# Define project name here
PROJECT = APRSBeacon

# Define linker script file here
LDSCRIPT = lpc2148-flash.ld

# List all user C define here, like -D_DEBUG=1
UDEFS = 

# Define ASM defines here
UADEFS = 

# List C source files here
CSRC  = CRunTime.c 
CSRC += fatfs/ff.c fatfs/mmc.c

# List CPP source files here
CPPSRC = AFSK.cpp main.cpp Engineering.cpp APRSBeacon.cpp IOPorts.cpp Log.cpp SDLogger.cpp GPSNmea.cpp

# List ASM source files here
ASRC = crt.s

# List all user directories here
UINCDIR = ../arm/arm7lib

# List the user directory to look for the libraries here
ULIBDIR = ../arm/arm7lib

# List all user libraries here
ULIBS = ../arm/arm7lib/libarm7lib.a 
#-larm7lib

# Define optimisation level here (set to -O0 for debugging)
OPT = -O0 # O2

#
# End of user defines
##########################################################################

INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))
DEFS    = $(DDEFS) $(UDEFS)
ADEFS   = $(DADEFS) $(UADEFS)
OBJS    = $(ASRC:.s=.o) $(CSRC:.c=.o) $(CPPSRC:.cpp=.o)
LIBS    = $(ULIBS) $(DLIBS)
MCFLAGS = -mcpu=$(MCU)

ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -Wa,-amhls=$(<:.s=.lst) $(ADEFS)
CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -fomit-frame-pointer -ffunction-sections -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)
CPPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -fno-rtti -fno-exceptions -fomit-frame-pointer -ffunction-sections -Wall -fverbose-asm -Wa,-ahlms=$(<:.cpp=.lst) $(DEFS)
LDFLAGS = $(MCFLAGS) -T$(LDSCRIPT) -Wl,-Map=$(PROJECT).map,--cref,--gc-sections,--no-warn-mismatch $(LIBDIR)
#		-nostartfiles 											 -t

# Generate dependency information
CPFLAGS += -MD -MP -MF .dep/$(@F).d
CPPFLAGS += -MD -MP -MF .dep/$(@F).d


##########################################################################
# Start of makefile rules
#
all: $(OBJS) $(PROJECT).elf $(PROJECT).bin $(PROJECT).hex 

%o : %c
	$(CC) -c $(CPFLAGS) -I . $(INCDIR) $< -o $@

%o : %cpp
	$(CPP) -c $(CPPFLAGS) -I . $(INCDIR) $< -o $@

%o : %s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(OBJS)
	$(CPP) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%bin: %elf
	$(BIN) $< $@

%hex: %elf
	$(HEX) $< $@
	$(SIZE) $<
	
docs:
	doxygen APRSBeacon.dox	

clean:
	-rm -f $(OBJS)
	-rm -f $(PROJECT).elf
	-rm -f $(PROJECT).bin
	-rm -f $(PROJECT).map
	-rm -f $(PROJECT).hex
	-rm -f $(CSRC:.c=.lst)
	-rm -f $(CPPSRC:.cpp=.lst)
	-rm -f $(ASRC:.s=.lst)
	-rm -fR .dep
	-rm -fR html
	
install:
	openocd -f lpc21xx_ocd.cfg -c "script lpc21xx_flash.script"
	
reset:
	openocd -f lpc21xx_ocd.cfg -c reset
	
debug:
	openocd-ftd2xx -f usbscarab.cfg

# 
# Include the dependency files, should be the last of the makefile
#
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

#
# End of makefile rules
##########################################################################
