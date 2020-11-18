var ferreroApp = angular.module('ferreroApp', ["tw.directives.clickOutside", "ngDragDrop", "ngCookies", "ui.bootstrap", "angular.filter"]);
ferreroApp.controller('FerreroController', function($scope) {
    $scope.arrCategoryList;
    $scope.arrTemplates;
    $scope.swatchData;
    $scope.selectedCategory = "select category";
    $scope.logMessages = "";
    $scope.nTemplateIndex = 0;
    $scope.categoryName = "";
    $scope.categoryId = "";

    $scope.appLoaded = function () 
    {
        console.log("ferreroApp : appLoaded()");
        var csInterface = new CSInterface();
        csInterface.addEventListener("com.ferrero.getcategory.result", $scope.populatecategorydropdown);
        csInterface.addEventListener("com.ferrero.getcategorytemplate.result", $scope.convertemplate);
        csInterface.addEventListener("com.ferrero.log", $scope.addMessageToConsole);
        $scope.sendEventToIllustrator ("com.ferrero.getcategory", "");
        csInterface.addEventListener("com.ferrero.converttemplate.result", $scope.processnexttemplate);
        csInterface.addEventListener("com.ferrero.addswatches", $scope.addSwatch)
 
    };

    $scope.addSwatch = function(event)
    {
        var arrTemplateInfo = event.data.split("~");
        $scope.swatchData = event.data;
        filePath = arrTemplateInfo[0];
        var extendScriptString = "openSwatchFile(\"" + escape(filePath) + "\")";
        var csInterface = new CSInterface();
        csInterface.evalScript(extendScriptString, (msg) => {
            $scope.sendEventToIllustrator ("com.ferrero.uploadTemplate", $scope.swatchData);
          if (msg.data) {
            console.log (msg)
          }});
    }

    $scope.convertemplate = function(event)
    {
        $scope.nTemplateIndex = 0;
        $scope.arrTemplates = event.data.split(",");
        var category = JSON.parse($scope.selectedCategory);
        $scope.categoryName = category.name;
        $scope.categoryId = category.id;
        if($scope.arrTemplates.length > 0 && $scope.arrTemplates[$scope.nTemplateIndex].length > 0)
            $scope.sendEventToIllustrator ("com.ferrero.converttemplate", $scope.categoryName+","+$scope.arrTemplates[$scope.nTemplateIndex]+","+$scope.categoryId);
    }

    $scope.processnexttemplate = function (event)
    {
        $scope.nTemplateIndex++;
        if($scope.nTemplateIndex < $scope.arrTemplates.length)
        {
            var strMessage = $scope.getCurrentDateTime() + " : converted "  + $scope.nTemplateIndex + "  of "+ $scope.arrTemplates.length +" templates\n";
            $scope.logMessages += strMessage;
            $scope.$apply();
            appendToFile(strMessage);
            scrolltobottom();
            $scope.sendEventToIllustrator ("com.ferrero.converttemplate",  $scope.categoryName+","+$scope.arrTemplates[$scope.nTemplateIndex]+","+$scope.categoryId);
   
        }
        else
            $scope.nTemplateIndex = 0;
    }
    
    $scope.populatecategorydropdown = function(event)
    {
        console.log("ferreroApp : populatecategorydropdown()");
        var oResult = event.data.category;
        $scope.arrCategoryList = oResult;
        $scope.$apply();
    }

    $scope.sendEventToIllustrator = function(eventname, eventdata)
    {
        console.log("ferreroApp : sendEventToIllustrator()");
        var csInterface = new CSInterface();
        var oEvent = new CSEvent(eventname, "APPLICATION", "ILST", "Ferrero");
        oEvent.data = eventdata;
        csInterface.dispatchEvent(oEvent);
    }

    $scope.onClickButton = function() 
    {
        console.log("ferreroApp : onClickButton()");
        var csInterface = new CSInterface();
        var category = JSON.parse($scope.selectedCategory);
        csInterface.evalScript("suppressAlerts()", (msg) => {
            $scope.sendEventToIllustrator ("com.ferrero.selectedcategory", category.id + "," + category.name);
          if (msg.data) {
            console.log (msg)
          }});
     }

    $scope.onTemplateButton = function() 
    {

       var strTemplates = document.getElementById("templateid").value;
        $scope.nTemplateIndex = 0;
        $scope.arrTemplates = strTemplates.split(",");
        $scope.categoryName = "NA";
        $scope.categoryId = "";
        if($scope.arrTemplates.length > 0 && $scope.arrTemplates[$scope.nTemplateIndex].length > 0)
            $scope.sendEventToIllustrator ("com.ferrero.converttemplate", $scope.categoryName+","+$scope.arrTemplates[$scope.nTemplateIndex]+","+$scope.categoryId);

    }

     $scope.stopConversion = function()
     {
        $scope.arrTemplates = [];
     }

    $scope.addMessageToConsole = function(event)
    {
        console.log("ferreroApp : addMessageToConsole()");
        var strMessage = $scope.getCurrentDateTime() + " : "  + event.data + "\n";
        $scope.logMessages += strMessage;
        $scope.$apply();
        appendToFile(strMessage);
        scrolltobottom();
    }

    $scope.copyLog = function()
    {
        console.log("ferreroApp : copyLog()");
        copyToClipboard("idConsoleLog");
    }

    $scope.getCurrentDateTime = function()
    {
        console.log("ferreroApp : getCurrentdatetime()");
        var strCurrentDate = new Date().toJSON().slice(0,10);
        var strCurrentTime = new Date().toJSON().slice(11,19)
        var strCurrentDateTime = strCurrentDate + ' ' + strCurrentTime;
        return strCurrentDateTime;
    }

    function copyToClipboard(containerId) 
    {
        console.log("ferreroApp : getCurrentdatetime()");
        var oRange = document.createRange();
        oRange.selectNode(document.getElementById(containerId));
        window.getSelection().addRange(oRange);
        document.execCommand("copy");
        console.log("ferreroApp : getCurrentdatetime() - copied to clipboard.")
    }

    function getTemporaryFolder()
    {
        console.log("ferreroApp : getTemporaryFolder()");
        var csInterface = new CSInterface();
        const ferreroFolder = csInterface.getSystemPath(SystemPath.MY_DOCUMENTS) + '/ferrero';
        const response = window.cep.fs.readdir(ferreroFolder);
        if(response.err !== 0){
            window.cep.fs.makedir(ferreroFolder);
        }
        return ferreroFolder;
    }

    function appendToFile(strLog)
    {
        console.log("ferreroApp : appendToFile()");
        var fs = require('fs');
        const filePath = getTemporaryFolder() + '/ferrero_logs.txt';
        fs.appendFile(filePath, strLog, function (err) {
            if (err) throw err;
            console.log('Saved!');
        });
          
    }

    function scrolltobottom()
    {
        var objDiv = document.getElementById("idConsoleLog");
        objDiv.scrollTop = objDiv.scrollHeight;
    }
 });
