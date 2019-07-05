#include <postgres.h>
#include <fmgr.h>

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
    char *html;
    HPDF_Doc pdf;
    HPDF_Page page;
    HPDF_UINT32 size;
    HPDF_BYTE *buf = NULL;
    HPDF_STATUS hs = 0;
    if (PG_ARGISNULL(0)) ereport(ERROR, (errmsg("html is null!")));
    html = TextDatumGetCString(PG_GETARG_DATUM(0));
    if (!(pdf = HPDF_NewEx(error_handler, (HPDF_Alloc_Func)palloc, pfree, 0, NULL))) ereport(ERROR, (errmsg("!pdf")));
    if (HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL) != HPDF_OK) goto HPDF_Free;
    if (HPDF_UseUTFEncodings(pdf) != HPDF_OK) goto HPDF_Free;
    if (!(page = HPDF_AddPage(pdf))) goto HPDF_Free;
    if (HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT) != HPDF_OK) goto HPDF_Free;
    if (!MyWPdfRenderer_render(error, pdf, page, html)) goto HPDF_Free;
    if (HPDF_SaveToStream(pdf) != HPDF_OK) goto HPDF_Free;
    if (!(size = HPDF_GetStreamSize(pdf))) goto HPDF_Free;
    elog(LOG, "size = %u", size);
    if (!(buf = palloc(size))) goto HPDF_Free;
    switch ((hs = HPDF_ReadFromStream(pdf, buf, &size))) {
        case HPDF_OK: break;
        case HPDF_STREAM_EOF: break;
        default: goto HPDF_Free;
    }
HPDF_Free:
//    elog(LOG, "size = %u", size);
//    elog(LOG, "hs = %lu", hs);
    page = HPDF_GetCurrentPage(pdf);
    if (page) while (HPDF_Page_GetGStateDepth(page) > 1) HPDF_Page_GRestore(page);
    (void)HPDF_Free(pdf);
    (void)pfree(html);
    elog(LOG, "buf = %*.*s", size, size, (const char *)buf);
    elog(LOG, "strlen(buf) = %lu", strlen((const char *)buf));
    elog(LOG, "size = %u", size);
//    PG_RETURN_TEXT_P(cstring_to_text_with_len((const char *)buf, size));
    return CStringGetTextDatum((const char *)buf);
}
