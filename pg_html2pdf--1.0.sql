-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_html2pdf" to load this file. \quit

CREATE OR REPLACE FUNCTION html2pdf(html text) RETURNS bytea AS 'MODULE_PATHNAME', 'html2pdf' LANGUAGE 'c';
