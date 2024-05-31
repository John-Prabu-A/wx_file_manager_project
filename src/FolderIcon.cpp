#pragma once
#include <wx/wx.h>

class FolderIcon : public wxPanel
{
public:
    FolderIcon(wxWindow *parent, std::string folderName, wxString folderPath, const wxSize &size = wxDefaultSize, wxWindowID id = wxID_ANY,
               const wxPoint &pos = wxDefaultPosition,
               long style = wxTAB_TRAVERSAL, const wxString &name = wxPanelNameStr)
        : wxPanel(parent, id, pos, size, style, name),
          m_folderName(folderName),
          m_folderPath(folderPath),
          m_active(false)
    {
        // Load icons
        m_iconNormal = wxIcon("src/resource/folder_icon_normal.ico", wxBITMAP_TYPE_ICO);
        m_iconActive = wxIcon("src/resource/folder_icon_active.ico", wxBITMAP_TYPE_ICO);

        // Display folder icon
        m_iconBitmap = new wxStaticBitmap(this, wxID_ANY, m_iconNormal);

        // Display folder name
        m_text = new wxStaticText(this, wxID_ANY, m_folderName, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);

        // Sizers to layout the components
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->AddStretchSpacer();
        sizer->Add(m_iconBitmap, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
        sizer->Add(m_text, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
        sizer->AddStretchSpacer();
        SetSizer(sizer);

        // Set fixed size for the sizer
        SetMinSize(wxSize(70, 70));
        SetMaxSize(wxSize(70, 70));

        // Binding events
        parent->Bind(wxEVT_LEFT_DOWN, &FolderIcon::OnLeftClick, this);
        m_iconBitmap->Bind(wxEVT_LEFT_DCLICK, &FolderIcon::OnDoubleClick, this);
        m_iconBitmap->Bind(wxEVT_LEFT_DOWN, &FolderIcon::OnIconClick, this);
        m_text->Bind(wxEVT_LEFT_DOWN, &FolderIcon::OnTextClick, this);
        m_text->Bind(wxEVT_LEFT_DCLICK, &FolderIcon::OnTextDoubleClick, this);
    }

    void Activate()
    {
        m_active = true;
        m_iconBitmap->SetIcon(m_iconActive);
    }

    void Deactivate()
    {
        m_active = false;
        m_iconBitmap->SetIcon(m_iconNormal);
    }

    wxString GetFolderPath() const
    {
        return m_folderPath;
    }
    void SetFolderPath(wxString folderPath)
    {
        m_folderPath = folderPath;
        return;
    }

private:
    void OnLeftClick(wxMouseEvent &event)
    {
        wxWindowList &children = GetParent()->GetChildren();
        for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
        {
            FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
            if (folderIcon)
            {
                folderIcon->Deactivate();
            }
        }
    }

    void OnIconClick(wxMouseEvent &event)
    {
        if (!m_active)
        {
            wxWindowList &children = GetParent()->GetChildren();
            for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
            {
                FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
                if (folderIcon)
                {
                    folderIcon->Deactivate();
                }
            }
            Activate();
            wxCommandEvent dummyEvent;
            wxPostEvent(GetParent(), dummyEvent);
        }
    }

    void OnTextClick(wxMouseEvent &event)
    {
        if (!m_active)
        {
            wxWindowList &children = GetParent()->GetChildren();
            for (wxWindowList::iterator it = children.begin(); it != children.end(); ++it)
            {
                FolderIcon *folderIcon = dynamic_cast<FolderIcon *>(*it);
                if (folderIcon)
                {
                    folderIcon->Deactivate();
                }
            }
            Activate();
            wxCommandEvent dummyEvent;
            wxPostEvent(GetParent(), dummyEvent);
        }
    }

    void OnDoubleClick(wxMouseEvent &event)
    {
        wxCommandEvent folderEvent(wxEVT_COMMAND_MENU_SELECTED, GetId());
        folderEvent.SetString(m_folderPath);
        GetEventHandler()->ProcessEvent(folderEvent);
    }

    void OnTextDoubleClick(wxMouseEvent &event)
    {
        wxCommandEvent folderEvent(wxEVT_COMMAND_MENU_SELECTED, GetId());
        folderEvent.SetString(m_folderPath);
        GetEventHandler()->ProcessEvent(folderEvent);
    }

    wxIcon m_iconNormal;
    wxIcon m_iconActive;
    wxStaticBitmap *m_iconBitmap;
    wxStaticText *m_text;
    wxString m_folderName;
    wxString m_folderPath;
    bool m_active;
};
