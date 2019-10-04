# Copyright 2013-2018 Alexander Peslyak
# Copyright 2019 Volbil
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

CC = gcc
LD = $(CC)
RM = rm -f
CFLAGS = -Wall -O2 -fomit-frame-pointer -I. -Iutils -Icrypto

PROJ = benchmark
OBJS_CORE = yespower.o
OBJS_COMMON = sha256.o
OBJS_BENCHMARK = $(OBJS_CORE) $(OBJS_COMMON) benchmark.o
CRYPTO = crypto/*.c
OBJS_RM = *.o

all: $(PROJ)

benchmark: $(OBJS_BENCHMARK)
	$(LD) $(OBJS_BENCHMARK) -o $@

benchmark.o: benchmark.c
	$(CC) -c $(CFLAGS) $(CRYPTO) $*.c

.c.o:
	$(CC) -c $(CFLAGS) $(CRYPTO) $*.c

yespower.o: yespower-platform.c yespower.h
benchmark.o: yespower.h

clean:
	$(RM) $(PROJ)
	$(RM) $(OBJS_BENCHMARK)
	$(RM) $(OBJS_RM)