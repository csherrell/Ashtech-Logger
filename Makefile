#
# Makefile for LAAS ground software
#
WARNINGS = -Wall -Wextra -ansi -pedantic

MOST_WARNINGS = -pedantic  -Wextra  -Wall  -Waggregate-return                 \
			-Wno-attributes -Wc++-compat -Wcast-align  -Wcast-qual  -Wchar-subscripts         \
			-Wcomment -Wconversion -Wno-deprecated-declarations -Wdisabled-optimization      \
			-Wno-div-by-zero -Wno-endif-labels                                                \
			-Wfloat-equal  -Wformat  -Wformat=2 -Wno-format-extra-args                        \
			-Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wimplicit                   \
			-Wimplicit-function-declaration -Wimplicit-int -Wimport -Wno-import -Winit-self   \
			-Winline -Wno-int-to-pointer-cast -Winvalid-pch             \
			-Wunsafe-loop-optimizations  -Wlong-long -Wmain                 \
			-Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute         \
			-Wmissing-include-dirs -Wmissing-noreturn -Wno-multichar  -Wnonnull -Wpacked      \
			-Wpadded -Wparentheses -Wpointer-arith  -Wno-pointer-to-int-cast                  \
			-Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare          \
			-Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch                  \
			-Wswitch-default -Wswitch-enum -Wsystem-headers -Wtrigraphs -Wundef               \
			-Wuninitialized -Wunknown-pragmas -Wno-pragmas -Wunreachable-code -Wunused        \
			-Wunused-function -Wunused-label -Wunused-parameter -Wunused-value                \
			-Wunused-variable  -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings      \
			-Wbad-function-cast -Wmissing-declarations -Wmissing-prototypes                   \
			-Wnested-externs -Wold-style-definition -Wstrict-prototypes -Wtraditional         \
			-Wdeclaration-after-statement -Wpointer-sign

ALL_WARNINGS = -pedantic  -pedantic-errors -w  -Wextra  -Wall  -Waggregate-return \
			-Wno-attributes -Wc++-compat -Wcast-align  -Wcast-qual  -Wchar-subscripts         \
			-Wcomment -Wconversion -Wno-deprecated-declarations -Wdisabled-optimization      \
			-Wno-div-by-zero -Wno-endif-labels -Werror  -Werror-implicit-function-declaration \
			-Wfatal-errors -Wfloat-equal  -Wformat  -Wformat=2 -Wno-format-extra-args         \
			-Wformat-nonliteral -Wformat-security  -Wformat-y2k -Wimplicit                   \
			-Wimplicit-function-declaration -Wimplicit-int -Wimport -Wno-import -Winit-self   \
			-Winline -Wno-int-to-pointer-cast -Winvalid-pch             \
			-Wunsafe-loop-optimizations  -Wlong-long -Wmain                 \
			-Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute         \
			-Wmissing-include-dirs -Wmissing-noreturn -Wno-multichar  -Wnonnull -Wpacked      \
			-Wpadded -Wparentheses -Wpointer-arith  -Wno-pointer-to-int-cast                  \
			-Wredundant-decls -Wreturn-type -Wsequence-point -Wshadow -Wsign-compare          \
			-Wstack-protector -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch                  \
			-Wswitch-default -Wswitch-enum -Wsystem-headers -Wtrigraphs -Wundef               \
			-Wuninitialized -Wunknown-pragmas -Wno-pragmas -Wunreachable-code -Wunused        \
			-Wunused-function -Wunused-label -Wunused-parameter -Wunused-value                \
			-Wunused-variable  -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings      \
			-Wbad-function-cast -Wmissing-declarations -Wmissing-prototypes                   \
			-Wnested-externs -Wold-style-definition -Wstrict-prototypes -Wtraditional         \
			-Wdeclaration-after-statement -Wpointer-sign


CC = cc
#CFLAGS = -O1 -std=c89 -Wall -Wextra -Wcomment -Wformat -Wmain -Wparentheses -Wswitch -Wunused -Wuninitialized -Wundef -Winline -Wmissing-declarations -Wlong-long -Wfloat-equal -Wunreachable-code -ansi -pedantic -g2 -ggdb2 -D__USE_POSIX -D_XOPEN_SOURCE
#CFLAGS = -std=c89 $(WARNINGS) -g2 -ggdb2 -D__USE_POSIX -D_XOPEN_SOURCE
CFLAGS = -std=c89 $(WARNINGS) -g3 -ggdb3 -D__USE_POSIX -D_XOPEN_SOURCE

#CFLAGS = -Werror -ansi -pedantic -g2 -ggdb2 -D__USE_POSIX -D_XOPEN_SOURCE
#CFLAGS = -g2 -Wall -pedantic
#CFLAGS = -ggdb2 -Wall -Wshadow -Wwrite-strings -Wconversion \
#         -Wsign-compare -Wredundant-decls

# options for gcc 3.x versions
# -Wfloat-equal -Wpadded -Wunreachable-code -std=c99

LDLIBS  = -lm -lgdbm
LDPATHS =

TELERAD = telerad/utils_string.c     \
		telerad/crc_16bit.c        \
		telerad/crc_32bit.c        \
		telerad/ephemeris_crc.c    \
		telerad/laasMessage.c      \
		telerad/laasMessageType1.c \
		telerad/laasMessageType2.c \
		telerad/laasMessageType4.c \
		telerad/telerad.c

ASHTECH = ashtech/shift_buffer.c \
		ashtech/ashtech.c      \
		ashtech/gg12.c         \
		ashtech/gg12w.c        \
		ashtech/zxtreme.c

LENS =    lens/xyzae.c     \
		lens/pseudorange_calculations.c  \
		lens/laas_calculation_sequence.c \
		lens/measurement_block.c

GDBM_DICTIONARY = gdbm/gdbm_dictionary.c

BINARY_SEARCH = binary_search/binary_search.c

SRCS = key_value_parser.c           \
	LAASInit.c                     \
	serial.c                       \
	find_string.c                  \
	conversions.c                  \
	time_conversions.c             \
	$(TELERAD)                     \
	$(ASHTECH)                     \
	satellite_position.c           \
	azimuth_elevation.c            \
	source_availability_duration.c \
	$(LENS)                        \
	$(GDBM_DICTIONARY)             \
	$(BINARY_SEARCH)               \
	snv_update.c                   \
	main.c

OBJS = $(SRCS:.c=.o)

EXEC = laas

all: $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LDPATHS) $(LDLIBS)

clean:
	rm -f $(OBJS) $(EXEC) core laas.db

empty:
	rm -f $(OBJS) $(EXEC) core laas.db tags

docs:
	doxygen Doxyfile; sudo cp -Rvu doxygen/html/. /var/www/lgf

count:
	wc *.c *.h */*.c */*.h

wc: count

tags:
	ctags -R
