#include <Wt/Render/WPdfRenderer.h>
#include "MyWPdfRenderer.h"
#include <postgres.h>

namespace Wt { namespace rapidxml {
class parse_error: public std::exception {
public:
    parse_error(const char *what, void *location) : m_what(what), m_where(location) { }
    virtual const char *what() const throw() {
        return m_what;
    }
    template<class Ch> Ch *where() const {
        return reinterpret_cast<Ch *>(m_where);
    }
private:
    const char *m_what;
    void *m_where;
};
}}

extern "C" {
    bool MyWPdfRenderer_render(HPDF_Doc pdf, HPDF_Page page, const char *html) {
        try {
            Wt::Render::WPdfRenderer renderer(pdf, page);
            renderer.setMargin(1.0);
            renderer.setDpi(96);
            renderer.render(html);
            return true;
        } catch (const Wt::rapidxml::parse_error &e) {
            ereport(ERROR, (errmsg("wt exception: what = %s, where = %s", e.what(), e.where<char>())));
        } catch (const std::exception &e) {
            ereport(ERROR, (errmsg("wt exception: what = %s", e.what())));
        } catch (...) {
            ereport(ERROR, (errmsg("wt exception")));
        }
        return false;
    }
}