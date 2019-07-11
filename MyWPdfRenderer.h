#ifndef __MYWPDFRENDERER_H
#define __MYWPDFRENDERER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hpdf.h>

typedef void (MyWPdfRenderer_error)(const char *msg);
bool MyWPdfRenderer_render(MyWPdfRenderer_error error, HPDF_Doc pdf, HPDF_Page page, const char *html);

#ifdef __cplusplus
}
#endif
#endif