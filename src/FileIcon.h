#ifndef FILEICON_H
#define FILEICON_H

#include <wx/wx.h>
#include <wx/filename.h>
#include <map>

class FileIcon : public wxPanel
{
public:
    FileIcon(wxWindow *parent, wxString fileName, wxString filePath, const wxSize &size = wxDefaultSize, wxWindowID id = wxID_ANY,
             const wxPoint &pos = wxDefaultPosition,
             long style = wxTAB_TRAVERSAL, const wxString &name = wxPanelNameStr);

    void Activate();
    void Deactivate();

    wxString GetFilePath() const;
    void SetFilePath(wxString filePath);
    wxString GetFileName() const;
    void SetFileName(wxString fileName);
    bool isActive();
    void SetIconOpacity(unsigned char alpha);

private:
    void OnLeftClick(wxMouseEvent &event);
    void OnIconClick(wxMouseEvent &event);
    void OnTextClick(wxMouseEvent &event);
    void OnDoubleClick(wxMouseEvent &event);
    void OnTextDoubleClick(wxMouseEvent &event);
    void AdjustText();

    wxStaticBitmap *m_iconBitmap;
    wxStaticText *m_text;
    wxString m_fileName;
    wxString m_filePath;
    bool m_active;

    // Map to store file extensions and their corresponding icon paths
    static std::map<wxString, wxString> fileIconMap;
    static void InitializeFileIconMap();
    wxString GetFileExtension(wxString filePath);
    wxString GetIconPath(wxString fileExtension);
};

#endif // FILEICON_H
