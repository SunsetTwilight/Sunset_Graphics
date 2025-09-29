
rem Array of vcxproj path
set vcxproj_path[0]=DirectXTex\DirectXTex\DirectXTex_Desktop_2022.vcxproj

rem •¶Žš‰»‚¯–hŽ~
set DOTNET_CLI_UI_LANGUAGE=en-US

set I=0

:LOOP
call set Flag=%%vcxproj_path[%I%]%%
if defined Flag (

    msbuild %vcxproj_path[0]%^
		/t:"%1"^
		/p:platform="%2"^
		/p:configuration="%3"

    set /a I=%I% + 1
    goto LOOP
)
