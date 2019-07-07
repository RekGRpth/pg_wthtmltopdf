#include <postgres.h>
#include <fmgr.h>

#include <catalog/pg_type.h>
#include <libpq/pqformat.h>
#include <utils/builtins.h>

#include "MyWPdfRenderer.h"

#define EXTENSION(function) Datum (function)(PG_FUNCTION_ARGS); PG_FUNCTION_INFO_V1(function); Datum (function)(PG_FUNCTION_ARGS)

PG_MODULE_MAGIC;

static void HPDF_STDCALL error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data) {
    ereport(ERROR, (errmsg("libharu: error_no=%04X, detail_no=%d\n", (unsigned int) error_no, (int) detail_no)));
}

static void error(const char *msg) {
    ereport(ERROR, (errmsg("%s", msg)));
}

EXTENSION(html2pdf) {
    text *html, *result = NULL;
    HPDF_Doc pdf;
    HPDF_Page page;
    HPDF_UINT32 size;
    if (PG_ARGISNULL(0)) ereport(ERROR, (errmsg("html is null!")));
    html = PG_GETARG_TEXT_P(0);
    if (!(pdf = HPDF_NewEx(error_handler, (HPDF_Alloc_Func)palloc, pfree, 0, NULL))) ereport(ERROR, (errmsg("!pdf")));
    if (HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL) != HPDF_OK) goto HPDF_Free;
    if (HPDF_UseUTFEncodings(pdf) != HPDF_OK) goto HPDF_Free;
    if (!(page = HPDF_AddPage(pdf))) goto HPDF_Free;
    if (HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT) != HPDF_OK) goto HPDF_Free;
    if (!MyWPdfRenderer_render(error, pdf, page, VARDATA_ANY(html))) goto HPDF_Free;
    if (HPDF_SaveToStream(pdf) != HPDF_OK) goto HPDF_Free;
    if (!(size = HPDF_GetStreamSize(pdf))) goto HPDF_Free;
    if (!(result = palloc(size + VARHDRSZ))) goto HPDF_Free;
    SET_VARSIZE(result, size + VARHDRSZ);
    switch (HPDF_ReadFromStream(pdf, (HPDF_BYTE *)VARDATA_ANY(result), &size)) {
        case HPDF_OK: break;
        case HPDF_STREAM_EOF: break;
        default: goto HPDF_Free;
    }
HPDF_Free:
    page = HPDF_GetCurrentPage(pdf);
    if (page) while (HPDF_Page_GetGStateDepth(page) > 1) HPDF_Page_GRestore(page);
    (void)HPDF_Free(pdf);
    (void)pfree(html);
    if (!result) PG_RETURN_NULL();
    PG_RETURN_TEXT_P(result);
}
