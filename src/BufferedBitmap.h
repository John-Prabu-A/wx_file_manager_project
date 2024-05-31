#ifndef BUFFEREDBITMAP_H
#define BUFFEREDBITMAP_H

#pragma once

#include <wx/wx.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>

class BufferedBitmap : public wxScrolled<wxWindow>
{
public:
    BufferedBitmap(wxWindow *parent, wxWindowID id, const wxBitmap &b, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0);

    void SetBitmap(const wxBitmap &bitmap);
    const wxBitmap &GetBitmap() const;

    double GetZoomMultiplier() const;
    double GetZoomPercentage() const;

    void ZoomIn();
    void ZoomOut();

private:
    void OnPaint(wxPaintEvent &evt);
    wxSize GetScaledBitmapSize() const;
    void CenterAfterZoom(wxPoint previousCenter, wxPoint currentCenter);

    wxBitmap bitmap;
    const double ZOOM_FACTOR = 1.1;
    int zoomLevel = 0;
};

#endif // BUFFEREDBITMAP_H
