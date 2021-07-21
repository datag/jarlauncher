#include <windows.h>
#include "config.h"

#define BUFSIZE	(MAX_PATH + 1)

typedef BOOL (WINAPI *W64FSREDIR)(PVOID*);
void ToggleWow64FsRedirection();

int APIENTRY WinMain(HINSTANCE	hInstance,
					 HINSTANCE	hPrevInstance,
					 LPSTR		lpCmdLine,
					 int		nCmdShow)
{
	LPSTR cmd = JAVA_BINARY;
	LPSTR param = JAVA_VM_OPTS " -jar " JAVA_APP " " JAVA_APP_OPTS;
	LPSTR workdir = NULL;
	
	DWORD dwRet;
	HINSTANCE hAppInstance;
	TCHAR path[BUFSIZE];
	
	// use environment variable JAVA_HOME, if set
	if (dwRet = GetEnvironmentVariable("JAVA_HOME", path, BUFSIZE)) {
		if (dwRet + 5 /* "\bin\" */ + lstrlen(cmd) < BUFSIZE) {
			// append "\bin" and command
			lstrcat(path, "\\bin\\");
			lstrcat(path, cmd);
			
			hAppInstance = ShellExecute(0, "open", path, param, workdir, SW_SHOWNORMAL);
			if ((int)hAppInstance <= 32) {
				MessageBox(NULL, "Error launching " JAVA_APP_NAME " by using "
					"JAVA_HOME environment variable. Make sure the variable is pointing "
					"to the correct Java installation and try again.",
					"Error launching " JAVA_APP_NAME,
					MB_OK | MB_ICONERROR);
				
				ExitProcess(1);
			}
		} else {
			MessageBox(NULL, "Error launching " JAVA_APP_NAME " because path in "
				"JAVA_HOME environment variable is too long.",
				"Error launching " JAVA_APP_NAME,
				MB_OK | MB_ICONERROR);
				
			ExitProcess(3);
		}
	} else {
		// first try: native
		hAppInstance = ShellExecute(0, "open", cmd, param, workdir, SW_SHOWNORMAL);
		if ((int)hAppInstance <= 32) {
			// seconds try: disabling file system redirection (64-bit Java, JarLauncher is 32-bit)
			ToggleWow64FsRedirection(1);
			hAppInstance = ShellExecute(0, "open", cmd, param, workdir, SW_SHOWNORMAL);
			ToggleWow64FsRedirection(0);
			
			if ((int)hAppInstance <= 32) {
				// last try: via file association
				hAppInstance = ShellExecute(0, "open", JAVA_APP, JAVA_APP_OPTS, workdir, SW_SHOWNORMAL);
			}
		}
		
		if ((int)hAppInstance <= 32) {
			int response = MessageBox(NULL, "Error launching " JAVA_APP_NAME ". "
					"It is most likely that there is no Java installation on your system.\r\n\r\n"
					"Do you want to open the Java download page in your default web browser?",
					"Error launching " JAVA_APP_NAME,
					MB_YESNO | MB_ICONEXCLAMATION);

			if (response == IDYES) {
				// run default action for hyperlink
				ShellExecute(NULL, "open", JAVA_URL, NULL, NULL, SW_SHOWNORMAL);
			}
			
			ExitProcess(1);
		}
	}
	
	return 0;
}

void ToggleWow64FsRedirection(int disable)
{
	W64FSREDIR pW64FSREDIR;
	PVOID *pOldValue;

	pW64FSREDIR = (W64FSREDIR) GetProcAddress(
		GetModuleHandle(TEXT("kernel32.dll")), 
		disable ? "Wow64DisableWow64FsRedirection" : "Wow64RevertWow64FsRedirection");
	
	if (pW64FSREDIR != NULL) {
		pW64FSREDIR(pOldValue);
	}
}
