#ifndef STYLEDBUTTON_H
#define STYLEDBUTTON_H

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <functional>

class StyledButton : public wxPanel
{
public:
    StyledButton(wxWindow *parent, wxWindowID id, const wxString &label,
                 const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
                 const wxColour &backgroundColor = wxNullColour, const wxColour &foregroundColor = wxNullColour,
                 const wxColour &borderColor = wxNullColour, int borderRadius = 10, int borderWidth = 1,
                 const wxFont &font = wxNullFont, long additionalStyle = 0,
                 const wxValidator &validator = wxDefaultValidator, const wxString &name = wxPanelNameStr);

    void SetStyles(const wxColour &backgroundColor, const wxColour &foregroundColor,
                   const wxColour &borderColor, int borderRadius, int borderWidth, const wxFont &font, long additionalStyle);

    void SetClickHandler(std::function<void()> handler);

private:
    void OnMouseEnter(wxMouseEvent &event);
    void OnMouseLeave(wxMouseEvent &event);
    void OnLeftDown(wxMouseEvent &event);
    void OnLeftUp(wxMouseEvent &event);
    void OnPaint(wxPaintEvent &event);

    void swap(wxColour &a, wxColour &b);

    wxString m_label;
    wxColour m_backgroundColor;
    wxColour m_foregroundColor;
    wxColour m_borderColor;
    int m_borderRadius;
    int m_borderWidth;
    wxFont m_font;
    bool isPressed = false;
    bool isHovered = false;

    std::function<void()> m_clickHandler;

    wxDECLARE_EVENT_TABLE();
};

#endif // STYLEDBUTTON_H
