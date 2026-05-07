#define MyAppName "Echoes of the Forgotten Keep"
#define MyAppVersion "1.0"
#define MyAppExeName "echoes_of_the_forgotten_keep.exe"
#define MyAppPublisher "Joseph Driedger"
#define BinDir "cmake-build-release\bin"
#define MingwDir "C:\Program Files\JetBrains\CLion 2025.3.1\bin\mingw\bin"

[Setup]
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=installer-output
OutputBaseFilename=EchoesOfTheForgottenKeep-Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
UninstallDisplayIcon={app}\{#MyAppExeName}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"

[Files]
; Executable
Source: "{#BinDir}\{#MyAppExeName}";         DestDir: "{app}"; Flags: ignoreversion

; SDL3 and other runtime DLLs
Source: "{#BinDir}\SDL3.dll";                DestDir: "{app}"; Flags: ignoreversion
Source: "{#BinDir}\SDL3_image.dll";          DestDir: "{app}"; Flags: ignoreversion
Source: "{#BinDir}\SDL3_mixer.dll";          DestDir: "{app}"; Flags: ignoreversion
Source: "{#BinDir}\libassimp-6.dll";         DestDir: "{app}"; Flags: ignoreversion

; MinGW runtime DLLs
Source: "{#MingwDir}\libgcc_s_seh-1.dll";    DestDir: "{app}"; Flags: ignoreversion
Source: "{#MingwDir}\libstdc++-6.dll";       DestDir: "{app}"; Flags: ignoreversion
Source: "{#MingwDir}\libwinpthread-1.dll";   DestDir: "{app}"; Flags: ignoreversion

; Assets
Source: "{#BinDir}\asset\*"; DestDir: "{app}\asset"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}";          Filename: "{app}\{#MyAppExeName}"
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}";  Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent
