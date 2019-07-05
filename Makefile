EXTENSION = pg_html2pdf
MODULE_big = $(EXTENSION)

PG_CONFIG = pg_config
OBJS = $(EXTENSION).o
DATA = pg_html2pdf--1.0.sql

SHLIB_LINK := $(LIBS)

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
