#include "wasmcartridge_handler.h"
#include "../wasmmessagebox/wasmmessagebox.h"

#include <iostream>
#include <optional>
#include <string>

#include <stdio.h>  // For fopen and fread

using std::string;

WASMCartridgeHandler::WASMCartridgeHandler (DiskDrive* diskDrive)
    :
    diskDrive_ (diskDrive)
{}

// The function operationActionRequested is called when the user requests
// an operation on the cartridge, such as loading or unloading it.
//
// The function gets the coordinates of the mouse click to open the popup
// menu at that position. This way this class is independent of SDL.
//
void WASMCartridgeHandler::operatorActionRequested (int x, int y)
{
    popupMenu_.open (x, y);
}

void WASMCartridgeHandler::menuItemSelected (const MenuOption* menuOption)
{
    switch (static_cast<OperatorAction> (menuOption->value ()))
    {
        case OperatorAction::UnloadCartridge:
            unloadCartridge ();
            break;

        case OperatorAction::LoadCartridge:
            loadCartridge ();
            break;

        case OperatorAction::DownloadCartridge:
            downloadCartridge ();
            break;

        case OperatorAction::None:
            std::cerr << "No option selected\n";
            break;
    }
}

void WASMCartridgeHandler::loadCartridge ()
{
    if (diskDrive_->spindleStopped () || diskDrive_->noCartridgeLoaded ())
    {
        dialog_.uploadFileDialog ([this] (WASMFileDialog::File* file)
            {this->fileSelected (file); });
    }
    else
        messageBox_.open ("Spin down disk before loading a cartridge");
}

void WASMCartridgeHandler::fileSelected (WASMFileDialog::File* selectedFile)
{
    uploadedFileName_ = "/"+ selectedFile->name ();

    dialog_.copyToMemfs (selectedFile, uploadedFileName_,
        [this] (bool success) {this->uploadCompleted (success); });
}

void WASMCartridgeHandler::uploadCompleted (bool success)
{
    if (success && fileExists (uploadedFileName_) &&
        diskDrive_->loadCartridge (uploadedFileName_))
            messageBox_.open ("Upload succeeded");
    else
        messageBox_.open ("Upload failed");
}

void WASMCartridgeHandler::unloadCartridge ()
{
    if (diskDrive_->spindleStopped ())
    {
        diskDrive_->unloadCartridge ();
        messageBox_.open ("Cartridge has been unloaded");
    }
    else if (diskDrive_->noCartridgeLoaded ())
        messageBox_.open ("No cartridge loaded");
    else
        messageBox_.open ("Spin down disk before unloading the cartridge");
}

void WASMCartridgeHandler::downloadCartridge ()
{
    if (diskDrive_->spindleStopped ())
    {
        const string cartridgeFileName = diskDrive_->cartridgeFileName ();

        if (!cartridgeFileName.empty () && fileExists (cartridgeFileName))
        {
            dialog_.downloadFileDialog (cartridgeFileName,
                [this] (WASMFileDialog::DownloadResult result) {this->downloadCompleted (result); });     
        }
        else
            messageBox_.open ("No cartridge loaded");
    }
    else
        messageBox_.open ("Spin down disk before downloading the cartridge");
}

void WASMCartridgeHandler::downloadCompleted (WASMFileDialog::DownloadResult result)
{
    if (result == WASMFileDialog::DownloadResult::Completed ||
            result == WASMFileDialog::DownloadResult::Started)
        std::cerr << "Bestand opgeslagen\n";
    else if (result == WASMFileDialog::DownloadResult::Failed)
        std::cerr << "File could not be downloaded\n"; 
}

bool WASMCartridgeHandler::fileExists (const std::string& fileName)
{
    FILE* pFile = fopen (fileName.c_str (), "r");
    if (pFile == NULL)
        return false;

    fclose (pFile);
    return true;
}

