$.appEncoding = "UTF-8";
hostApplication = app.name;

function loadJSXFile(scriptFilePath){
    sendDebug("jsx: in loadJSXFile()");
	//Leave this as unescape(), not processMessage().
	scriptFilePath = unescape(scriptFilePath);
	var scriptFile = File(scriptFilePath);
	if(scriptFile.exists){
		sendDebug("jsx: loadJSXFile(): About to evalFile() " + scriptFilePath);
		$.evalFile(scriptFile);
		sendDebug("jsx: loadJSXFile(): Successfully loaded " + scriptFilePath);
	}
	else{
		sendDebug("jsx: loadJSXFile(): File not found " + scriptFilePath);
	}
}

function sendCSXSEvent(type, data){
	var csxsEvent = new CSXSEvent(); 
	csxsEvent.type = type; 
	csxsEvent.data = data; 
	csxsEvent.dispatch();
}

function sendDebug(string){
	sendCSXSEvent("idDebug", string);
}

function suppressAlerts()
{
     app.userInteractionLevel = UserInteractionLevel.DONTDISPLAYALERTS;
}

function openSwatchFile(swatchFilePath){
 //   alert("jsx: in openSwatchFile()"+ swatchFilePath);
	//Leave this as unescape(), not processMessage().
	swatchFilePath = unescape(swatchFilePath);
	var oFile = new File(swatchFilePath);
	var openOpt = new OpenOptions();
	//  get swatches
	openOpt.openAs=LibraryType.SWATCHES;
	open(oFile,null,openOpt);
}