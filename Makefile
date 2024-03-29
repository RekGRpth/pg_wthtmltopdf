EXTENSION = pg_wthtmltopdf
MODULE_big = $(EXTENSION)

PG_CONFIG = pg_config
OBJS = MyWPdfRenderer.o $(EXTENSION).o
DATA = pg_wthtmltopdf--1.0.sql

LIBS += -lstdc++ -lwt
SHLIB_LINK := $(LIBS)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

$(OBJS): MyWPdfRenderer.h Makefile