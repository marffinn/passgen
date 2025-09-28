@echo off
setlocal enabledelayedexpansion

echo [INFO] Cleaning up old GitHub releases
echo ======================================

REM Get the 3 most recent release tags
set "COUNT=0"
for /f "tokens=3" %%a in ('gh release list --limit 3') do (
    set /a "COUNT+=1"
    set "KEEP_TAG_!COUNT!=%%a"
)

echo [INFO] Keeping the 3 most recent releases:
for /l %%i in (1,1,!COUNT!) do (
    echo [INFO]   - !KEEP_TAG_%%i!
)
echo [INFO] Deleting all other releases...

REM Get all release tags except the 3 most recent
for /f "tokens=3" %%a in ('gh release list --limit 100') do (
    set "TAG=%%a"
    set "DELETE=true"
    
    REM Check if this tag is one of the 3 to keep
    for /l %%i in (1,1,!COUNT!) do (
        if "!TAG!"=="!KEEP_TAG_%%i!" (
            set "DELETE=false"
        )
    )
    
    if "!DELETE!"=="true" (
        echo [INFO] Deleting release: !TAG!
        gh release delete !TAG! --yes
        
        REM Also delete the tag
        echo [INFO] Deleting tag: !TAG!
        git tag -d !TAG! 2>nul
        git push origin :refs/tags/!TAG! 2>nul
    )
)

echo [SUCCESS] Cleanup complete!
echo [INFO] Only the 3 most recent releases remain.