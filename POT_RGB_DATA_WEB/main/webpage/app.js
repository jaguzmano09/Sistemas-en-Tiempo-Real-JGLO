/**
 * Add gobals here
 */
var seconds 	= null;
var otaTimerVar =  null;
var wifiConnectInterval = null;

/**
 * Initialize functions here.
 */
$(document).ready(function(){
	//getUpdateStatus();
	startPOTVoltageInterval();// Start the interval for getting the updated POT voltage values
	startNTCSensorInterval();// Start the interval for getting the updated NTC sensor values
	UpdateSlider_red(); // 
	UpdateSlider_green(); //
	UpdateSlider_blue(); //
	$("#connect_wifi").on("click", function(){
		checkCredentials();
	});
	$("sendRGB").on("click", function(){
		sendRGBValues();
	});
	$("#OFF_Send_UART").on("click", function(){
		turn_off_uart();
	});
	$("#ON_Send_UART").on("click", function(){
		turn_on_uart();
	});
});   

/**
 * Gets file name and size for display on the web page.
 */        
function getFileInfo() 
{
    var x = document.getElementById("selected_file");
    var file = x.files[0];

    document.getElementById("file_info").innerHTML = "<h4>File: " + file.name + "<br>" + "Size: " + file.size + " bytes</h4>";
}

/**
 * Handles the firmware update.
 */
function updateFirmware() 
{
    // Form Data
    var formData = new FormData();
    var fileSelect = document.getElementById("selected_file");
    
    if (fileSelect.files && fileSelect.files.length == 1) 
	{
        var file = fileSelect.files[0];
        formData.set("file", file, file.name);
        document.getElementById("ota_update_status").innerHTML = "Uploading " + file.name + ", Firmware Update in Progress...";

        // Http Request
        var request = new XMLHttpRequest();

        request.upload.addEventListener("progress", updateProgress);
        request.open('POST', "/OTAupdate");
        request.responseType = "blob";
        request.send(formData);
    } 
	else 
	{
        window.alert('Select A File First')
    }
}

/**
 * Progress on transfers from the server to the client (downloads).
 */
function updateProgress(oEvent) 
{
    if (oEvent.lengthComputable) 
	{
        getUpdateStatus();
    } 
	else 
	{
        window.alert('total size is unknown')
    }
}

/**
 * Posts the firmware udpate status.
 */
function getUpdateStatus() 
{
    var xhr = new XMLHttpRequest();
    var requestURL = "/OTAstatus";
    xhr.open('POST', requestURL, false);
    xhr.send('ota_update_status');

    if (xhr.readyState == 4 && xhr.status == 200) 
	{		
        var response = JSON.parse(xhr.responseText);
						
	 	document.getElementById("latest_firmware").innerHTML = response.compile_date + " - " + response.compile_time

		// If flashing was complete it will return a 1, else -1
		// A return of 0 is just for information on the Latest Firmware request
        if (response.ota_update_status == 1) 
		{
    		// Set the countdown timer time
            seconds = 10;
            // Start the countdown timer
            otaRebootTimer();
        } 
        else if (response.ota_update_status == -1)
		{
            document.getElementById("ota_update_status").innerHTML = "!!! Upload Error !!!";
        }
    }
}

/**
 * Displays the reboot countdown.
 */
function otaRebootTimer() 
{	
    document.getElementById("ota_update_status").innerHTML = "OTA Firmware Update Complete. This page will close shortly, Rebooting in: " + seconds;

    if (--seconds == 0) 
	{
        clearTimeout(otaTimerVar);
        window.location.reload();
    } 
	else 
	{
        otaTimerVar = setTimeout(otaRebootTimer, 1000);
    }
}

/**
 * 
 */
function getregValues()
{
	$.getJSON('/read_regs.json', function(data) {
		$("#reg_1").text(data["reg1"]);
		$("#reg_2").text(data["reg2"]);
		$("#reg_3").text(data["reg3"]);
		$("#reg_4").text(data["reg4"]);
		$("#reg_5").text(data["reg5"]);
		$("#reg_6").text(data["reg6"]);
		$("#reg_7").text(data["reg7"]);
		$("#reg_8").text(data["reg8"]);
		$("#reg_9").text(data["reg9"]);
		$("#reg_10").text(data["reg10"]);
	});
}

//MARK: NTC_READING
/**
 * Gets NTC sensor values of temperature for display on the web page.
 */
function getNTCSensorValues()
{
	$.getJSON('/NTCSensor.json', function(data) {
		$("#temperature_reading").text(data["temp"]);
	});
}
/**
 * Sets the interval for getting the updated NTC sensor values of temperature.
 */
function startNTCSensorInterval()
{
	setInterval(getNTCSensorValues, 1000);    
}

//MARK: POT_READING
/**
 * Gets the POT voltage values for display on the web page.
 */
function getPOTVoltage()
{
	$.getJSON('/pot_volt.json', function(data) {
		$("#pot_voltage").text(data["pot_volt"]);
	});
}

/**
 * Sets the interval for getting the updated POT voltage values.
 */
function startPOTVoltageInterval()
{
	setInterval(getPOTVoltage, 500);
}

//MARK: Sliders_rgb
/**
 * Update the slider value in real time for color red.
 */
function UpdateSlider_red() {
    var slider = document.getElementById("red_value");
    var output = document.getElementById("sliderValue_red");
	// Actualizar el valor en tiempo real
	slider.oninput = function() {
		output.innerText = this.value; // Muestra el valor actual
	}
}

/**
 * Update the slider value in real time for color green.
 */
function UpdateSlider_green() {
    var slider = document.getElementById("green_value");
    var output = document.getElementById("sliderValue_green");
	slider.oninput = function() {
		output.innerText = this.value; // Muestra el valor actual
	}
}

/**
 * Update the slider value in real time for color blue.
 */
function UpdateSlider_blue() {
	var slider = document.getElementById("blue_value");
	var output = document.getElementById("sliderValue_blue");
	slider.oninput = function() {
		output.innerText = this.value; // Muestra el valor actual
	}
}

//MARK: RGB_VALUES
/**
 * Send RGB values function.
 */
function sendRGBValues() {
    red = parseInt(document.getElementById("red_value").value);
    green = parseInt(document.getElementById("green_value").value);
    blue = parseInt (document.getElementById("blue_value").value);
	// Create an object to hold the data to be sent in the request body
	var requestData = {
		'red': red,
		'green': green,
		'blue': blue,
		'timestamp': Date.now()
		};
		
	// Serialize the data object to JSON
	var requestDataJSON = JSON.stringify(requestData);
	
	$.ajax({
	  url: '/rgb_values.json', // The URL to post to on the server
	  dataType: 'json', // The data type to be returned from the server
	  method: 'POST', // The HTTP method to use
	  cache: false, // Do not allow caching
	  data: requestDataJSON, // Send the JSON data in the request body
	  contentType: 'application/json', // Set the content type to JSON
	  success: function(response) {
		// Handle the success response from the server
		console.log(response);
	  },
	  error: function(xhr, status, error) {
		// Handle errors
		console.error(xhr.responseText);
	  }
	});

}

//MARK: UART_OFF
/**
 * Turn off uart function.
 */
function turn_off_uart() {
	var errorList = "";
	// Muestra mensaje de éxito en la página
	errorList += "<h4 class='rd'>UART send OFF!</h4>";
	$("#message_uart").html(errorList);
    // Enviar petición AJAX para apagar UART
    $.ajax({
        url: '/toogle_uart_off.json',
        dataType: 'json',
        method: 'POST',
        cache: false,
    });
}

//MARK: UART_ON
/**
 * turn on uart function.
 */
function turn_on_uart()
{
	var errorList = "";
	// Muestra mensaje de éxito en la página
	errorList += "<h4 class='rd'>UART send ON!</h4>";
	$("#message_uart").html(errorList);
	$.ajax({
		url: '/toogle_uart_on.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
	});
}

//MARK: WIFI_CONNECT
/**
 * Clears the connection status interval.
 */
function stopWifiConnectStatusInterval()
{
	if (wifiConnectInterval != null)
	{
		clearInterval(wifiConnectInterval);
		wifiConnectInterval = null;
	}
}

/**
 * Gets the WiFi connection status.
 */
function getWifiConnectStatus()
{
	var xhr = new XMLHttpRequest();
	var requestURL = "/wifiConnectStatus";
	xhr.open('POST', requestURL, false);
	xhr.send('wifi_connect_status');
	
	if (xhr.readyState == 4 && xhr.status == 200)
	{
		var response = JSON.parse(xhr.responseText);
		
		document.getElementById("wifi_connect_status").innerHTML = "Connecting...";
		
		if (response.wifi_connect_status == 2)
		{
			document.getElementById("wifi_connect_status").innerHTML = "<h4 class='rd'>Failed to Connect. Please check your AP credentials and compatibility</h4>";
			stopWifiConnectStatusInterval();
		}
		else if (response.wifi_connect_status == 3)
		{
			document.getElementById("wifi_connect_status").innerHTML = "<h4 class='gr'>Connection Success!</h4>";
			stopWifiConnectStatusInterval();
		}
	}
}

/**
 * Starts the interval for checking the connection status.
 */
function startWifiConnectStatusInterval()
{
	wifiConnectInterval = setInterval(getWifiConnectStatus, 2800);
}

/**
 * Connect WiFi function called using the SSID and password entered into the text fields.
 */
function connectWifi()
{
	selectedSSID = $("#connect_ssid").val();
	pwd = $("#connect_pass").val();
	
	// Create an object to hold the data to be sent in the request body
	var requestData = {
	  'selectedSSID': selectedSSID,
	  'pwd': pwd,
	  'timestamp': Date.now()
	};
	
	// Serialize the data object to JSON
	var requestDataJSON = JSON.stringify(requestData);
	
	$.ajax({
	  url: '/wifiConnect.json',
	  dataType: 'json',
	  method: 'POST',
	  cache: false,
	  data: requestDataJSON, // Send the JSON data in the request body
	  contentType: 'application/json', // Set the content type to JSON
	  success: function(response) {
		// Handle the success response from the server
		console.log(response);
		if (response.status === "success") {
			$("#wifi-status").text(response.message).css("color", "green");
		}
	  },
	  error: function(xhr, status, error) {
		// Handle errors
		console.error(xhr.responseText);
	  }
	});
}

/**
 * Checks credentials on connect_wifi button click.
 */
function checkCredentials()
{
	errorList = "";
	credsOk = true;
	
	selectedSSID = $("#connect_ssid").val();
	pwd = $("#connect_pass").val();
	
	if (selectedSSID == "")
	{
		errorList += "<h4 class='rd'>SSID cannot be empty!</h4>";
		credsOk = false;
	}
	if (pwd == "")
	{
		errorList += "<h4 class='rd'>Password cannot be empty!</h4>";
		credsOk = false;
	}
	
	if (credsOk == false)
	{
		$("#wifi_connect_credentials_errors").html(errorList);
	}
	else
	{
		$("#wifi_connect_credentials_errors").html("");
		connectWifi();    
	}
}

/**
 * Shows the WiFi password if the box is checked.
 */
function showPassword()
{
	var x = document.getElementById("connect_pass");
	if (x.type === "password")
	{
		x.type = "text";
	}
	else
	{
		x.type = "password";
	}
}

//MARK: REGISTERS
function send_register()
{
    // Assuming you have selectedNumber, hours, minutes variables populated from your form
    selectedNumber = $("#selectNumber").val();
    hours = $("#hours").val();
    minutes = $("#minutes").val();
    
    // Create an array for selected days
    var selectedDays = [];
    if ($("#day_mon").prop("checked")) selectedDays.push("1");
	else selectedDays.push("0");
    if ($("#day_tue").prop("checked")) selectedDays.push("1");
	else selectedDays.push("0");
    if ($("#day_wed").prop("checked")) selectedDays.push("1");
	else selectedDays.push("0");
    if ($("#day_thu").prop("checked")) selectedDays.push("1");
	else selectedDays.push("0");
    if ($("#day_fri").prop("checked")) selectedDays.push("1");
	else selectedDays.push("0");
    if ($("#day_sat").prop("checked")) selectedDays.push("1");
	else selectedDays.push("0");
    if ($("#day_sun").prop("checked")) selectedDays.push("1");
	else selectedDays.push("0");

    // Create an object to hold the data to be sent in the request body
    var requestData = {
        'selectedNumber': selectedNumber,
        'hours': hours,
        'minutes': minutes,
        'selectedDays': selectedDays,
        'timestamp': Date.now()
    };

    // Serialize the data object to JSON
    var requestDataJSON = JSON.stringify(requestData);

	$.ajax({
		url: '/regchange.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
		data: requestDataJSON, // Send the JSON data in the request body
		contentType: 'application/json', // Set the content type to JSON
		success: function(response) {
		  // Handle the success response from the server
		  console.log(response);
		},
		error: function(xhr, status, error) {
		  // Handle errors
		  console.error(xhr.responseText);
		}
	  });

    // Print the resulting JSON to the console (for testing)
    //console.log(requestDataJSON);
}

/**
 * Reads the register values.
 */
function read_reg()
{

	
	$.ajax({
		url: '/readreg.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
		//headers: {'my-connect-ssid': selectedSSID, 'my-connect-pwd': pwd},
		//data: {'timestamp': Date.now()}
	});
//	var xhr = new XMLHttpRequest();
//	xhr.open("POST", "/toogle_led.json");
//	xhr.setRequestHeader("Content-Type", "application/json");
//	xhr.send(JSON.stringify({data: "mi información"}));
}


function erase_register()
{
    // Assuming you have selectedNumber, hours, minutes variables populated from your form
    selectedNumber = $("#selectNumber").val();

    // Create an object to hold the data to be sent in the request body
    var requestData = {
        'selectedNumber': selectedNumber,
        'timestamp': Date.now()
    };

    // Serialize the data object to JSON
    var requestDataJSON = JSON.stringify(requestData);

	$.ajax({
		url: '/regchange.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
		data: requestDataJSON, // Send the JSON data in the request body
		contentType: 'application/json', // Set the content type to JSON
		success: function(response) {
		  // Handle the success response from the server
		  console.log(response);
		},
		error: function(xhr, status, error) {
		  // Handle errors
		  console.error(xhr.responseText);
		}
	  });
}

//MARK: LED
function toogle_led() 
{	
	$.ajax({
		url: '/toogle_led.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
	});

}

function brigthness_up() 
{	
	$.ajax({
		url: '/toogle_led.json',
		dataType: 'json',
		method: 'POST',
		cache: false,
	});

}











    










    


