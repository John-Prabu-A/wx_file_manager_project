#include "StyledButton.h"

StyledButton::StyledButton(wxWindow *parent, wxWindowID id, const wxString &label,
                           const wxPoint &pos, const wxSize &size,
                           const wxColour &backgroundColor, const wxColour &foregroundColor,
                           const wxFont &font, long additionalStyle,
                           const wxValidator &validator, const wxString &name)
    : wxButton(parent, id, label, pos, size, additionalStyle, validator, name),
      m_backgroundColor(backgroundColor.IsNull() ? *wxWHITE : backgroundColor),
      m_foregroundColor(foregroundColor.IsNull() ? *wxBLACK : foregroundColor),
      m_font(font.IsNull() ? wxFont(wxFontInfo().Family(wxFONTFAMILY_DEFAULT).Bold()) : font)
{
    // Apply styles
    SetBackgroundColour(m_backgroundColor);
    SetForegroundColour(m_foregroundColor);
    SetFont(m_font);

    // Bind events
    Bind(wxEVT_ENTER_WINDOW, &StyledButton::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &StyledButton::OnMouseLeave, this);
}

void StyledButton::OnMouseEnter(wxMouseEvent &event)
{
    SetBackgroundColour(m_backgroundColor.ChangeLightness(150)); // Lighten the background color
    SetCursor(wxCursor(wxCURSOR_HAND));                          // Change cursor to hand
    Refresh();
    event.Skip();
}

void StyledButton::OnMouseLeave(wxMouseEvent &event)
{
    SetBackgroundColour(m_backgroundColor); // Revert to the original background color
    SetCursor(wxCursor(wxCURSOR_ARROW));    // Change cursor back to arrow
    Refresh();
    event.Skip();
}

void StyledButton::SetStyles(const wxColour &backgroundColor, const wxColour &foregroundColor, const wxFont &font, long additionalStyle)
{
    m_backgroundColor = backgroundColor;
    m_foregroundColor = foregroundColor;
    m_font = font;

    // Apply styles
    SetBackgroundColour(m_backgroundColor);
    SetForegroundColour(m_foregroundColor);
    SetFont(m_font);
    SetWindowStyle(additionalStyle);
}