#include "sdlfiledialog.h"

#include <SDL3/SDL.h>
#include <string>
#include <optional>
#include <future>
#include <memory>

using std::string;
using std::optional;
using std::nullopt;
using std::promise;
using std::future;
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::move;

// This function is a "trampoline". It transforms a C function pointer needed
// by SDL_ShowOpenFileDialog() to a std::function passed to loadFileDialog().
// SDL calls dialogCallBack() which in turn calls the callback passed to
// loadFileDialog().
//
void SDLCALL SDLFileDialog::dialogCallback (void* userdata,
    const char* const* filelist, int filter)
{
    SDLFileDialog* fileDialog = static_cast<SDLFileDialog*> (userdata);

    fileDialog->selectionCallback_ (userdata, filelist, filter);
}

// Synchronous version, using the asynchronous version and providing the
// synchronization via a future/promise mechanism.
[[nodiscard]] optional<string> SDLFileDialog::loadFileDialog ()
{
    // Synchronization: wait until the user has selected a file
    selectionPromise_ = make_shared<PromiseType> ();
    future<DialogResult> selectionFuture = selectionPromise_->get_future ();

    loadFileDialog (
        [this] (void* userdata, const char* const* filelist, int filter)
        {
            selectionCompleted (userdata, filelist, filter);
        });

    return selectionFuture.get ();
}

// Asynchronous version
//
 void SDLFileDialog::loadFileDialog (SelectionCallback callback)
 {
    selectionCallback_ = move (callback);

    SDL_ShowOpenFileDialog (SDLFileDialog::dialogCallback, this,
        NULL, NULL, 0, NULL, false);
 }

 void SDLFileDialog::selectionCompleted (void* userdata,
     const char* const* filelist, int filter)
 {
     if (filelist == NULL || *filelist == NULL)
         selectionPromise_->set_value (nullopt);
     else
         selectionPromise_->set_value (string {*filelist});
 }