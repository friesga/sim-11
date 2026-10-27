#include "wasmcartridge_handler.h"
#include "../wasmmessagebox/wasmmessagebox.h"

#include <iostream>
#include <optional>
#include <string>

#include <stdio.h>  // For fopen and fread


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
    dialog_.uploadFileDialog ([this] (WASMFileDialog::File* file)
        {this->fileSelected (file); });
}

void WASMCartridgeHandler::fileSelected (WASMFileDialog::File* selectedFile)
{
    std::cerr << "User selected " << selectedFile->name () << '\n';

    dialog_.copyToMemfs (selectedFile, "/uploaded.img",
        [this] (bool success) {this->uploadCompleted (success); });
}

void WASMCartridgeHandler::uploadCompleted (bool success)
{
    if (success && fileExists ("/uploaded.img"))
    {
        messageBox_.open ("Upload succeeded");
        printFile ("/uploaded.img");
    }
    else
        messageBox_.open ("Upload failed");
}

void WASMCartridgeHandler::unloadCartridge ()
{
    messageBox_.open ("The cartridge is unloaded");
}

void WASMCartridgeHandler::downloadCartridge ()
{
    if (!fileExists ("/uploaded.img"))
    {
        messageBox_.open ("No cartridge to download");
        return;
    }

    dialog_.downloadFileDialog ("/uploaded.img",
        [this] (WASMFileDialog::DownloadResult result) {this->downloadCompleted (result); });
}

void WASMCartridgeHandler::downloadCompleted (WASMFileDialog::DownloadResult result)
{
    if (result == WASMFileDialog::DownloadResult::Completed ||
            result == WASMFileDialog::DownloadResult::Started)
        std::cerr << "Bestand opgeslagen\n";
    else if (result == WASMFileDialog::DownloadResult::Failed)
        std::cerr << "File could not be downloaded\n"; 
}

bool WASMCartridgeHandler::fileExists (const char* fileName)
{
    FILE* pFile = fopen (fileName, "r");
    if (pFile == NULL)
        return false;

    fclose (pFile);
    return true;
}

void WASMCartridgeHandler::printFile (const char* fileName)
{
    FILE* pFile;
    pFile = fopen (fileName, "r");
    if (pFile == NULL)
    {
        std::cerr << "Open failed\n";
        return;
    }
 
    const size_t bufferSize {80};
    char buffer[bufferSize];

    size_t result = fread (buffer, 1, bufferSize, pFile);
    if (result != bufferSize)
    {
        std::cerr << "Read failed\n";
        return;
    }
   
    std::cerr << "Contents: " << std::oct <<
        static_cast<unsigned int> (buffer[0]) << " " <<
        static_cast<unsigned int> (buffer[1]) << " " <<
        static_cast<unsigned int> (buffer[2]) << " " <<
        static_cast<unsigned int> (buffer[3]) << " " <<
        std::dec << '\n';
        
    fclose (pFile);
}
