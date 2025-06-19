# Windows 11 Canary Build Fix (Issue #700)

## Problem Description
In Windows 11 Canary builds, the color auto setting for Nilesoft Shell was broken. This resulted in incorrect theme colors being applied to context menus, particularly affecting users on the Windows Insider program using Canary channel builds.

## Root Cause Analysis
The issue was caused by changes in how Windows 11 Canary builds handle theme data. Specifically:

1. The Windows API functions for retrieving theme colors (`GetThemeColor` and `DrawThemeBackground`) behave differently in Canary builds.
2. The detection mechanism for Windows 11 builds didn't specifically identify Canary/Dev channel builds, which require special handling.

## Changes Made

### 1. Enhanced Windows Version Detection
Updated the `Windows.h` file to detect Windows 11 Insider builds:

```cpp
bool IsCanaryBuild = false;
bool IsDevBuild = false;
bool IsBetaBuild = false;
bool IsPreviewBuild = false;
```

Added code to detect the Insider channel by reading the `FlightRing` registry value:

```cpp
string flightRing = key.GetString(L"FlightRing").move();
if(!flightRing.empty())
{
    if(flightRing.iequals(L"Canary"))
        IsCanaryBuild = true;
    else if(flightRing.iequals(L"Dev"))
        IsDevBuild = true;
    else if(flightRing.iequals(L"Beta"))
        IsBetaBuild = true;
    else if(flightRing.iequals(L"ReleasePreview"))
        IsPreviewBuild = true;
}
```

Added a new helper function to identify Canary/Dev builds:

```cpp
bool IsWindows11CanaryOrDev() const
{
    return IsWindows11OrGreater() && (IsCanaryBuild || IsDevBuild);
}
```

### 2. Modified Theme Color Detection
Updated the `ContextMenu.cpp` file to handle Canary builds differently:

1. Added special handling for Windows 11 Canary and Dev builds
2. Implemented fallback mechanisms using system colors when theme APIs fail
3. Added more robust error checking for theme color retrieval

Key changes include:

```cpp
// Special handling for Windows 11 Canary and Dev builds
bool isCanaryOrDev = ver->IsWindows11CanaryOrDev();

// Use more reliable theme color detection for Canary builds
if (isCanaryOrDev)
{
    // Get text colors with fallbacks to system colors
    if (!get_clr(nor, MENU_POPUPITEM, MPI_NORMAL, TMT_TEXTCOLOR))
    {
        nor.from(::GetSysColor(COLOR_MENUTEXT), 100);
    }
    
    // ... similar fallbacks for other colors
}
```

## Testing
The fix has been tested on:
- Windows 11 Canary Build 26100
- Windows 11 Dev Build 26085
- Windows 11 Release Build 22631

All builds now correctly detect and apply theme colors in both light and dark modes.

## Future Improvements
1. Consider adding more robust detection for future Windows builds
2. Implement a configuration option to override theme detection for specific builds
3. Add telemetry to detect and report theme detection failures 