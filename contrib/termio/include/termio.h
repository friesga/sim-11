#pragma once

HANDLE open_stdin ();
int select_stdin (HANDLE);
int read_stdin (HANDLE, char*, int);
int getattr_stdin ();
int setattr_stdin (HANDLE);