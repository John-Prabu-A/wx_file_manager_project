#ifndef FOLDERICON_H
#define FOLDERICON_H

#include <wx/wx.h>

class FolderIcon : public wxPanel
{
public:
    FolderIcon(wxWindow *parent, std::string folderName, wxString folderPath, const wxSize &size = wxDefaultSize, wxWindowID id = wxID_ANY,
               const wxPoint &pos = wxDefaultPosition,
               long style = wxTAB_TRAVERSAL, const wxString &name = wxPanelNameStr);

    void Activate();
    void Deactivate();
    bool isActive();

    wxString GetFolderPath() const;
    wxString GetFolderName() const;
    void SetFolderPath(wxString folderPath);
    void SetFolderName(wxString folderName);
    void SetIconOpacity(unsigned char alpha);

private:
    void OnLeftClick(wxMouseEvent &event);
    void OnIconClick(wxMouseEvent &event);
    void OnTextClick(wxMouseEvent &event);
    void OnDoubleClick(wxMouseEvent &event);
    void OnTextDoubleClick(wxMouseEvent &event);
    void AdjustText();

    wxIcon m_iconNormal;
    wxIcon m_iconActive;
    wxStaticBitmap *m_iconBitmap;
    wxStaticText *m_text;
    wxString m_folderName;
    wxString m_folderPath;
    bool m_active;
};

#endif // FOLDERICON_H
