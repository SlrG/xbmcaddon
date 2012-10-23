#include <windows.h>
#include <Winuser.h>

bool scrsvrexists ()
{
  int long lnResult;
  HKEY hKey=NULL;
  lnResult=RegOpenKeyEx(HKEY_CURRENT_USER,"Control Panel\\Desktop",0,KEY_READ,&hKey);
  if(!hKey)
    return false;
  LONG lRet = RegQueryValueEx(hKey, "SCRNSAVE.EXE", NULL, NULL, NULL, NULL);
  if (lRet == ERROR_SUCCESS)
    return true;
  else
    return false;
}

void start ()
{
  DefWindowProc(GetDesktopWindow(), WM_SYSCOMMAND, SC_SCREENSAVE, 0 );
}

int check()
{
  BOOL bScrnSvrRunning = FALSE;
  ::SystemParametersInfo(SPI_GETSCREENSAVERRUNNING,0,&bScrnSvrRunning,0);
  return bScrnSvrRunning;
}

BOOL CALLBACK KillScreenSaverFunc(HWND hwnd, LPARAM lParam) 
{ 
	if(IsWindowVisible(hwnd)) 
		PostMessage(hwnd, WM_CLOSE, 0, 0); 
	return TRUE; 
} 

void stop ()
{
  if (check())
  {
    HDESK hdesk; 
    hdesk = OpenDesktop(TEXT("Screen-saver"), 0, FALSE, DESKTOP_READOBJECTS | DESKTOP_WRITEOBJECTS); 
    if (hdesk) 
    { 
	     EnumDesktopWindows(hdesk, KillScreenSaverFunc, 0);
	     CloseDesktop(hdesk);
    } 
    else 
      PostMessage(GetForegroundWindow(), WM_CLOSE, 0, 0L); 
  }
}

void wait()
{
  BOOL bScrnSvrRunning = FALSE;
  // give the screensaver some time to activate and then wait for it to close
  do
    Sleep(1000);
  while (check());
}

void restore()
{
   HWND hWnd_xbmc;
   hWnd_xbmc = FindWindow(NULL,"xbmc");
   if(hWnd_xbmc)
     ShowWindow(hWnd_xbmc, SW_RESTORE);   
}

void keypressed()
{
   HWND hWnd_xbmc;
   hWnd_xbmc = FindWindow(NULL,"xbmc");
   PostMessage(hWnd_xbmc, WM_KEYDOWN, VK_UP, 0);
}

int WINAPI WinMain(HINSTANCE inst,HINSTANCE prev,LPSTR cmd,int show)
{
  if (strcmp (cmd,"-t") == 0)
  {
    if (scrsvrexists ())
      return 0;
    else
      return 1;
  }
  else if (strcmp (cmd,"-s") == 0)
    start();
  else if (strcmp (cmd,"-c") == 0)
  {
    if (!check())
      return 1;
  }
  else if (strcmp (cmd,"-cs") == 0)
    stop();
  else if (strcmp (cmd,"-r") == 0)
    restore();
  else if (strcmp (cmd,"-k") == 0)
    keypressed();
  else
  {
    MessageBox(NULL, "Command Line Switches are:\n\n"
                     "-t\tcheck if windows screensaver is set\n"
                     "-s\tstart system screensaver\n"
                     "-c\tcheck for running screensaver\n"
                     "-cs\tcheck and stop\n"
                     "-r\trestore XBMC to fullscreen\n"
                     "-k\tsend <up> key to XBMC", "Help",
                     MB_ICONINFORMATION|MB_OK);
    return 10;
  }
  return 0;
}