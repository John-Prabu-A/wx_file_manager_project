#include <wx/wx.h>
#include <wx/combo.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/sizer.h>

// Define a new application type
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// Define a new frame type
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title);

private:
    void OnTextChanged(wxCommandEvent &event);
    wxComboBox *comboBox;
    wxArrayString allItems;
};

// Implement the application
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame("Dropdown Search Example");
    frame->Show(true);
    return true;
}

// Implement the frame
MyFrame::MyFrame(const wxString &title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300))
{
    // Create a panel to contain the controls
    wxPanel *panel = new wxPanel(this, wxID_ANY);

    // Initialize the combo box with initial items
    allItems.Add("Apple");
    allItems.Add("Banana");
    allItems.Add("Cherry");
    allItems.Add("Date");
    allItems.Add("Elderberry");
    allItems.Add("Fig");
    allItems.Add("Grape");

    // Create the combo box
    comboBox = new wxComboBox(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, allItems, wxCB_DROPDOWN);
    comboBox->Bind(wxEVT_TEXT, &MyFrame::OnTextChanged, this);

    // Create a sizer to manage the layout
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(comboBox, 0, wxALL | wxEXPAND, 10);
    panel->SetSizer(sizer);
}

void MyFrame::OnTextChanged(wxCommandEvent &event)
{
    wxString value = comboBox->GetValue();

    // Temporarily unbind the event to avoid recursive triggering
    comboBox->Unbind(wxEVT_TEXT, &MyFrame::OnTextChanged, this);

    // Clear and repopulate the combo box
    comboBox->Clear();
    for (const auto &item : allItems)
    {
        if (item.Lower().Contains(value.Lower()))
        {
            comboBox->Append(item);
        }
    }

    comboBox->SetValue(value);
    comboBox->SetInsertionPointEnd();
    if (comboBox->GetCount() > 0)
    {
        comboBox->Popup();
    }

    // Rebind the event
    comboBox->Bind(wxEVT_TEXT, &MyFrame::OnTextChanged, this);
}
