#include "StyledButton.h"

wxBEGIN_EVENT_TABLE(StyledButton, wxPanel)
    EVT_ENTER_WINDOW(StyledButton::OnMouseEnter)
        EVT_LEAVE_WINDOW(StyledButton::OnMouseLeave)
            EVT_LEFT_DOWN(StyledButton::OnLeftDown)
                EVT_LEFT_UP(StyledButton::OnLeftUp)
                    EVT_PAINT(StyledButton::OnPaint)
                        wxEND_EVENT_TABLE()

    // Constructor

    StyledButton::StyledButton(wxWindow *parent, wxWindowID id, const wxString &label,
                               const wxPoint &pos, const wxSize &size,
                               const wxColour &backgroundColor, const wxColour &foregroundColor,
                               const wxColour &borderColor, int borderRadius, int borderWidth,
                               const wxFont &font, long additionalStyle,
                               const wxValidator &validator, const wxString &name)
    : wxPanel(parent, id, pos, size, additionalStyle, name),
      m_label(label),
      m_backgroundColor(backgroundColor.IsNull() ? *wxWHITE : backgroundColor),
      m_foregroundColor(foregroundColor.IsNull() ? *wxBLACK : foregroundColor),
      m_borderColor(borderColor.IsNull() ? *wxBLACK : borderColor),
      m_borderRadius(borderRadius),
      m_borderWidth(borderWidth),
      m_font(font.IsNull() ? wxFont(wxFontInfo().Family(wxFONTFAMILY_DEFAULT).Bold()) : font)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Apply styles
    SetFont(m_font);

    // Bind events
    Bind(wxEVT_ENTER_WINDOW, &StyledButton::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &StyledButton::OnMouseLeave, this);
    Bind(wxEVT_LEFT_DOWN, &StyledButton::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &StyledButton::OnLeftUp, this);
}

void StyledButton::SetClickHandler(std::function<void()> handler)
{
    m_clickHandler = handler;
}

void StyledButton::OnMouseEnter(wxMouseEvent &event)
{
    std::cout << "OnMouseEnter called.." << std::endl;
    isHovered = true;
    Refresh();
    event.Skip(false);
}

void StyledButton::OnMouseLeave(wxMouseEvent &event)
{
    std::cout << "OnMouseLeave called.." << std::endl;
    isHovered = false;
    Refresh();
    event.Skip(false);
}

void StyledButton::OnLeftDown(wxMouseEvent &event)
{
    std::cout << "OnLeftDown Called.." << std::endl;
    isPressed = true;
    Refresh();
    event.Skip(false);
}

void StyledButton::OnLeftUp(wxMouseEvent &event)
{
    isPressed = false;
    Refresh();
    if (m_clickHandler)
    {
        std::cout << "OnLeftUP Called..." << std::endl;
        m_clickHandler();
    }
    event.Skip(false);
}

void StyledButton::OnPaint(wxPaintEvent &event)
{
    wxAutoBufferedPaintDC dc(this);
    // wxGCD dc(dc); // Ensure wx/graphics.h is included

    wxSize size = GetSize();
    wxRect rect(0, 0, size.GetWidth(), size.GetHeight());

    wxColour bgColor = isPressed ? m_backgroundColor.ChangeLightness(80) : (isHovered ? m_backgroundColor.ChangeLightness(120) : m_backgroundColor);

    dc.SetBrush(wxBrush(bgColor));
    dc.SetPen(wxPen(m_borderColor, m_borderWidth));

    dc.DrawRoundedRectangle(rect, m_borderRadius);

    dc.SetFont(m_font);
    dc.SetTextForeground(m_foregroundColor);
    wxSize textSize = dc.GetTextExtent(m_label);
    int textX = (size.GetWidth() - textSize.GetWidth()) / 2;
    int textY = (size.GetHeight() - textSize.GetHeight()) / 2;

    dc.DrawText(m_label, textX, textY);
}

void StyledButton::swap(wxColour &a, wxColour &b)
{
    wxColour temp = a;
    a = b;
    b = temp;
}

void StyledButton::SetStyles(const wxColour &backgroundColor, const wxColour &foregroundColor,
                             const wxColour &borderColor, int borderRadius, int borderWidth, const wxFont &font, long additionalStyle)
{
    m_backgroundColor = backgroundColor.IsNull() ? *wxWHITE : backgroundColor;
    m_foregroundColor = foregroundColor.IsNull() ? *wxBLACK : foregroundColor;
    m_borderColor = borderColor.IsNull() ? *wxBLACK : borderColor;
    m_borderRadius = borderRadius;
    m_borderWidth = borderWidth;
    m_font = font.IsNull() ? wxFont(wxFontInfo().Family(wxFONTFAMILY_DEFAULT).Bold()) : font;

    // Apply styles
    SetFont(m_font);
    SetWindowStyle(additionalStyle);

    Refresh();
}
