#ifndef STYLED_BUTTON_H
#define STYLED_BUTTON_H

#include <wx/wx.h>

class StyledButton : public wxButton
{
public:
    StyledButton(wxWindow *parent, wxWindowID id, const wxString &label,
                 const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
                 const wxColour &backgroundColor = wxNullColour, const wxColour &foregroundColor = wxNullColour,
                 const wxFont &font = wxNullFont, long additionalStyle = wxNO_BORDER,
                 const wxValidator &validator = wxDefaultValidator,
                 const wxString &name = wxButtonNameStr);

    void SetStyles(const wxColour &backgroundColor, const wxColour &foregroundColor, const wxFont &font, long additionalStyle);
    void OnMouseEnter(wxMouseEvent &event);
    void OnMouseLeave(wxMouseEvent &event);

private:
    wxColour m_backgroundColor;
    wxColour m_foregroundColor;
    wxFont m_font;
};

#endif // STYLED_BUTTON_H
