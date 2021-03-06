const CONFIG_OPT    = "config";
const YDEV_OPT      = "ydev";
const FIFO_SIZE     = 10;

var devNameLabel         = document.getElementById("devNameLabel");
var webPageTitle         = document.getElementById("webPageTitle");
var devNameText          = document.getElementById("devNameText");
var groupNameText        = document.getElementById("groupNameText");
var syslogEnableCheckbox = document.getElementById("syslogEnableCheckbox");

var tab2Button           = document.getElementById("tab2Button");
var setConfigButton      = document.getElementById("setConfigButton");
var setDefaultsButton    = document.getElementById("setDefaultsButton");
var rebootButton         = document.getElementById("rebootButton");
var cmdHistoryDiv        = document.getElementById("cmd_history");

var storageVoltageCB     = document.getElementById("storageVoltageCB");
var chargeLevel5         = document.getElementById("chargeLevel5");
var chargeLevel4         = document.getElementById("chargeLevel4");
var chargeLevel3         = document.getElementById("chargeLevel3");
var chargeLevel2         = document.getElementById("chargeLevel2");
var chargeLevel1         = document.getElementById("chargeLevel1");

var batVoltageText       = document.getElementById("batVoltage");
var batAmpsText          = document.getElementById("batAmps");
var chargePowerText      = document.getElementById("chargePower");
var batteryTempCText     = document.getElementById("tempC");
var chargeStateText      = document.getElementById("chargeState");
var warningField         = document.getElementById("warningField");

var deviceConfig = {}; //Holds the device configuration and is loaded
                       //when updateConfigState() is called.

/**
 * @brief A USerOutput class responsible for displaying log messages
 **/
class UO {
   /**
    * @brief constructor
    * @param debugEnabled If True debug messages are displayed.
    **/
   constructor(debugEnabled) {
       this.debugEnabled=debugEnabled;
   }

   /**
    * @brief Display an info level message.
    * @param msg The message text to be displayed on the console.
    **/
   info(msg) {
       console.log("INFO:  "+msg);
   }

   /**
    * @brief Display a warning level message.
    * @param msg The message text to be displayed on the console.
    **/
   warn(msg) {
       console.log("WARN:  "+msg);
   }

   /**
    * @brief Display a error level message.
    * @param msg The message text to be displayed on the console.
    **/
   error(msg) {
       console.log("ERROR: "+msg);
   }

   /**
    * @brief Display a debug level message if the constructor argument was true.
    * @param msg The message text to be displayed on the console.
    **/
   debug(msg) {
       if( this.debugEnabled ) {
           console.log("DEBUG: "+msg);
       }
   }
}
var uo = new UO(true);

// @brief A first in first out memory unit
class FiFo {
  // @brief Constructor
  // @param maxLength The max length of the FiFo. If 0 or less then
  //                  no max length is unlimited.
  constructor(maxLength) {
        this.maxLength = maxLength;
        this._fifo = Array();
  }

  // @brief Push an object into the Fifo.
  // If the Fifo is full then remove the last object from the fifo before
  // pushing the object in.
  // @return null if there was space in the FiFo to push the object in.
  //         If not null then return the object removed from the FiFo in
  //         order to fit the new object into the FiFo.
  push(object) {
        var ejectedObject = null;
        if( this.maxSize <= 0 ) {
                this._fifo.push(object);
        }
        // If max fifo size reached remove an object from the fifo
        if( this.maxLength > 0 && this._fifo.length == this.maxLength ) {
                ejectedObject = this.pop();
        }
        this._fifo.push(object);
        return ejectedObject;
  }

  // @return A value from the FiFo or return null/undefined if empty.
  pop() {
        return this._fifo.shift();
  }

  // @return The number of elements in the fifo.
  length() {
        return this._fifo.length;
  }
  
  // @return The entire contents of the FiFo as an array of objects. 
  contents() {
    return this._fifo;
  }
}

// We use FiFo's' to hold the amps and volts so that the value is smoothed as the 
// battery nears full charge. In this state the charge is turned on and off
// periodically, therefore smoothin is useful for the value displayed to the user.
var ampsFiFo = new FiFo(FIFO_SIZE);
var voltsFiFo = new FiFo(FIFO_SIZE);

/**
 * @brief log text to the log message area on the page.
 * @param msg
 *            The text message to be displayed.
 */
var logCmd = function(msg) {
    var curentLog = cmdHistoryDiv.innerHTML;
    var now = new Date();
    var dataTimeString = now.toUTCString();
    msg = dataTimeString+": "+msg;
    var newLog = msg + "<BR>" + curentLog;
    cmdHistoryDiv.innerHTML = newLog;
};

/**
 * @brief Update the config state.
 **/
function updateConfigState() {
  uo.debug("updateConfigState()");
  $.ajax({
    url: '/rpc/Config.Get',
    success: function(data) {
    deviceConfig = data;

    //This shows all the config options available on the device.
    console.log(data);
    //Get the subsection specific to yView devices.
    var yDevConfig = data['ydev'];

    devNameText.value = yDevConfig["unit_name"];
    devNameLabel.innerHTML = devNameText.value;
    document.title = devNameText.value;
    groupNameText.value = yDevConfig["group_name"];
    syslogEnableCheckbox.checked = yDevConfig['enable_syslog'];
    
    var yDevConfig = data['batmon'];
    var batChargeVoltageID = yDevConfig["charge_level"];
    if( batChargeVoltageID == 5 ) {
        chargeLevel5.checked=true;
    }
    else if( batChargeVoltageID == 4 ) {
        chargeLevel4.checked=true;
    }
    else if( batChargeVoltageID == 3 ) {
        chargeLevel3.checked=true;
    }
    else if( batChargeVoltageID == 2 ) {
        chargeLevel2.checked=true;
    }
    else if( batChargeVoltageID == 1 ) {
        chargeLevel1.checked=true;
    }
    else {
      uo.error("Invalid max_bat_charge_voltage_id: "+rfLevel);
    }

    },
  });
}

// @brief Get the mean value of all values held in the fifo.
// @param fifo The fifo whose contents are averaged.
// @return The average value to two decimal places.
function getMean(fifo) {
    retValue = 0.0;
    if( fifo.length() > 0 ) {
        valueList = fifo.contents();
        total = 0;
        for( let index in valueList ) {
            total += valueList[index];
        }
        mean = total/valueList.length;
        retValue = mean.toFixed(2);
     }
     return retValue;
}


function updateState() {
    uo.debug("updateState()");
    $.ajax({
      url: '/rpc/get_state',
      success: function(data) {
          deviceState = data;
          //This shows all the config options available on the device.
          console.log(data);

          // Push the battery volts value into the FiFo
          voltsFiFo.push(data['battery_voltage']);
          // Push the current amps value into the FiFo
          ampsFiFo.push(data['amps']);
          
          // Get the average amps value held in the FiFo
          var amps = getMean(ampsFiFo);
          var charging = true;
          if( amps <= 0.0 ) {
            charging = false;
          }
          
          var battery_voltage = getMean(voltsFiFo);
          var watts = (amps * battery_voltage).toFixed(2);
          var tempC = data['tempC'];
          var fully_charged_voltage = data["fully_charged_voltage"];
          var battery_cell_count = data["battery_cell_count"];
          var warning_message = data["warning_message"];
          var last_on_charge_voltage = data["last_on_charge_voltage"];
          
          // If we have more than one cell in the battery and are still charging the battery set the tool tip
          // to indicate the volts per cell.
          if( battery_cell_count > 0 ) {
              volts_per_cell = last_on_charge_voltage / battery_cell_count;
              batVoltageText.title = "Battery pack is charged to "+volts_per_cell.toFixed(2) + " volts per cell."
          }

          batAmpsText.value = amps;
          // Display the current battery voltage
          batVoltageText.value = battery_voltage;

          chargePowerText.value = watts;
          batteryTempCText.value = tempC;

          if( charging ) {
              chargeState.value="Charging";
              chargeStateText.title = "Waiting for battery to reach " + fully_charged_voltage + " volts.";
              chargeStateText.style.backgroundColor = "red";
          }
          else {
              chargeState.value="Fully Charged";
              chargeStateText.title = "The battery has reached a charge voltage of " + fully_charged_voltage + " volts.";
              chargeStateText.style.backgroundColor = "green";
          }

          uo.debug("warning_message.length = "+  warning_message.length);
          // Display warning to user.
          if( warning_message.length > 0 ) {
              warningField.removeAttribute("hidden");
              warningField.style.backgroundColor = "white";
              warningField.style.color = "red";
              warningField.innerHTML = warning_message;
              uo.debug(warning_message);
          }
          else {
              warningField.setAttribute("hidden", true);
              warningField.innerHTML = "";
          }
      },
    });
    
    var sCBDisabled = false;
    if( storageVoltageCB.checked ) {
        sCBDisabled = true;
    }
    chargeLevel5.disabled = sCBDisabled;
    chargeLevel4.disabled = sCBDisabled;
    chargeLevel3.disabled = sCBDisabled;
    chargeLevel2.disabled = sCBDisabled;
    chargeLevel1.disabled = sCBDisabled;
}

/**
 * @brief Called when the Save button is selected.
 **/
function setDeviceConfig() {
  uo.debug("setDeviceConfig()");
  
  var batChargeVoltageID = 0;

  if( chargeLevel5.checked ) {
      batChargeVoltageID=5;
  }
  else if( chargeLevel4.checked ) {
      batChargeVoltageID=4;
  }
  else if( chargeLevel3.checked ) {
      batChargeVoltageID=3;
  }
  else if( chargeLevel2.checked ) {
      batChargeVoltageID=2;
  }
  else if( chargeLevel1.checked ) {
      batChargeVoltageID=1;
  }
  else {
      batChargeVoltageID = 0;
  }
  
  let configData = {};
  configData[CONFIG_OPT] = {};
  configData[CONFIG_OPT][YDEV_OPT] = {};
  configData[CONFIG_OPT][YDEV_OPT]["unit_name"]     = devNameText.value;
  configData[CONFIG_OPT][YDEV_OPT]["group_name"]    = groupNameText.value;
  configData[CONFIG_OPT][YDEV_OPT]["enable_syslog"] = syslogEnableCheckbox.checked;
  configData[CONFIG_OPT]["batmon"] = {}
  if( storageVoltageCB.checked ) {
      // A charge level of -1 indicates that the storage charge level checkbox has been selected.
      configData[CONFIG_OPT]["batmon"]["charge_level"] = -1;
  }
  else {
      configData[CONFIG_OPT]["batmon"]["charge_level"] = batChargeVoltageID;
  }
  let jsonStr = JSON.stringify(configData);
  uo.debug("jsonStr="+jsonStr);

  $.ajax({
    url: '/rpc/config.set',
    data: jsonStr,
    type: 'POST',
    success: function(data) {
      uo.debug("Set config success");

      //Now save the config to a file.
      $.ajax({
        url: '/RPC/Config.Save',
        data: jsonStr,
        type: 'POST',
        success: function(data) {
          uo.debug("Save config success");

          //Now update the syslog state.
          $.ajax({
            url: '/rpc/update_syslog',
            type: 'POST',
            success: function(data) {
              uo.debug("Update syslog success");
            },
          })

        },
      })
    },
  })
  logCmd("Updated configuration.");
}

/**
 * @brief Called when the factory defaults button is selected.
 **/
function setDefaults() {
  uo.debug("setDefaults()");
  if( confirm("Are you sure that you wish to set the device configuration to factory defaults and reboot ?") ) {
      $.ajax({
      url: '/rpc/ydev.factorydefault',
      type: 'POST',
      success: function(data) {
        uo.debug("Set factory default config success");
        alert("The device is now rebooting.");
        document.body.style.cursor = 'wait';
        setTimeout( location.reload() , 1000);
        logCmd("Set device factory defaults.");
      },
    })
  }
}

/**
 * @brief Called when the Reboot button is selected.
 **/
function reboot() {
  uo.debug("reboot()");
  if( confirm("Are you sure that you wish to reboot the device ?") ) {
    $.ajax({
      url: '/rpc/sys.reboot',
      type: 'POST',
      success: function(data) {
        uo.debug("Reboot success");
        alert("The device is now rebooting.");
        document.body.style.cursor = 'wait';
        setTimeout( location.reload() , 1000);
        logCmd("Rebooted device.");
      },
    })
  }
}

/**
 * @brief Called when a tab (in product.html) is selected.
 **/
function selectTab(evt, cityName) {
  uo.debug("selectTab()");
  var i, tabcontent, tablinks;
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
  tabcontent[i].style.display = "none";
  }
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
  tablinks[i].className = tablinks[i].className.replace(" active", "");
  }
  document.getElementById(cityName).style.display = "block";
  evt.currentTarget.className += " active";
}

/**
 * Called at intervals to update the voltage, current, watts and temp on the first tab.
 * 
 * @returns null
 */
setInterval(function() {
    updateState(false); 
}, 1000);

/**
 * @brief Called when the page is loaded.
 **/
window.onload = function(e){
  uo.debug("window.onload()");
  updateConfigState();

  setConfigButton.addEventListener("click", setDeviceConfig);
  setDefaultsButton.addEventListener("click", setDefaults);
  rebootButton.addEventListener("click", reboot);

  chargeStateText.style.backgroundColor = "red";
  
  uo = new UO(true);
  ampsFiFo = new FiFo(10);

}

//Select the required tab when the product.html page is loaded.
document.getElementById("defaultOpen").click();
