#ifndef _SDLFILEDIALOG_H_
#define _SDLFILEDIALOG_H_

#include <SDL3/SDL.h>
#include <optional>
#include <string>
#include <memory>
#include <functional>
#include <future>

class SDLFileDialog
{
public:
    using DialogResult = std::optional<std::string>;
    using PromiseType = std::promise<DialogResult>;
    using SelectionCallback = std::function<void (void* userData,
        const char * const *filelist, int filter)>;

    // Synchronous version, returning when the user has selected a file
    // to load.
    [[nodiscard]] DialogResult loadFileDialog ();

    // Asynchronous version, returns immediately after opening the file
    // selector. The callback will be called when the user has selected
    // the file to load.
    void loadFileDialog (SelectionCallback callback);

private:
    // The selectionPromise_ is defined as class member to make it survive
    // the asynchronous  SDL_ShowOpenFileDialog() call.
    std::shared_ptr<PromiseType> selectionPromise_ {};
    SelectionCallback selectionCallback_;

    static void SDLCALL dialogCallback (void* userdata, const char* const* filelist,
        int filter);
    void selectionCompleted (void* userdata, const char* const* filelist, int filter);
};


#endif // _SDLFILEDIALOG_H_