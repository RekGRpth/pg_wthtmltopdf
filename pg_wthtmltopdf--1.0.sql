-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_wthtmltopdf" to load this file. \quit

CREATE OR REPLACE FUNCTION wthtmltopdf(html text) RETURNS text AS 'MODULE_PATHNAME', 'wthtmltopdf' LANGUAGE 'c';
