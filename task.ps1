$msbuild = "C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe"
Write-Host "Current version: " -n -f ([ConsoleColor]::Green)
Write-Host (Get-Content ".\skin_definition.json" | ConvertFrom-Json).version

function Dist
{
    param (
        [Parameter(Mandatory = $true)][int16]$major,
        [Parameter(Mandatory = $true)][int16]$minor,
        [Parameter(Mandatory = $true)][int16]$patch
    )
    Remove-Item -Recurse .\dist -ErrorAction SilentlyContinue

    $ver = "$($major).$($minor).$($patch)"
    BumpVersion $ver

    New-Item -ItemType directory .\dist -ErrorAction SilentlyContinue
    New-Item -ItemType directory .\dist\x64 -ErrorAction SilentlyContinue
    New-Item -ItemType directory .\dist\x86 -ErrorAction SilentlyContinue
    Copy-Item .\build\x64\WebView.dll .\dist\x64
    Copy-Item .\build\x86\WebView.dll .\dist\x86

    Compress-Archive -Path .\dist\x64, .\dist\x86 -DestinationPath ".\dist\PluginWebView_$($ver)_x64_x86_dll.zip"

    SkinPackager.exe .\skin_definition.json
}

function BumpVersion
{
    param (
        [Parameter(Mandatory = $true)][string]$ver
    )

    $verComma = $ver -replace "\.", ","
    $rc = Get-Content ".\PluginWebView.rc" -Encoding UTF8
    $rc = $rc -replace "FILEVERSION [\d,]*", "FILEVERSION $verComma,0"
    $rc = $rc -replace "STRPRODUCTVER `"[\d.]*`"", "STRPRODUCTVER `"$ver`""
    
    $rc | Set-Content "./PluginWebView.rc" -Encoding UTF8

    $skinDef = @{
        name = "PluginWebView Example Skins"
        author = "khanhas"
        version = $ver
        minimumVersion = "4.5"
        minimumWindows = "5.1"
        output = ".\dist\PluginWebView_$ver.rmskin"
        skinDir = ".\examples"
        configPrefix = "PluginWebViewExample"
        headerImage = (Get-Location).Path + "\banner.bmp"
        loadType = "Skin"
        load = "PluginWebViewExample\weather\weather.ini"
        variableFiles = ""
        plugins = @(
            @{
                name = "WebView.dll"
                x86 = (Get-Location).Path + "\dist\x86\WebView.dll"
                x64 = (Get-Location).Path + "\dist\x64\WebView.dll"
            }
        )
    }

    $skinDef | ConvertTo-Json | Set-Content ".\skin_definition.json" -Encoding UTF8
}
