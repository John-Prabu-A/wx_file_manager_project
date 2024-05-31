#include <wx/wx.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

private:
    void OnSendOrEnter(wxCommandEvent &event);
    void OnReceiveOrSpace(wxKeyEvent &event);
    void OnExit(wxCloseEvent &event);

    wxTextCtrl *textCtrl;
    wxTextCtrl *messageTextBox; // Text box for typing messages
    const char *fifoName;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("Program 1", wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    fifoName = "/tmp/myfifo";
    mkfifo(fifoName, 0666); // Creating FIFO pipe

    wxPanel *panel = new wxPanel(this, wxID_ANY);

    textCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY); // Text area for displaying messages

    messageTextBox = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);

    messageTextBox->Bind(wxEVT_TEXT_ENTER, &MyFrame::OnSendOrEnter, this); // Bind Enter key press event to message text box
    Bind(wxEVT_CHAR_HOOK, &MyFrame::OnReceiveOrSpace, this);               // Bind space key press event to frame

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(textCtrl, 1, wxEXPAND | wxALL, 5);
    vbox->Add(messageTextBox, 0, wxEXPAND | wxALL, 5); // Add the message text box

    panel->SetSizer(vbox);

    Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnExit, this);
}

void MyFrame::OnSendOrEnter(wxCommandEvent &event)
{
    int fd = open(fifoName, O_WRONLY); // Open FIFO pipe for writing
    if (fd == -1)
    {
        std::cerr << "Error opening FIFO pipe\n";
        return;
    }

    wxString data = messageTextBox->GetValue();
    write(fd, (const void *)data.c_str(), data.length()); // Writing data to FIFO pipe

    close(fd);
    messageTextBox->Clear(); // Clear message text box after sending
}

void MyFrame::OnReceiveOrSpace(wxKeyEvent &event)
{
    if (event.GetKeyCode() == WXK_RIGHT) // Check if the pressed key is space
    {
        int fd = open(fifoName, O_RDONLY); // Open FIFO pipe for reading
        if (fd == -1)
        {
            std::cerr << "Error opening FIFO pipe\n";
            return;
        }

        char buffer[1024];
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer)); // Read data from FIFO pipe
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            wxString receivedMessage(buffer, wxConvUTF8);
            textCtrl->AppendText(receivedMessage + "\n"); // Append received message to text area
        }

        close(fd);
    }
    else
    {
        event.Skip(); // Allow default key event handling for other keys
    }
}

void MyFrame::OnExit(wxCloseEvent &event)
{
    unlink(fifoName); // Deleting FIFO pipe
    event.Skip();     // Allow the default window close event processing to occur
}
