June 20:
=======

Created the Audio Thread, and all the Implementation of the WASAPI transfered into it. The the application is now functionnal.
The GUI been cleaned of all the menu items for testing.

The project can be flaged as operationnal enough...

June 17:
========

Created the AudioWorkerThread class, which should replace the run and me->PlayToneThreadProc used
in the MFC based project RenderSharedMGC. In this project, there is a cascade of method used: 

	void CMainDlg::OnBnClickedPlayButton()
		--> playTone(TargetFrequency, TargetDurationInSec);
			--> run
	    		--> me->PlayToneThreadProc(NULL);
And the function thread (PlayToneThreadProc) is a member of the frame class (CMainDlg.cpp).
I WANT TO AVOID THIS!!!
So, use AudioWorkerThread to do all the work:
    i) Own all the artifacts related to WASAPI  (variable and method)
	   --> The MainFrame class should be 'cleaned' of any reference to WASAPI
    ii) initWASAPI sould be called in this thread (not in the MainFrame class)
	iii) The signal generation should be handled in this thread
	iv) This thread should spawn the WASAPI sub thread 
	    (the CWASAPIRenderer has a thread, and is started with  renderer->Start(renderQueue)
		


