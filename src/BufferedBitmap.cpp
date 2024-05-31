#include "BufferedBitmap.h"

BufferedBitmap::BufferedBitmap(wxWindow *parent, wxWindowID id, const wxBitmap &b, const wxPoint &pos, const wxSize &size, long style)
    : wxScrolled<wxWindow>(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE | wxVSCROLL | wxHSCROLL | style)
{
    this->SetBackgroundStyle(wxBG_STYLE_PAINT); // needed for windows

    this->Bind(wxEVT_PAINT, &BufferedBitmap::OnPaint, this);
    this->SetBitmap(b);
}

void BufferedBitmap::OnPaint(wxPaintEvent &evt)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    DoPrepareDC(dc);

    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    if (gc)
    {
        // scaling consistent with wxStaticBitmap
        // GetVirtualSize() returns the bitmap size or the client size if the bitmap is smaller
        const wxSize drawSize = ToDIP(GetVirtualSize());

        const wxSize bmpSize = GetScaledBitmapSize();

        double w = bmpSize.GetWidth();
        double h = bmpSize.GetHeight();

        double x = (drawSize.GetWidth() - w) / 2;
        double y = (drawSize.GetHeight() - h) / 2;

        double scaleFactor = 1.0; // Assuming no scaling initially
        if (GetParent())
        {
            wxSize dpi = GetParent()->GetDPI();
            scaleFactor = static_cast<double>(dpi.GetWidth()) / 96.0; // Assuming base DPI is 96
        }

        wxPoint logicalPos(static_cast<int>(x * scaleFactor), static_cast<int>(y * scaleFactor));
        wxSize logicalSize(static_cast<int>(w * scaleFactor), static_cast<int>(h * scaleFactor));
        gc->DrawBitmap(bitmap, logicalPos.x, logicalPos.y, logicalSize.x, logicalSize.y);

        delete gc;
    }
}

void BufferedBitmap::SetBitmap(const wxBitmap &bitmap)
{
    this->bitmap = bitmap;

    SetScrollRate(FromDIP(5), FromDIP(5));
    SetVirtualSize(FromDIP(GetScaledBitmapSize()));

    this->Refresh();
}

const wxBitmap &BufferedBitmap::GetBitmap() const
{
    return bitmap;
}

double BufferedBitmap::GetZoomMultiplier() const
{
    return std::pow(ZOOM_FACTOR, zoomLevel);
}

double BufferedBitmap::GetZoomPercentage() const
{
    return GetZoomMultiplier() * 100;
}

void BufferedBitmap::ZoomIn()
{
    auto centerPos = CalcUnscrolledPosition(wxPoint(GetClientSize().GetWidth() / 2, GetClientSize().GetHeight() / 2));

    zoomLevel++;

    // reversed order to avoid unnecessary repaint and flickering
    CenterAfterZoom(centerPos, centerPos * ZOOM_FACTOR);
    SetVirtualSize(FromDIP(GetScaledBitmapSize()));

    this->Refresh();
    this->Update(); // force repaint
}

void BufferedBitmap::ZoomOut()
{
    auto centerPos = CalcUnscrolledPosition(wxPoint(GetClientSize().GetWidth() / 2, GetClientSize().GetHeight() / 2));

    zoomLevel--;

    // reversed order to avoid unnecessary repaint and flickering
    CenterAfterZoom(centerPos, centerPos * (1.0 / ZOOM_FACTOR));
    SetVirtualSize(FromDIP(GetScaledBitmapSize()));

    this->Refresh();
    this->Update(); // force repaint
}

wxSize BufferedBitmap::GetScaledBitmapSize() const
{
    const wxSize bmpSize = bitmap.GetSize();
    const double zoom = GetZoomMultiplier();
    return wxSize(bmpSize.GetWidth() * zoom, bmpSize.GetHeight() * zoom);
}

void BufferedBitmap::CenterAfterZoom(wxPoint previousCenter, wxPoint currentCenter)
{
    wxPoint pixelsPerUnit;
    GetScrollPixelsPerUnit(&pixelsPerUnit.x, &pixelsPerUnit.y);

    auto delta = currentCenter - previousCenter;

    auto destX = GetViewStart().x + delta.x / pixelsPerUnit.x;
    auto destY = GetViewStart().y + delta.y / pixelsPerUnit.y;

    Scroll(destX, destY);
}
