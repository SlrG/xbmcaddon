import os, sys, xbmc, xbmcaddon, xbmcgui, subprocess, threading

Addon = xbmcaddon.Addon('script.windowsscreensaver')

__scriptname__ = Addon.getAddonInfo('name')
__path__ = Addon.getAddonInfo('path')
cmd = os.path.join(__path__,'resources','scrsvr.exe')

#------------------------------------------------------------------------------

class Timer(threading.Thread):

    def __init__(self, msec, loop, exit_callback):
        self.runTime = msec
        self.loop = loop
        self.exit_callback = exit_callback
        threading.Thread.__init__(self)

    def run(self):
        #check for running windows screensaver
        while True: 
            xbmc.sleep(self.runTime)
            svrcheck = subprocess.Popen([cmd,'-c'])
            svrcheck.wait()
            if svrcheck.returncode == 1:
                self.loop = False
                print '3 Screensaver: External Screensaver stopped'
                self.exit_callback()
            if not self.loop:
                break

#------------------------------------------------------------------------------

class Screensaver(xbmcgui.WindowXMLDialog):

    class ExitMonitor(xbmc.Monitor):

        def __init__(self, exit_callback):
            self.exit_callback = exit_callback

        def onScreensaverDeactivated(self):
            print '4 Screensaver: ExitMonitor sending exit_callback'
            self.exit_callback()

        def onAbortRequested(self):
            print '4 Screensaver: ExitMonitor detects Abort Request'
            self.exit_callback()

    def onInit(self):
        print '2 Screensaver: onInit'
        self.monitor = self.ExitMonitor(self.exit)
        #start windows screensaver
        subprocess.Popen([cmd,'-s'])
        xbmc.sleep(2000)
        t = Timer(500, True, self.exit)
        t.start()
             
    def exit(self):
        print '5 Screensaver: Exit requested'
        #check if external saver is still running and stop it
        subprocess.Popen([cmd,'-cs'])
        #restore XBMC Window (needed if True Fullscreen mode is used)
        subprocess.Popen([cmd,'-r'])
        self.close()

#------------------------------------------------------------------------------

if __name__ == '__main__':
    print '1 Screensaver: Python Screensaver Started'
    #check if windows screensaver is set
    svrcheck = subprocess.Popen([cmd,'-t'])
    svrcheck.wait()
    if svrcheck.returncode == 0:
        fcheck = subprocess.Popen([cmd,'-f'])
        fcheck.wait()
        if fcheck.returncode == 0:
            screensaver_gui = Screensaver('script-%s-main.xml' % __scriptname__,
                __path__,'default',)
            screensaver_gui.doModal()
            print '6 Screensaver: Python Screensaver Exits'
            del screensaver_gui
            #force stop of XBMC internal screensaver
            subprocess.Popen([cmd,'-k'])
        else:
            print '2-6 Screensaver: XBMC not active, no need to start windows saver. XBMC saver started.'
    else:
        print '2-6 Screensaver: No Windows screensaver is set. XBMC saver started.'
    print '7 Screensaver: Script Ends'
    sys.modules.clear()