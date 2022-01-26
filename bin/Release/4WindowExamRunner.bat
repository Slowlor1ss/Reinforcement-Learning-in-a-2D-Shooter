start GPP_Framework.exe 30 30

start GPP_Framework.exe 1000 30

start GPP_Framework.exe 30 540

start GPP_Framework.exe 1000 540
echo new ActiveXObject("WScript.Shell").AppActivate("GPP_Framework.exe"); > tmp.js
cscript //nologo tmp.js & del tmp.js