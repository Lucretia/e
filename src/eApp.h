/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __EAPP_H__
#define __EAPP_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dynlib.h>

#ifdef __WXDEBUG__
#include <wx/ffile.h>
#endif

#include "eIpcServer.h"
#include "eSettings.h"
#include "IAppPaths.h"
#include "IExecuteAppCommand.h"

#include <map>
#include <boost/ptr_container/ptr_map.hpp>

// Pre-definitions
class wxSingleInstanceChecker;
class TmSyntaxHandler;
class PListHandler;
class EditorFrame;
class AppVersion;
class EditorCtrl;
class ApiHandler;

class eApp : public wxApp, 
	public IAppPaths, 
	public IExecuteAppCommand
{
public:
	virtual bool OnInit();
	int OnExit();
	void CatalystCommit();

	const wxString& VersionName() const { return m_version_name; }
	unsigned int VersionId() const { return m_version_id; }

	// Frames
	EditorFrame* NewFrame();
	void CloseAllFrames();
	bool IsLastFrame() const;
	EditorFrame* GetTopFrame() const;

	// Editors
	EditorCtrl* GetActiveEditorCtrl() const;
	EditorCtrl* GetEditorCtrl(int winId) const;

	// Handlers
	TmSyntaxHandler& GetSyntaxHandler() {return *m_pSyntaxHandler;};

	// Execute internal commands
	virtual bool ExecuteCmd(const wxString& cmd);
	virtual bool ExecuteCmd(const wxString& cmd, wxString& result);

	virtual const wxString& AppPath() const;
	virtual const wxString& AppDataPath() const;
	virtual wxString CreateTempAppDataFile();

	// Settings functions
	const wxLongLong& GetId() const;
	eSettings& GetSettings(){return m_settings;};

	const wxString& GetAppTitle();

	// Registration functions
	bool IsRegistered() const;
	int DaysLeftOfTrial() const;
	int TotalDays() const;
	const wxString& RegisteredUserName() const;
	const wxString& RegisteredUserEmail() const;

    // Suppress default console handling even if wxWidgets was compiled with --enable-cmdline
#if wxUSE_CMDLINE_PARSER
    virtual bool OnCmdLineParsed(wxCmdLineParser&) {return true;}
    virtual bool OnCmdLineHelp(wxCmdLineParser&) {return true;}
    virtual bool OnCmdLineError(wxCmdLineParser&) {return true;}
#endif // wxUSE_CMDLINE_PARSER
        
#ifdef __WXDEBUG__
	void OnAssertFailure(const wxChar *file, int line, const wxChar *cond, const wxChar *msg);
#endif  //__WXDEBUG__

	// Api notifications
	void OnInputLineChanged(unsigned int nid, const wxString& text);
	void OnInputLineClosed(unsigned int nid);

private:
	// Frames
	EditorFrame* OpenFrame(size_t frameId);
	void CheckForModifiedFiles();

	// Member variables
	wxString m_version_name;
	unsigned int m_version_id;

	// Member functions
	void ClearState();
	void ClearLayout();

	bool SendArgsToInstance();

#ifdef __WXMSW__
	// Callback for ASProtect to set days left
	static void __declspec(dllexport) __stdcall GetTrialDays(int Total, int Left);
#endif

	// Event handlers
	void OnUpdatesAvailable(wxCommandEvent& event);
	void OnUpdatesChecked(wxCommandEvent& event);
	void OnIdle(wxIdleEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	wxDynamicLibrary blackboxLib;
	wxSingleInstanceChecker* m_checker;
	Catalyst* m_pCatalyst;
	CatalystWrapper* m_catalyst;
	TmSyntaxHandler* m_pSyntaxHandler;
	PListHandler* m_pListHandler;
	wxString m_appPath;
	wxString m_appDataPath;
	wxArrayString m_openStack;
	ApiHandler* m_apiHandler;

#ifndef __WXMSW__
	eIpcServer* m_server;
#endif

	// Option vars
	wxArrayString m_files;
	unsigned long m_lineNum;
	unsigned long m_columnNum;

	// Settings
	eSettings m_settings;

#ifdef __WXDEBUG__
	wxFFile m_logFile;
#endif //__WXDEBUG__
};


DECLARE_APP(eApp)

#ifndef __WXMSW__
class eClient: public wxClient {
public:
	eClient(eApp& app) : app(app) {}
	wxConnectionBase *OnMakeConnection(void) { return new eConnection(app); }

private:
	eApp& app;
};
#endif


#endif // __EAPP_H__
